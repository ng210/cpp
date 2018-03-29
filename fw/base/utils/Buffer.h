#ifndef __BUFFER_H
#define __BUFFER_H

#include "basedef.h"
#include "collection/array.h"

NS_FW_BASE_BEGIN

#define BUFFER_CHUNK_SIZE 256*1024		// 256kB

class BufferChunk {
protected: PROP_R(size_t, byteCount);
protected: PROP_R(BYTE*, buffer);
public:
	BufferChunk(size_t byteCount);
	~BufferChunk();
	BYTE* flush();
	BYTE& get(size_t offset);
};

class Buffer {
protected: PROP_R(size_t, length);
protected: PROP_R(Array*, chunks);
//protected: size_t writeTo_;
protected: void init();
public:
	Buffer();
	virtual ~Buffer();

	size_t append(void* data, size_t byteCount, size_t to = 0);
	size_t read(void* targetBuffer, size_t byteCount = -1, size_t from = 0, size_t to = 0);
	size_t write(void* sourceBuffer, size_t byteCount, size_t from = 0, size_t to = 0);
	template<typename T> T* readAsType(void* targetBuffer, size_t count, size_t from = 0, size_t to = 0) {
		return (T*)read(targetBuffer, sizeof(T)*count, sizeof(T)*from, sizeof(T)*to);
	};
	BYTE* getByteBuffer();
	template<typename T> T* getBufferAsType() {
		return (T*)getByteBuffer();
	};
};

NS_FW_BASE_END

#endif