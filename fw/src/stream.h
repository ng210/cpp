#ifndef __STREAM_H
#define __STREAM_H

#include "basedef.h"
#include "types.h"
#include "base/str.h"

NS_FW_BASE_BEGIN

class Stream {
protected:PROP_R(long, size);
protected: byte* data_;
protected:PROP_R(byte*, cursor);
protected:byte* ensureSize(long delta);
public:
	Stream();
	Stream(long size);
	Stream(byte* source, long size);
	virtual ~Stream();
	void init(long size);

	void cursor(byte* p);
	inline byte* const data() { return data_; }

	byte readByte();
	word readWord();
	dword readDword();
	qword readQWord();
	float readFloat();
	double readDouble();
	char* readString();

	inline long length() { return (long)(cursor_ - data_); }
	inline void reset() { cursor_ = data_; }
	Stream* writeByte(byte b);
	Stream* writeWord(word w);
	Stream* writeDword(dword dw);
	Stream* writeQWord(qword qw);
	Stream* writeFloat(float f);
	Stream* writeDouble(double d);
	Stream* writeString(char* s, bool addEnding = true);
	Stream* writeBytes(byte* data, long length, long offset = 0);
	Stream* writeStream(Stream* s, long length = 0, long offset = 0);
	byte* extract(long offset = 0, long length = 0);
};


NS_FW_BASE_END

#endif