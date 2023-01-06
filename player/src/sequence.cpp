#include "stdio.h"
#include "base/memory.h"
#include "sequence.h"
#include "player-device.h"

NS_FW_BASE_USE
using namespace PLAYER;

Sequence::Sequence() {
	device_ = NULL;
	init(128);
}
Sequence::Sequence(Device* device) {
	device_ = device;
	init(128);
}
Sequence::Sequence(Device* device, byte* stream, int offset, int length) {
	device_ = device;
	init(length);
	memcpy(cursor_, stream, length);
	cursor_ += length;
	length_ += length;
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
	cursor_ = ensureSize(length);
	length_ = 0;
	frames_ = NULL;
}

//byte Sequence::getByte(byte*& cursor) { var data = *(byte*)cursor; cursor += sizeof(byte); return data; }
//word Sequence::getWord(byte*& cursor) { var data = *(word*)cursor; cursor += sizeof(word); return data; }
//dword Sequence::getDword(byte*& cursor) { var data = *(dword*)cursor; cursor += sizeof(dword); return data; }
//float Sequence::getFloat(byte*& cursor) { var data = *(float*)cursor; cursor += sizeof(float); return data; }
//char* Sequence::getString(byte*& cursor) { var data = (char*)cursor; cursor += fmw::strlen(data) + 1; return data; }
//
//Sequence* Sequence::writeString(char* str) {
//	stream_->writeString(str);
//	return this;
//}
//Sequence* Sequence::writeStream(byte* stream, int length) {
//	stream_->writeBytes(stream, length);
//	return this;
//}
Sequence* Sequence::writeString(char* string) { Stream::writeString(string); return this; }
Sequence* Sequence::writeByte(byte value) { Stream::writeByte(value); return this; }
Sequence* Sequence::writeWord(word value) { Stream::writeWord(value); return this; }
Sequence* Sequence::writeDword(dword value) { Stream::writeDword(value); return this; }
Sequence* Sequence::writeFloat(float value) { Stream::writeFloat(value); return this; }
Sequence* Sequence::writeHeader() { reset(); writeByte((byte)device_->type()); return this; };
Sequence* Sequence::writeDelta(word delta) { writeWord(delta); return this; };
Sequence* Sequence::writeCommand(byte cmd) { writeByte(cmd); return this; };
Sequence* Sequence::writeEOF() { writeByte(PlayerCommands::CmdEOF); return this; };
Sequence* Sequence::writeEOS() { writeByte(PlayerCommands::CmdEOS); return this; };

PArray* Sequence::toFrames(Device* device) {
	frames_ = NEW_(PArray);
	reset();
	cursor_++;
	if (device == NULL) device = device_;
	//var cursor = data_ + 1;         // skip 1st byte device id
	var stream = NEW_(Stream, 65536);
	while (true) {
		var frame = NEW_(Frame);
		frame->delta_ = readWord();
		byte cmd = 0;
		while (true) {
			// read command code byte
			cmd = *cursor_;
			if (cmd > PlayerCommands::CmdEOS) {
				frame->addCommand(stream->cursor());
				var len = device->getCommandSize(cmd, cursor_);
				stream->writeBytes(cursor_, len);
				cursor_ += len;
			} else {
				cursor_++;
				break;
			}
		}
		frames_->add(frame);
		if (cmd == PlayerCommands::CmdEOS) {
			frame->addCommand(stream->cursor());
			stream->writeByte(CmdEOS);
			break;
		}
	}
	stream->extract();
	DEL_(stream);
	return frames_;
}

Sequence* Sequence::fromFrames(Collection* frames, Device* device) {
	var sequence = NEW_(Sequence, device);
	sequence->init(65536);
	var cursor = sequence->data();
	
	sequence->writeHeader();
	var hasEOS = false;
	for (var fi = 0; fi < frames->length(); fi++) {
		var frame = (Frame*)frames->getAt(fi);
		var hasEOF = false;
		sequence->writeDelta(frame->delta_);
		for (var ci = 0; ci < frame->commands_.length(); ci++) {
			var command = (byte*)frame->commands_.getAt(ci);
			var cmd = *command;
			var len = device->getCommandSize(cmd, command + 1);
			sequence->writeBytes(command, len);
			if (cmd == PlayerCommands::CmdEOF) {
				hasEOF = true;
				break;
			}
			if (cmd == PlayerCommands::CmdEOS) {
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
	if (!hasEOS) {
		sequence->writeByte(PlayerCommands::CmdEOS);
	}
	// resize stream to match the exact length
	sequence->data_ = REALLOC(sequence->data_, byte, sequence->length());
	return sequence;
}

char* Sequence::print(Device* device) {
	var stream = NEW_(Stream, 64);
	if (device == NULL) device = device_;
	char str[128];
	// write device
	stream->writeString("Device: ", false);
	sprintf_s(str, 64, " #%02X\n", device->type());
	stream->writeString(str, false);
	// write frames
	stream->writeString("Frames\n", false);
	var hasFrames = frames_ != NULL;
	if (!hasFrames) {
		toFrames();
	}

	const char* hexDigits = "0123456789ABCDEF";
	for (var i = 0; i < frames_->length(); i++) {
		var frame = (Frame*)frames_->getAt(i);
		sprintf_s(str, 64, " #%02d [%03d] ", i, frame->delta_);
		stream->writeString(str, false);
		for (var j = 0; j < frame->commands_.length(); j++) {
			var cmd = (byte*)frame->commands_.getAt(j);
			var code = *cmd;
			var len = device->getCommandSize(code, cmd);
			// hexdump command
			var n = 0;
			str[n++] = hexDigits[code >> 4];
			str[n++] = hexDigits[code & 0x0f];
			str[n++] = '(';
			for (var k = 1; k < len; k++) {
				var b = cmd[k];
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