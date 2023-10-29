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
}

void Sequence::init(int length) {
	size_ = 0;
	cursor_ = ensureSize(length);
	length_ = 0;
}

void Sequence::device(Device* dev) {
	if (device_ == NULL) {
		device_ = dev;
	}
}

Sequence* Sequence::writeString(char* string) { Stream::writeString(string); return this; }
Sequence* Sequence::writeByte(byte value) { Stream::writeByte(value); return this; }
Sequence* Sequence::writeWord(word value) { Stream::writeWord(value); return this; }
Sequence* Sequence::writeDword(dword value) { Stream::writeDword(value); return this; }
Sequence* Sequence::writeFloat(float value) { Stream::writeFloat(value); return this; }
Sequence* Sequence::writeHeader() { reset(); writeByte((byte)device_->type()); return this; }
Sequence* Sequence::writeDelta(word delta) { writeWord(delta); return this; }
Sequence* Sequence::writeCommand(byte cmd) { writeByte(cmd); return this; }
Sequence* Sequence::writeEOF() { writeByte(PlayerCommands::CmdEOF); return this; }
Sequence* Sequence::writeEOS() { writeByte(PlayerCommands::CmdEOS); return this; }
Sequence* Sequence::writeBytes(byte* stream, int length) { Stream::writeBytes(stream, length); return this; }
Sequence* Sequence::writeStream(Stream* stream, long length, long offset) { Stream::writeStream(stream, length, offset); return this; }
