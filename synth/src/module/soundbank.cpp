#include "base/str.h"
#include "synth/src/module/soundbank.h"

using namespace SYNTH;

Soundbank::Soundbank() {
	initialize(0, NULL);
}
Soundbank::Soundbank(int bankSize, int count, byte* bytes) {
	Map::initialize(16 * sizeof(char), bankSize *sizeof(byte), Map::hashingStr, Collection::compareStr);
	initialize(bankSize, bytes);
}

Soundbank::~Soundbank() {
}

void Soundbank::initialize(int bankSize, byte* bytes) {
	bankSize_ = bankSize;
	data(bytes);
}

void Soundbank::data(byte* bytes) {
	if (bytes) {
		var count = READ(bytes, byte);
		var name = bytes;
		var program = &bytes[count * 16];
		for (var i = 0; i < count; i++) {
			put(name, program);
			name += SoundbankNameLength;
			program += bankSize_;
		}
	}
}

void Soundbank::copy(Soundbank* sb, int start, int end) {
	if (sb->size_ < end) end = (int)sb->size_;
	for (var i = start; i < end; i++) {
		put(sb->keys()->get(i), sb->values()->get(i));
	}
}

//void Soundbank::add(char* name, byte* program) {
//	var count = count_ + 1;
//	// allocate size + 1 times name and prg
//	var bytes = MALLOC(byte, 1 + count * (SoundbankNameLength + size_));
//	var names = (char*)&bytes[1];
//	var programs = &bytes[1 + count * SoundbankNameLength];
//	// copy old entries
//	if (count_ > 0) {
//		fmw::memcpy(names, names_, count_ * SoundbankNameLength);
//		fmw::memcpy(programs, programs_, count_ * size_);
//	}
//	fmw::strncpy(&names[count_ * SoundbankNameLength], SoundbankNameLength, name);
//	fmw::memcpy(&programs[count_ * size_], program, size_);
//	bytes[0] = count;
//	data(bytes);
//}

//void Soundbank::remove(int ix) {
//	if (ix > 0 && ix < count_) {
//		var name = &data_[1 + ix * SoundbankNameLength];
//		var length = SoundbankNameLength * (count_ - ix - 2);
//		fmw::memcpy(name, name + SoundbankNameLength, length);
//		// move 1st part
//		programs_ -= SoundbankNameLength;
//		fmw::memcpy(programs_, &programs_[SoundbankNameLength], size_ * ix);
//		// move 2nd part
//		fmw::memcpy(&programs_[size_ * ix], &programs_[size_ * ix + size_], size_ * (count_ - ix - 2));
//		count_--;
//	}
//}

byte* Soundbank::getProgram(int ix) {
	if (ix >= size_) ix = 0;
	return (byte*)values_->get(ix);
}

int Soundbank::indexOf(char* name) {
	return keys_->findIndex(name).i;
	//int res = -1;
	//var names = names_;
	//var data = data_;
	//for (var i = 0; i < count_; i++) {
	//	if (fmw::strncmp(name, names, SoundbankNameLength) == 0) {
	//		res = i;
	//		break;
	//	}
	//	names += SoundbankNameLength;
	//	data += size_;
	//}
	//return res;
}

//Stream* Soundbank::toStream() {
//	var length = 1 + count_ * (SoundbankNameLength + size_);
//	return NEW_(Stream, data_, length);
//}

void Soundbank::initializeFromStream(byte*& bytes) {
	data(bytes);
	bytes += 1 + size_ * (SoundbankNameLength + bankSize_);
}

void Soundbank::add(char* name, byte* prg) {
	char str[16];
	fmw::strncpy(str, 15, name);
	Map::add(&str, prg);
}

void Soundbank::writeToStream(Stream* stream) {
	stream->writeByte((int)size_);
	//var offset = stream->cursor() - stream->data();
	char name[16];
	for (var i = 0; i < size_; i++) {
		memset(name, 0, 16);
		fmw::strncpy(name, 15, (char*)keys_->get(i));
		stream->writeBytes((byte*)name, 16);
		//offset += SoundbankNameLength;
		//stream->cursor(stream->data() + offset);
	}
	for (var i = 0; i < size_; i++) {
		stream->writeBytes((byte*)values_->get(i), (long)bankSize_);
		//offset += bankSize_;
		//stream->cursor(stream->data() + offset);
	}
}
