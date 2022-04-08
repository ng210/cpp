#include "base/memory.h"
#include "sequence.h"
#include "player.h"
#include "stdio.h"

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
	if (frames_) {
		var frame0 = (Frame*)frames_->getAt(0);
		var cmd0 = frame0->commands_.getAt(0);
		FREE(cmd0);
		ARRAY_FOREACH(frames_, DEL_((Frame*)value));
		DEL_(frames_);
	}
}

void Sequence::init(int length) {
	size_ = 0;
	ensureSize(length);
	cursor_ = data_;
	frames_ = NULL;
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
	frames_ = NEW_(PArray);
	reset();
	cursor_++;
	//var cursor = data_ + 1;         // skip 1st byte adapter id
	var stream = NEW_(Stream, 256);
	while (true) {
		var frame = NEW_(Frame);
		frame->delta_ = readWord();
		byte cmd = 0;
		while (true) {
			// read command code byte
			cmd = readByte();
			if (cmd > CmdEOS) {
				frame->addCommand(stream->cursor());
				stream->writeByte(cmd);
				var len = adapter_->getCommandArgsSize(cmd, cursor_);
				stream->writeBytes(cursor_, len);
				cursor_ += len;
			}
			else break;
		}
		frames_->add(frame);
		if (cmd == CmdEOS) {
			frame->addCommand(stream->cursor());
			stream->writeByte(CmdEOS);
			break;
		}
	}
	stream->extract();
	DEL_(stream);
	return frames_;
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

char* Sequence::print() {
	var stream = NEW_(Stream, 64);
	// write adapter
	stream->writeString("Adapter: ", false);
	stream->writeString((char*)adapter_->info()->name, false);
	stream->writeString("\n", false);
	// write frames
	stream->writeString("Frames\n", false);
	var hasFrames = frames_ != NULL;
	if (!hasFrames) {
		toFrames();
	}

	char str[128];
	const char* hexDigits = "0123456789ABCDEF";
	for (var i = 0; i < frames_->length(); i++) {
		var frame = (Frame*)frames_->getAt(i);
		sprintf_s(str, 64, " #%02d [%03d] ", i, frame->delta_);
		stream->writeString(str, false);
		for (var j = 0; j < frame->commands_.length(); j++) {
			var cmd = (byte*)frame->commands_.getAt(j);
			var code = *cmd++;
			var len = adapter_->getCommandArgsSize(code, cmd);
			// hexdump command
			var n = 0;
			str[n++] = hexDigits[code >> 4];
			str[n++] = hexDigits[code & 0x0f];
			str[n++] = '(';
			for (var k = 0; k < len; k++) {
				var b = *cmd++;
				str[n++] = hexDigits[b >> 4];
				str[n++] = hexDigits[b & 0x0f];
				if (k < len-1) str[n++] = ' ';
			}
			str[n++] = ')';
			if (j < frame->commands_.length() - 1) {
				str[n++] = ' ';
			}
			str[n] = '\0';
			stream->writeString(str, false);
		}
		stream->writeString("\n", false);
	}
	stream->writeByte(0);

	if (!hasFrames) {
		FREE(((Frame*)frames_->getAt(0))->commands_.getAt(0));
		ARRAY_FOREACH(frames_, DEL_((Frame*)value));
		DEL_(frames_);
		frames_ = NULL;
	}
	var data = (char*)stream->extract();
	DEL_(stream);
	return data;
}