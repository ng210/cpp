#include "base/memory.h"
#include "base/str.h"
#include "player/src/preset-bank.h"

using namespace PLAYER;

PresetBank::PresetBank() {
	initialize(0, NULL);
}
PresetBank::PresetBank(int bankSize, int count, byte* bytes) {
	Map::initialize(16 * sizeof(char), bankSize * sizeof(byte), Map::hashingStr, Collection::compareStr);
	initialize(bankSize, bytes);
}
PresetBank::~PresetBank() {
}

void PresetBank::initialize(int bankSize, byte* bytes) {
	bankSize_ = bankSize;
	data(bytes);
}

void PresetBank::data(byte* bytes) {
	if (bytes) {
		var count = READ(bytes, byte);
		var name = bytes;
		var program = &bytes[count * 16];
		for (var i = 0; i < count; i++) {
			put(name, program);
			name += PresetBankNameLength;
			program += bankSize_;
		}
	}
}

void PresetBank::copy(PresetBank* sb, int start, int end) {
	if (sb->size_ < end) end = (int)sb->size_;
	for (var i = start; i < end; i++) {
		put(sb->keys()->get(i), sb->values()->get(i));
	}
}

void PresetBank::add(char* name, byte* prg) {
	char str[16];
	fmw::strncpy(str, 15, name);
	Map::add(&str, prg);
}
byte* PresetBank::getPreset(int ix) {
	if (ix >= size_) ix = 0;
	return (byte*)values_->get(ix);
}
int PresetBank::indexOf(char* name) {
	return keys_->findIndex(name).i;
	//int res = -1;
	//var names = names_;
	//var data = data_;
	//for (var i = 0; i < count_; i++) {
	//	if (fmw::strncmp(name, names, PresetBankNameLength) == 0) {
	//		res = i;
	//		break;
	//	}
	//	names += PresetBankNameLength;
	//	data += size_;
	//}
	//return res;
}

void PresetBank::initializeFromStream(byte*& bytes) {
	data(bytes);
	bytes += 1 + size_ * (PresetBankNameLength + bankSize_);
}
void PresetBank::writeToStream(Stream* stream) {
	stream->writeByte((int)size_);
	//var offset = stream->cursor() - stream->data();
	char name[16];
	for (var i = 0; i < size_; i++) {
		memset(name, 0, 16);
		fmw::strncpy(name, 15, (char*)keys_->get(i));
		stream->writeBytes((byte*)name, 16);
		//offset += PresetBankNameLength;
		//stream->cursor(stream->data() + offset);
	}
	for (var i = 0; i < size_; i++) {
		stream->writeBytes((byte*)values_->get(i), (long)bankSize_);
		//offset += bankSize_;
		//stream->cursor(stream->data() + offset);
	}
}
