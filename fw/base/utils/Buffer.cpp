#include "Buffer.h"
#include "base/memory.h"

NS_FW_BASE_BEGIN


BufferChunk::BufferChunk(size_t byteCount) {
	byteCount_ = byteCount;
	buffer_ = MALLOC(BYTE, byteCount);
}
BufferChunk::~BufferChunk() {
	FREE(buffer_);
}
BYTE& BufferChunk::get(size_t offset) {
	return buffer_[offset];
}
BYTE* BufferChunk::flush() {
	BYTE* buffer = buffer_;
	buffer_ = NULL;
	return buffer;
}

Buffer::Buffer() {
	init();
}
Buffer::~Buffer() {
	ARRAY_FOREACH(chunks_, ((BufferChunk*)value)->~BufferChunk());
	DEL_(chunks_);
}
void Buffer::init() {
	length_ = 0;
	//writeTo_ = 0;
	chunks_ = NEW_(Array, sizeof(BufferChunk), 16);
}
size_t Buffer::append(void* data, size_t byteCount, size_t from) {
	write(data, byteCount, from, length_);
	return length_;
}
size_t Buffer::write(void* data, size_t byteCount, size_t from, size_t to) {
	size_t remainingBytes = byteCount - from;
	size_t readFrom = from;
	size_t writeTo = to;
	UINT32 chunkIndex = 0;
	if (to <= length_) {
		// seek the first chunk
		for (; chunkIndex < chunks_->length(); chunkIndex++) {
			BufferChunk* chunk = (BufferChunk*)chunks_->getAt(chunkIndex);
			if (writeTo < chunk->byteCount()) {
				break;
			}
			writeTo -= chunk->byteCount();
		}
		//writeTo_ = writeTo;
	} else {
		chunkIndex = chunks_->length() - 1;
		//writeTo = writeTo_;
	}
	// write part of data that fits into current buffer
	for (; chunkIndex < chunks_->length(); chunkIndex++) {
		BufferChunk* chunk = (BufferChunk*)chunks_->getAt(chunkIndex);
		size_t bytesInChunk = chunk->byteCount() - writeTo;
		size_t writeLength = remainingBytes < bytesInChunk ? remainingBytes : bytesInChunk;
		for (size_t i = 0; i < writeLength; i++) {
			chunk->get(writeTo++) = ((BYTE*)data)[readFrom++];
		}
		remainingBytes -= writeLength;
		length_ += writeLength;
		//writeTo = 0;
	}
	// write rest of the data into a new chunk
	if (remainingBytes > 0) {
		// allocate at least BUFFER_CHUNK_SIZE bytes
		size_t chunkSize = remainingBytes < BUFFER_CHUNK_SIZE ? BUFFER_CHUNK_SIZE : remainingBytes;
		BufferChunk chunk(chunkSize);
		chunks_->add(&chunk);
		for (size_t i = 0; i < remainingBytes; i++) {
			chunk.get(i) = ((BYTE*)data)[readFrom++];
		}
		chunk.flush();
		length_ += remainingBytes;
	}
	return length_;
}
size_t Buffer::read(void* targetBuffer, size_t byteCount, size_t from, size_t to) {
	size_t length = byteCount < length_ ? byteCount : length_;
	size_t writeTo = to;
	size_t readFrom = from;
	UINT32 chunkIndex = 0;
	// seek the first chunk
	for (; chunkIndex < chunks_->length(); chunkIndex++) {
		BufferChunk* chunk = (BufferChunk*)chunks_->getAt(chunkIndex);
		if (readFrom < chunk->byteCount()) {
			break;
		}
		readFrom -= chunk->byteCount();
	}
	// copy data
	for (; chunkIndex < chunks_->length(); chunkIndex++) {
		BufferChunk* chunk = (BufferChunk*)chunks_->getAt(chunkIndex);
		size_t remainingBytes = chunk->byteCount() - readFrom;
		long long readLength = (long long)(length < remainingBytes ? length : remainingBytes);
		for (long long j = 0; j < readLength; j++) {
			((BYTE*)targetBuffer)[writeTo++] = chunk->get(j + readFrom);
		}
		length -= readLength;
		if (length == 0) {
			break;
		}
		readFrom = 0;
	}
	return length_;
}
BYTE* Buffer::getByteBuffer() {
	BYTE* buffer = MALLOC(BYTE, length_);
	size_t length = length_;
	size_t writeTo = 0;
	for (UINT32 i = 0; i < chunks_->length(); i++) {
		BufferChunk* chunk = (BufferChunk*)chunks_->getAt(i);
		size_t writeLength = length < chunk->byteCount() ? length : chunk->byteCount();
		for (size_t j = 0; j < writeLength; j++) {
			buffer[writeTo++] = chunk->get(j);
		}
		length -= writeLength;
	}
	return buffer;
}

//#define BUFFER_SET(t) void Buffer::set(t v, size_t ix) { ((t*)buffer_)[ix] = v; }
//#define BUFFER_PUSH(t) void Buffer::push(t v) { ((t*)buffer_)[position_++] = v; }
//
//BUFFER_SET(char)
//BUFFER_SET(int)
//BUFFER_SET(size_t)
//BUFFER_SET(float)
//BUFFER_SET(double)
//
//BUFFER_PUSH(char)
//BUFFER_PUSH(int)
//BUFFER_PUSH(size_t)
//BUFFER_PUSH(float)
//BUFFER_PUSH(double)

NS_FW_BASE_END