#ifndef __BUFFER_H
#define __BUFFER_H

#include "base/base_defs.h"
#include "base/Object.h"
#include "collection/Array.h"

NS_FW_BASE_BEGIN

#define TYPE_BUFFER GET_TYPE(Buffer)
#define BUFFER_CHUNK_SIZE 256*1024		// 256kB

class Buffer;
class BufferChunk : public Object {
	friend class Buffer;
	DECL_TYPE(BufferChunk);
private: PROP_R(size_t, byteCount);
private: PROP_R(BYTE*, buffer);
private:
	BufferChunk(size_t byteCount);
	~BufferChunk();
	static void initialize();
public:
	BYTE& operator[](size_t ix);
	BYTE& get(size_t ix);
};

class Buffer : public Object {
	friend class RunTime;
	friend class File;
	DECL_TYPE(Buffer);
//private: PROP_R(char*, buffer);
private: PROP_R(size_t, length);
private: PROP_R(const Type*, type);
private:
	BufferChunk* chunk_;
	Array chunks_;
	size_t writeTo_;

	void init(const Type* type);
protected:
	void initialize();
public:
	//Buffer();
	Buffer(const Type* type = TYPE_CHAR);
	virtual ~Buffer();

	//size_t getSize();

	//size_t append(Buffer& data, size_t offset = 0, size_t length = 0);
	size_t append(void* data, size_t byteCount, size_t byteOffset = 0);
	void write(void* data, size_t ix, size_t byteCount, size_t byteOffset = 0);

	BYTE* getByteBuffer();
	template<typename T> T* getBuffer() {
		return (T*)getByteBuffer();
	};
};

NS_FW_BASE_END

#endif