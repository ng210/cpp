#include "base/memory.h"
#include "base//string.h"
#include "player/src/frame-list.h"
#include "player/src/player-device.h"

NS_FW_BASE_USE

using namespace PLAYER;

FrameList::FrameList(DeviceExt* deviceExt) {
	deviceExt_ = deviceExt;
}

FrameList::~FrameList() {
	for (var fi = 0; fi < length_; fi++) {
		var frame = (Frame*)get(fi);
		for (var ci = 0; ci < frame->commands.length(); ci++) {
			FREE(frame->commands.get(ci));
		}
		DEL_(frame);
	}
}

int FrameList::insertCommand(int time, byte* command) {
	Key pos = -1;
	var frame = (Frame*)binSearch(time, pos, FrameList::compareFrames);
	// list contains frame at time?
	if (frame == NULL) {
		// - no: create frame
		frame = NEW_(Frame, time);
		add(pos, frame);
	}
	var cmd = frame->commands.search(*command, pos, FrameList::compareCommands);
	var length = deviceExt_->getCommandSize(command);
	var newCmd = MALLOC(byte, length);
	fmw::memcpy(newCmd, command, length);
	// frame contains command?
	if (cmd != NULL) {
		// - yes: replace command
		FREE(cmd);
		frame->commands.set(pos, newCmd);
	}
	else {
		frame->commands.push(newCmd);
	}

	return length;
}

void FrameList::removeCommand(int time, byte* command) {
	Key pos = -1;
	// get frame
	var frame = (Frame*)binSearch(time, pos, FrameList::compareFrames);
	if (frame) {
		// get command
		var cmd = frame->commands.search(*command, pos, FrameList::compareCommands);
		if (cmd) {
			// remove command
			FREE(cmd);
			frame->commands.remove(pos);
		}
	}
}

Sequence* FrameList::toSequence(DeviceExt* deviceExt) {
	var seq = NEW_(Sequence);
	seq->writeHeader();
	var time = 0;
	for (var fi = 0; fi < length_; fi++) {
		var frame = (Frame*)get(fi);
		seq->writeDelta(frame->time - time);
		time = frame->time;
		for (var ci = 0; ci < frame->commands.length(); ci++) {
			var cmd = (byte*)frame->commands.get(ci);
			var length = deviceExt->getCommandSize(cmd);
			seq->writeBytes(cmd, length);
		}
	}
	return seq;
}

char* FrameList::toJSON(DeviceExt* deviceExt) {
	Stream str;
	str.writeString("[\n", false);
	for (var fi = 0; fi < length_; fi++) {
		var frame = (Frame*)get(fi);
		var token = str_format("  {\n    \"time\":%d,\n    \"commands\": [\n");
		str.writeString(token, false);
		FREE(token);
		var cmdLength = frame->commands.length();
		for (var ci = 0; ci < cmdLength; ci++) {
			var cmd = (byte*)frame->commands.get(ci);
			var length = deviceExt->getCommandSize(cmd);
			str.writeString("      [ ", false);
			for (var bi = 0; bi < length; bi++) {
				token = str_format("\"%02X\"", cmd[bi]);
				str.writeString(token, false);
				FREE(token);
				if (bi < length - 1) str.writeString(", ", false);
			}
			ci < cmdLength - 1 ? str.writeString("],\n", false) : str.writeString(" ]\n", false);
		}
		str.writeString("  }\n", false);
	}
	str.writeString("]");
	return (char*)str.extract();
}

