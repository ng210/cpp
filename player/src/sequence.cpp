#include "base/memory.h"
#include "sequence.h"
#include "player.h"

NS_FW_BASE_USE
using namespace PLAYER;

Sequence::Sequence() {
	adapter_ = NULL;
	init(128);
}
Sequence::Sequence(Adapter* adapter) {
	adapter_ = adapter;
	init(128);
}
Sequence::Sequence(Adapter* adapter, byte* stream, int offset, int length) {
	adapter_ = adapter;
	init(length);
	memcpy(data_, stream, length);
}

Sequence::~Sequence() {
	;
}

void Sequence::init(int length) {
	size_ = 0;
	ensureSize(length);
	cursor_ = data_;
}

//byte Sequence::getByte(byte*& cursor) { var data = *(byte*)cursor; cursor += sizeof(byte); return data; }
//word Sequence::getWord(byte*& cursor) { var data = *(word*)cursor; cursor += sizeof(word); return data; }
//dword Sequence::getDword(byte*& cursor) { var data = *(dword*)cursor; cursor += sizeof(dword); return data; }
//float Sequence::getFloat(byte*& cursor) { var data = *(float*)cursor; cursor += sizeof(float); return data; }
//char* Sequence::getString(byte*& cursor) { var data = (char*)cursor; cursor += fmw::strlen(data) + 1; return data; }


//Sequence* Sequence::writeString(char* str) {
//	stream_->writeString(str);
//	return this;
//}
//Sequence* Sequence::writeStream(byte* stream, int length) {
//	stream_->writeBytes(stream, length);
//	return this;
//}
//Sequence* Sequence::writeByte(byte value) { stream_->writeByte(value); return this; }
//Sequence* Sequence::writeWord(word value) { stream_->writeByte(value); return this; }
//Sequence* Sequence::writeDword(dword value) { stream_->writeByte(value); return this; }
//Sequence* Sequence::writeFloat(float value) { stream_->writeByte(value); return this; }
Sequence* Sequence::writeHeader() { reset(); writeByte((byte)adapter_->info()->id); return this; };
Sequence* Sequence::writeDelta(word delta) { writeWord(delta); return this; };
Sequence* Sequence::writeCommand(byte cmd) { writeByte(cmd); return this; };
Sequence* Sequence::writeEOF() { writeByte(PlayerCommands::CmdEOF); return this; };
Sequence* Sequence::writeEOS() { writeByte(PlayerCommands::CmdEOS); return this; };

PArray* Sequence::toFrames() {
	var frames = NEW_(PArray);
	reset();
	cursor_++;
	//var cursor = data_ + 1;         // skip 1st byte adapter id
	while (true) {
		var frame = NEW_(Frame);
		frame->delta_ = readWord();
		byte cmd = 0;
		while (true) {
			// read command code byte
			cmd = readByte();
			if (cmd > PlayerCommands::CmdEOS) {
				var command = adapter_->makeCommand(cmd, this, cursor_);
				frame->addCommand(command);
			}
			else {
				if (cmd == PlayerCommands::CmdEOF && frame->commands_.length() == 0) {
					var command = MALLOC(byte, 1); command[0] = cmd;
					frame->addCommand(command);
				}
				break;
			}
		}
		frames->add(frame);
		if (cmd == PlayerCommands::CmdEOS) {
			break;
		}
	}
	return frames;
}

Sequence* Sequence::fromFrames(Collection* frames, Adapter* adapter) {
	var stream = MALLOC(byte, 65536);
	var cursor = stream;
	var sequence = NEW_(Sequence, adapter, stream);
	
	sequence->writeHeader();
	var hasEOS = false;
	for (var fi = 0; fi < frames->length(); fi++) {
		var frame = (Frame*)frames->getAt(fi);
		var hasEOF = false;
		sequence->writeDelta(frame->delta_);
		for (var ci = 0; ci < frame->commands_.length(); ci++) {
			var command = (byte*)frame->commands_.getAt(ci);
			// first byte is the length
			var len = *command++;
			sequence->writeBytes(command, len);
			var cmd = command[1];
			if (cmd == 0) {
				hasEOF = true;
				break;
			}
			if (cmd == 1) {
				hasEOS = true;
				hasEOF = true;
				break;
			}
		}
		//if (hasEOS) break;
		if (!hasEOF) {
			sequence->writeByte(PlayerCommands::CmdEOF);
		}
	}
	// todo!!!
	sequence->cursor_--;
	if (!hasEOS) {
		sequence->writeByte(PlayerCommands::CmdEOS);
	}
	// resize stream to match the exact length
	sequence->data_ = REALLOC(sequence->data_, byte, sequence->length());
	return sequence;
}
