#include "stream.h"
#include "base/memory.h"

NS_FW_BASE_USE

Stream::Stream() {
	init(1024);
}

Stream::Stream(long size) {
	init(size);
}

Stream::Stream(byte* source, long length) {
	init(length);
	writeBytes(source, length);
}

void Stream::init(long size) {
	data_ = NULL;
	cursor_ = NULL;
	size_ = 0;
	cursor_ = ensureSize(size);
}

Stream::~Stream() {
	FREE(data_);
}

void Stream::cursor(byte* p) {
	if (p < data_) cursor_ = data_;
	else if (p < cursor_) cursor_ = p;
}

byte* Stream::ensureSize(long delta) {
	var l = length();
	if (size_ < l + delta) {
		size_ = l + delta;
		data_ = REALLOC(data_, byte, size_);
		cursor_ = data_ + l;
	}
	return data_;
}

byte Stream::readByte() {
	var b = *cursor_;
	cursor_++;
	return b;
}
word Stream::readWord() {
	var w = *(word*)cursor_;
	cursor_ += sizeof(word);
	return w;
}
dword Stream::readDword() {
	var dw = *(dword*)cursor_;
	cursor_ += sizeof(dword);
	return dw;
}
qword Stream::readQWord() {
	var qw = *(qword*)cursor_;
	cursor_ += sizeof(qword);
	return qw;
}
float Stream::readFloat() {
	var f = *(float*)cursor_;
	cursor_ += sizeof(float);
	return f;
}
double Stream::readDouble() {
	var d = *(double*)cursor_;
	cursor_ += sizeof(double);
	return d;
}
char* Stream::readString() {
	var s = (char*)cursor_;
	cursor_ += strlen(s);
	return s;
}

Stream* Stream::writeByte(byte v) {
	ensureSize(sizeof(v));
	*cursor_ = v;
	cursor_++;
	return this;
}
Stream* Stream::writeWord(word v) {
	ensureSize(sizeof(v));
	*(word*)cursor_ = v;
	cursor_ += sizeof(v);
	return this;
}
Stream* Stream::writeDword(dword v) {
	ensureSize(sizeof(v));
	*(dword*)cursor_ = v;
	cursor_ += sizeof(v);
	return this;
}
Stream* Stream::writeQWord(qword v) {
	ensureSize(sizeof(v));
	*(qword*)cursor_ = v;
	cursor_ += sizeof(v);
	return this;
}
Stream* Stream::writeFloat(float v) {
	ensureSize(sizeof(v));
	*(float*)cursor_ = v;
	cursor_ += sizeof(v);
	return this;
}
Stream* Stream::writeDouble(double v) {
	ensureSize(sizeof(v));
	*(double*)cursor_ = v;
	cursor_ += sizeof(v);
	return this;
}
Stream* Stream::writeString(char* s, bool addEnding) {
	var len = strlen(s);
	if (addEnding) len++;
	return writeBytes((byte*)s, len);
}
Stream* Stream::writeBytes(byte* data, long length, long offset) {
	var len = length - offset;
	ensureSize(len);
	memcpy(cursor_, data, len);
	cursor_ += len;
	return this;
}
Stream* Stream::writeStream(Stream* s, long length, long offset) {
	if (length == 0) length = s->length();
	length -= offset;
	return writeBytes(s->data_, length, offset);
}

byte* Stream::extract(long offset, long length) {
	var data = data_;
	size_ = 0;
	cursor_ = NULL;
	data_ = NULL;
	return data;
}