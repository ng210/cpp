#include "base/memory.h"
#include "player/src/frames.h"
#include "player/src/player-device.h"

NS_FW_BASE_USE

using namespace PLAYER;

Frames::Frames() {
	device_ = NULL;
	channels_.initialize(sizeof(int), MAP_USE_REF, Map::hashingInt, Map::compareInt);
	channels_.hasRefKey(false);
}

Frames::~Frames() {
	for (var vi = 0; vi < channels_.values()->length(); vi++) {
		var channel = (Array*)channels_.values()->get(vi);
		DEL_(channel);
	}	
}

byte* Frames::insertCommand(Key time, Key channelId, byte* command) {
	var channel = (Array*)channels_.get(channelId);
	if (channel == NULL) {
		channel = NEW_(Array, sizeof(Command));
		channels_.add(channelId, channel);
	}

	Key pos = -1;
	byte* res = NULL;
	var cmd = (Command*)channel->binSearch(time, pos, Frames::compareCommand);
	if (cmd == NULL) {
		// insert
		Command cmd_(time.i, command);
		cmd = (Command*)channel->insert(pos, &cmd_);
	}
	else {
		// set
		res = cmd->command;
		cmd->command = command;
	}

	return res;
}

byte* Frames::removeCommand(Key time, Key channelId) {
	var channel = (Array*)channels_.get(channelId);
	byte* res = NULL;
	if (channel != NULL) {
		Key pos = -1;
		var cmd = (Command*)channel->binSearch(time, pos, Frames::compareCommand);
		if (cmd != NULL) {
			res = cmd->command;
			channel->remove(pos);
		}
	}
	return res;
}


Frames* Frames::fromSequence(Sequence* seq, FRAMES_GET_CHANNEL_ID getChannelId) {
	var frames = NEW_(Frames);
	int time = 0;
	frames->device_ = seq->device();
	seq->reset();
	seq->readByte();	// skip device id
	bool isEOS = false;
	while (!isEOS) {
		time += seq->readWord();
		while (true) {
			var cmd = *seq->cursor();
			var channelId = getChannelId(seq->cursor());
			isEOS = channelId == PlayerCommands::CmdEOS;
			if (isEOS || channelId == PlayerCommands::CmdEOF) {
				seq->readByte();
				break;
			}
			frames->insertCommand(time, channelId, seq->cursor());
			var cmdSize = seq->device()->getCommandSize(cmd, seq->cursor() + 1);
			seq->cursor() += cmdSize;
		}
	}

	return frames;
}

Sequence* Frames::toSequence() {
	var seq = NEW_(Sequence, device_);
	seq->writeHeader();
	var time = 0;
	var lastTime = 0;
	Key pos = -1;
	PArray channels;
	for (var chi = 0; chi < channels_.values()->length(); chi++) {
		var chn = (Array*)channels_.values()->get(chi);
		channels.push(chn);
	}
	while (true) {
		var isFirstCommand = true;
		var hasContent = false;
		for (var chi = 0; chi < channels.length(); chi++) {
			var chn = (Array*)channels.get(chi);
			var cmd = (Command*)chn->binSearch(time, pos, Frames::compareCommand);
			if (cmd != NULL) {
				if (isFirstCommand) {
					seq->writeDelta(time - lastTime);
					lastTime = time;
					isFirstCommand = false;
				}
				var size = device_->getCommandSize(cmd->command[0], cmd->command + 1);
				seq->writeBytes(cmd->command, size);
				hasContent = true;

				if (pos.i == chn->length() - 1) {
					channels.remove(chi);
					chi--;
				}
			}
		}
		if (channels.length() == 0) {
			seq->writeEOS();
			break;
		}

		if (hasContent) {
			seq->writeEOF();
			hasContent = false;
		}
		time++;
	}

	return seq;
}

int Frames::compareCommand(void* value, Key key, Collection* collection, va_list args) {
	var cmd = (Command*)value;
	return cmd->time - key.i;
}
