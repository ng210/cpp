#include "base/stream.h"
#include "base/memory.h"

NS_FW_BASE_USE

Stream::Stream() {
	init();
}

Stream::Stream(long size) {
	init(size);
}

Stream::Stream(byte* source) {
	init();
	FREE(data_);
	data_ = source;
	cursor_ = source;
}

Stream::Stream(byte* source, long length) {
	init(length);
	writeBytes(source, length);
	rewind();
}

void Stream::init(long size) {
	data_ = NULL;
	size_ = 0;
	length_ = 0;
	cursor_ = ensureSize(size);
	length_ = 0;
}

Stream::~Stream() {
	FREE(data_);
}

byte* Stream::seek(long long bytes, int origin) {
	var end = data_ + length_;
	switch (origin) {
		case SEEK_SET: cursor_ = data_;
		case SEEK_CUR:
			cursor_ += bytes;
			if (cursor_ > end) cursor_ = end;
			break;
		case SEEK_END: 
			cursor_ = data_ + length_ - bytes;
			break;
	}
	if (cursor_ < data_) cursor_ = data_;
	
	return cursor_;
}

void Stream::cursor(byte* p) {
	if (p < data_) cursor_ = data_;
	else if (p < data_ + length_) cursor_ = p;
}

byte* Stream::ensureSize(long delta) {
	var diff = delta - (size_ - length_);
	if (diff > 0) {
		size_ += delta < STREAM_DEFAULT_SIZE ? STREAM_DEFAULT_SIZE : delta;
		var curPos = cursor_ - data_;
		data_ = REALLOC(data_, byte, size_);
		cursor_ = data_ + curPos;
	}
	length_ += delta;
	return data_;
}

byte Stream::readByte() {
	var b = *cursor_;
	cursor_++;
	return b;
}
long Stream::readBytes(void* buffer, long length) {
	var rest = (long)(data_ + length_ - cursor_);
	if (rest < length) length = rest;
	memcpy(buffer, cursor_, length);
	cursor_ += length;
	return length;
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
	cursor_ += strlen(s) + 1;
	return s;
}
char* Stream::readRow() {
	char* row = NULL;
	var start = cursor_;
	var end = &data_[length_];
	var i = 0, j = 0;
	while (cursor_ < end) {
		var ch = *cursor_++;
		if (ch == '\0') break;
		if (ch == '\r') {
			j = 1;
			if (cursor_[j] == '\n') j++;
			break;
		}
		if (ch == '\n') {
			j = 1;
			break;
		}
		i++;
	}
	if (i > 0) {
		row = MALLOC(char, (size_t)(i + 1));
		memcpy(row, start, i);
		row[i] = '\0';
		cursor_ += j;
	}
	return row;
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
	var data = REALLOC(data_, byte, length_);
	size_ = 0;
	cursor_ = NULL;
	data_ = NULL;
	length_ = 0;
	return data;
}

byte* Stream::readBytesFromFile(const char* path, size_t byteCount, size_t offset) {
	var fp = File::open(path);
	var bytes = readBytesFromFile(fp, byteCount, offset);
	fclose(fp);
	return bytes;
}

byte* Stream::readBytesFromFile(FILE* fp, size_t byteCount, size_t offset) {
	var curr = fseek(fp, 0, SEEK_CUR);
	fseek(fp, 0, SEEK_END);
	size_t length = (size_t)ftell(fp);
	fseek(fp, curr + (long)offset, SEEK_SET);
	var bytesAhead = length - (curr + offset);
	if (byteCount > bytesAhead) byteCount = bytesAhead;
	ensureSize((long)byteCount);
	var cursor = cursor_;
	cursor_ += fread_s(cursor_, byteCount, sizeof(byte), byteCount, fp);
	return cursor;
}

size_t Stream::writeBytesToFile(const char* path, size_t byteCount, size_t offset) {
	FILE* fp = NULL;
	size_t bytesWritten = 0;
	if (fopen_s(&fp, path, "w+b") == 0 && fp != NULL) {
		bytesWritten = writeBytesToFile(fp, byteCount, offset);
		fclose(fp);
	}
	return bytesWritten;
}
size_t Stream::writeBytesToFile(FILE* fp, size_t byteCount, size_t offset) {
	var bytes = byteCount < length_ ? byteCount : length_;
	if (offset + bytes > length_) bytes = length_ - offset;
	return File::write(fp , &data_[offset], bytes);
}