#include <stdarg.h>
#include "Buffer.h"
#include "base/memory.h"

NS_FW_BASE_BEGIN


BufferChunk::BufferChunk(UINT32 byteCount) {
	byteCount_ = byteCount;
	buffer_ = MALLOC(UINT8, byteCount);
}
BufferChunk::~BufferChunk() {
	FREE(buffer_);
}
UINT8& BufferChunk::get(UINT32 offset) {
	return buffer_[offset];
}
UINT8* BufferChunk::flush() {
	UINT8* buffer = buffer_;
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
	offset_ = 0;
	//writeTo_ = 0;
	//readFrom_ = 0;
	chunks_ = NEW_(Array, sizeof(BufferChunk), 16);
}
UINT32 Buffer::seek(UINT32 to, UINT32& offset) {
	UINT32 chunkIndex = 0;
	offset = to;
	if (to <= length_) {
		// seek the first chunk
		for (; chunkIndex < chunks_->length(); chunkIndex++) {
			BufferChunk* chunk = (BufferChunk*)chunks_->getAt(chunkIndex);
			if (offset < chunk->byteCount()) {
				break;
			}
			offset -= chunk->byteCount();
		}
	} else {
		chunkIndex = chunks_->length() - 1;
		offset = 0;
	}
	return chunkIndex;
}
UINT32 Buffer::append(void* data, UINT32 byteCount, UINT32 from) {
	write(data, byteCount, from, length_);
	return length_;
}
UINT32 Buffer::append(Buffer* buffer, UINT32 byteCount, UINT32 from) {
	write(buffer, byteCount, from, length_);
	return length_;
}
UINT32 Buffer::write(void* data, UINT32 byteCount, UINT32 from, UINT32 to) {
	UINT32 remainingBytes = byteCount;
	UINT32 readFrom = from;
	UINT32 writeTo = 0;
	UINT32 offset = 0;
	UINT32 length = to;
	UINT32 chunkIndex = seek(to, writeTo);
	// write part of data that fits into the last buffer chunk
	for (UINT32 i=chunkIndex; i<chunks_->length(); i++) {
		BufferChunk* chunk = (BufferChunk*)chunks_->getAt(chunkIndex);
		UINT32 freeBytesInChunk = chunk->byteCount() - writeTo;
		UINT32 writeLength = remainingBytes < freeBytesInChunk ? remainingBytes : freeBytesInChunk;
		for (UINT32 i = 0; i < writeLength; i++) {
			chunk->get(writeTo++) = ((UINT8*)data)[readFrom++];
		}
		remainingBytes -= writeLength;
		length += writeLength;
		//writeTo = 0;
	}
	// write the rest of the data into a new chunk
	if (remainingBytes > 0) {
		// allocate at least BUFFER_CHUNK_SIZE bytes
		UINT32 chunkSize = remainingBytes < BUFFER_CHUNK_SIZE ? BUFFER_CHUNK_SIZE : remainingBytes;
		BufferChunk chunk(chunkSize);
		chunks_->add(&chunk);
		for (UINT32 i = 0; i < remainingBytes; i++) {
			chunk.get(i) = ((UINT8*)data)[readFrom++];
		}
		chunk.flush();
		length += remainingBytes;
	}
	if (length > length_) {
		length_ = length;
	}
	return length_;
}
UINT32 Buffer::write(Buffer* buffer, UINT32 byteCount, UINT32 from, UINT32 to) {
	UINT32 remainingBytes = byteCount != 0 ? byteCount : buffer->length();
	UINT32 readFrom = from;
	UINT32 writeTo = 0;
	UINT32 offset = 0;
	UINT32 chunkIndex = seek(to, writeTo);
	// write part of data that fits into the last buffer chunk
	if (chunkIndex < chunks_->length()) {
		BufferChunk* chunk = (BufferChunk*)chunks_->getAt(chunkIndex);
		UINT32 freeBytesInChunk = chunk->byteCount() - writeTo;
		UINT32 writeLength = remainingBytes < freeBytesInChunk ? remainingBytes : freeBytesInChunk;
		buffer->read(chunk->buffer(), writeLength, readFrom, writeTo);
		readFrom += writeLength;
		remainingBytes -= writeLength;
		length_ += writeLength;
	}
	// write the rest of the data into a new chunk
	if (remainingBytes > 0) {
		// allocate at least BUFFER_CHUNK_SIZE bytes
		UINT32 chunkSize = remainingBytes < BUFFER_CHUNK_SIZE ? BUFFER_CHUNK_SIZE : remainingBytes;
		BufferChunk chunk(chunkSize);
		chunks_->add(&chunk);
		buffer->read(chunk.buffer(), remainingBytes, readFrom, 0);
		chunk.flush();
		length_ += remainingBytes;
	}
	return length_;
}

UINT32 Buffer::read(void* targetBuffer, UINT32 byteCount, UINT32 from, UINT32 to) {
	UINT32 length = byteCount < length_ ? byteCount : length_;
	UINT32 writeTo = to;
	UINT32 readFrom = 0;
	UINT32 chunkIndex = seek(from, readFrom);
	//// seek the first chunk
	//for (; chunkIndex < chunks_->length(); chunkIndex++) {
	//	BufferChunk* chunk = (BufferChunk*)chunks_->getAt(chunkIndex);
	//	if (readFrom < chunk->byteCount()) {
	//		break;
	//	}
	//	readFrom -= chunk->byteCount();
	//}
	// copy data
	for (; chunkIndex < chunks_->length(); chunkIndex++) {
		BufferChunk* chunk = (BufferChunk*)chunks_->getAt(chunkIndex);
		UINT32 remainingBytes = chunk->byteCount() - readFrom;
		UINT32 readLength = length < remainingBytes ? length : remainingBytes;
		for (UINT32 j = 0; j < readLength; j++) {
			((UINT8*)targetBuffer)[writeTo++] = chunk->get(j + readFrom);
		}
		length -= readLength;
		if (length == 0) {
			break;
		}
		readFrom = 0;
	}
	//readFrom_ += byteCount;
	return length_;
}
UINT8* Buffer::getByteBuffer() {
	UINT8* buffer = MALLOC(UINT8, length_);
	UINT32 length = length_;
	UINT32 writeTo = 0;
	for (UINT32 i = 0; i < chunks_->length(); i++) {
		BufferChunk* chunk = (BufferChunk*)chunks_->getAt(i);
		UINT32 writeLength = length < chunk->byteCount() ? length : chunk->byteCount();
		for (UINT32 j = 0; j < writeLength; j++) {
			buffer[writeTo++] = chunk->get(j);
		}
		length -= writeLength;
	}
	return buffer;
}

//#define BUFFER_SET(t) void Buffer::set(t v, UINT32 ix) { ((t*)buffer_)[ix] = v; }
//#define BUFFER_PUSH(t) void Buffer::push(t v) { ((t*)buffer_)[position_++] = v; }
//
//BUFFER_SET(char)
//BUFFER_SET(int)
//BUFFER_SET(UINT32)
//BUFFER_SET(float)
//BUFFER_SET(double)
//
//BUFFER_PUSH(char)
//BUFFER_PUSH(int)
//BUFFER_PUSH(UINT32)
//BUFFER_PUSH(float)
//BUFFER_PUSH(double)

NS_FW_BASE_END