FrameList* FrameList::fromSequence(Sequence* seq, DeviceExt* deviceExt) {
	var frames = NEW_(FrameList, deviceExt);
	seq->rewind();
	seq->readByte();	// skip device id
	bool isEOS = false;
	int time = 0;
	var cmdId = 0;
	do {
		time += seq->readWord();
		var frame = NEW_(Frame, time);
		frames->push(frame);
		var isEOF = false;
		do {
			cmdId = *seq->cursor();
			var length = deviceExt->getCommandSize(seq->cursor());
			var cmd = MALLOC(byte, length);
			fmw::memcpy(cmd, seq->cursor(), length);
			frame->commands.push(cmd);
			seq->cursor() += length;
		} while (cmdId != PlayerCommands::CmdEOF && cmdId != PlayerCommands::CmdEOS);
	} while (cmdId != PlayerCommands::CmdEOS);

	return frames;
}

// Map<key, FrameList*>
Map* FrameList::splitSequence(Sequence* seq, DeviceExt* deviceExt, FRAME_FILTER* filter, ...) {
	// split a sequence into multiple list of frames using a filter function
	va_list args;
	va_start(args, filter);
	var map = NEW_(Map, sizeof(int), MAP_USE_REF, Map::hashingInt, Map::compareInt);
	seq->reset();
	seq->readByte();	// skip device id
	var time = 0;
	var cmdId = 0;
	do {
		// add delta
		time += seq->readWord();
		do {
			cmdId = *seq->cursor();
			var keyValue = filter(time, seq->cursor(), args);
			if (keyValue) {
				var frameList = (FrameList*)map->get(keyValue->key());
				if (!frameList) {
					frameList = NEW_(FrameList);
					frameList->deviceExt_ = deviceExt;
					map->put(keyValue->key(), frameList);
				}
				frameList->insertCommand(time, seq->cursor());
			}
		} while (cmdId != PlayerCommands::CmdEOF && cmdId != PlayerCommands::CmdEOS);
	} while (cmdId != PlayerCommands::CmdEOS);
	va_end(args);

	return map;
}

Sequence* FrameList::merge(Map* frameListMap, DeviceExt* deviceExt) {
	// merge multiple lists of frames into one sequence
	var seq = NEW_(Sequence);
	var count = frameListMap->size();
	// array of indices to current frame
	var cursors = MALLOC(int, count);
	fmw::memset(cursors, 0, sizeof(int) * count);
	// list of FrameList 
	var values = frameListMap->values();

	var time = 0, lastTime = 0;
	var hasEOS = false;
	while (true) {
		var hasContent = false;
		var hasNextContent = false;
		var hasEOF = false;
		var isFirstCommand = false;
		// search for frames in every list with current time
		for (var vi = 0; vi < count; vi++) {
			var frameList = (FrameList*)values->get(vi);
			// list still has frames?
			if (cursors[vi] < frameList->length()) {
				var frame = (Frame*)frameList->get(cursors[vi]);
				// frame is for current time?
				if (frame->time == time) {
					if (isFirstCommand) {
						// write delta only once
						seq->writeDelta(time - lastTime);
						lastTime = time;
						isFirstCommand = true;
					}
					// write every command at frame into the sequence
					for (var ci = 0; ci < frame->commands.length(); ci++) {
						var cmd = (byte*)frame->commands.get(ci);
						var length = deviceExt->getCommandSize(cmd);
						seq->writeBytes(cmd, length);
						hasEOF = *cmd == PlayerCommands::CmdEOF;
						hasEOS = *cmd == PlayerCommands::CmdEOS;
					}
					hasContent = true;
				}
				// next frame
				cursors[vi]++;
				// are there still frames to write?
				if (cursors[vi] < frameList->length()) {
					hasNextContent = true;
				}
			}
		}

		if (hasContent) {
			if (!hasEOF) {
				seq->writeEOF();
			}
		}

		if (!hasNextContent) break;

		time++;
	}
	if (!hasEOS) {
		seq->writeEOS();
	}

	return seq;
}

int FrameList::compareFrames(void* value, Key key, Collection* collection, va_list args) {
	return ((Frame*)value)->time - key.i;
}

int FrameList::compareCommands(void* value, Key key, Collection* collection, va_list args) {
	return *(byte*)value - key.i;
}
