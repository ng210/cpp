#ifndef __BUFFER_H
#define __BUFFER_H

#include "basedef.h"
#include "types.h"
#include "collection/array.h"

NS_FW_BASE_BEGIN

#define BUFFER_CHUNK_SIZE 256*1024		// 256kB

class BufferChunk {
protected: PROP_R(UINT32, byteCount);
protected: PROP_R(BYTE*, buffer);
public:
	BufferChunk(UINT32 byteCount);
	~BufferChunk();
	BYTE* flush();
	BYTE& get(UINT32 offset);
};

class Buffer {
protected: PROP_R(UINT32, length);
//protected: PROP_R(UINT32, writeTo);
//protected: PROP_R(UINT32, readFrom);
protected: PROP_R(UINT32, offset);
protected: PROP_R(Array*, chunks);

protected: void init();
public:
	Buffer();
	virtual ~Buffer();

	UINT32 seek(UINT32 writeTo, UINT32& offset);
	UINT32 append(void* data, UINT32 byteCount, UINT32 from = 0);
	UINT32 append(Buffer* buffer, UINT32 byteCount = 0, UINT32 from = 0);
	UINT32 read(void* targetBuffer, UINT32 byteCount = -1, UINT32 from = 0, UINT32 to = 0);
	UINT32 write(void* data, UINT32 byteCount, UINT32 from = 0, UINT32 to = 0);
	UINT32 write(Buffer* sourceBuffer, UINT32 byteCount = 0, UINT32 from = 0, UINT32 to = 0);
	template<typename T> UINT32 readAsType(void* targetBuffer, UINT32 count, UINT32 from = 0, UINT32 to = 0) {
		return read(targetBuffer, sizeof(T)*count, sizeof(T)*from, sizeof(T)*to);
	};
	BYTE* getByteBuffer();
	template<typename T> T* getBufferAsType() {
		return (T*)getByteBuffer();
	};
};

NS_FW_BASE_END

#endif