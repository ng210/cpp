#include "Buffer.h"
#include "base/MemoryMgr.h"

NS_FW_BASE_BEGIN

const Type* BufferChunk::classType_;

void BufferChunk::initialize() {
	// add type entry
	classType_ = ADD_TYPE(BufferChunk);					// Type::add(STRINGIFY(NS_FW_BASE)"::bufferchunk");
}
BufferChunk::BufferChunk(size_t byteCount) {
	byteCount_ = byteCount;
	buffer_ = MALLOC(BYTE, byteCount);
}
BufferChunk::~BufferChunk() {
	FREE(buffer_);
}
BYTE& BufferChunk::get(size_t ix) {
	return buffer_[ix];
}
BYTE& BufferChunk::operator[](size_t ix) {
	return get(ix);
}

const Type* Buffer::classType_;

void Buffer::initialize() {
	// add type entry
	Buffer::classType_ = ADD_TYPE(Buffer);				// Type::add(STRINGIFY(NS_FW_BASE)"::buffer");
	BufferChunk::initialize();
}
//Buffer::Buffer() {
//	init(TYPE_CHAR, BUFFER_CHUNK_SIZE);
//}
//Buffer::Buffer(const Type* type) {
//	init(type, BUFFER_CHUNK_SIZE);
//}
Buffer::Buffer(const Type* type) {
	init(type);
}
Buffer::~Buffer() {
	chunks_.cleanUp();
}
void Buffer::init(const Type* type) {
	type_ = type;
	length_ = 0;
	writeTo_ = 0;
	chunk_ = NULL;
}

//size_t Buffer::getSize() {
//	return type_->size();
//}

size_t Buffer::append(void* data, size_t byteCount, size_t byteOffset) {
	//size_t freeBytesInChunk = chunk_ != NULL ? chunk_->byteCount_ - writeTo_ : 0;
	//size_t readFrom = byteOffset;
	//size_t remainingBytes = byteCount - byteOffset;

	//// copy part of data that fits into the current chunk
	//if (freeBytesInChunk > 0) {
	//	size_t writeLength = remainingBytes < freeBytesInChunk ? remainingBytes : freeBytesInChunk;
	//	for (size_t i = 0; i < writeLength; i++) {
	//		chunk_->get(writeTo_++) = ((BYTE*)data)[readFrom++];
	//	}
	//	freeBytesInChunk -= writeLength;
	//	remainingBytes -= writeLength;
	//}
	//// copy the remaining part of data as new chunk
	//if (remainingBytes > 0) {
	//	// allocate at least BUFFER_CHUNK_SIZE bytes
	//	size_t chunkSize = remainingBytes < BUFFER_CHUNK_SIZE ? BUFFER_CHUNK_SIZE : remainingBytes;
	//	chunk_ = NEW_(BufferChunk, chunkSize);
	//	chunks_.push(chunk_);
	//	size_t writeLength = remainingBytes;
	//	for (size_t i = 0; i < writeLength; i++) {
	//		chunk_->get(writeTo_++) = ((BYTE*)data)[readFrom++];
	//	}
	//}
	//length_ += byteCount - byteOffset;
	write(data, -1, byteCount, byteOffset);
	return length_;
}
void Buffer::write(void* data, size_t ix, size_t byteCount, size_t byteOffset) {
	size_t remainingBytes = byteCount - byteOffset;
	size_t readFrom = byteOffset;
	size_t writeTo = ix;
	size_t chunkIndex = 0;
	if (ix < length_) {
		// seek within buffer
		for (; chunkIndex < chunks_.length(); chunkIndex++) {
			BufferChunk* chunk = (BufferChunk*)chunks_[chunkIndex];
			if (writeTo > chunk->byteCount_) {
				writeTo_ = writeTo - chunk->byteCount_;
			}
			break;
		}
	} else {
		chunkIndex = chunks_.length() - 1;
		//writeTo = writeTo_;
	}
	// write part of data that fits into current buffer
	for (; chunkIndex < chunks_.length(); chunkIndex++) {
		BufferChunk* chunk = (BufferChunk*)chunks_[chunkIndex];
		size_t bytesInChunk = chunk->byteCount_ - writeTo_;
		size_t writeLength = remainingBytes < bytesInChunk ? remainingBytes : bytesInChunk;
		for (size_t i = 0; i < writeLength; i++) {
			chunk_->get(writeTo_++) = ((BYTE*)data)[readFrom++];
		}
		remainingBytes -= writeLength;
		length_ += writeLength;
		//writeTo = 0;
	}
	// write rest of the data into a new chunk
	if (remainingBytes > 0) {
		// allocate at least BUFFER_CHUNK_SIZE bytes
		size_t chunkSize = remainingBytes < BUFFER_CHUNK_SIZE ? BUFFER_CHUNK_SIZE : remainingBytes;
		chunk_ = NEW_(BufferChunk, chunkSize);
		chunks_.push(chunk_);
		for (size_t i = 0; i < remainingBytes; i++) {
			chunk_->get(writeTo_++) = ((BYTE*)data)[readFrom++];
		}
		length_ += remainingBytes;
	}
}

BYTE* Buffer::getByteBuffer() {
	BYTE* buffer = MALLOC(BYTE, length_);
	size_t length = length_;
	size_t writeTo = 0;
	for (size_t i = 0; i < chunks_.length(); i++) {
		BufferChunk* chunk = (BufferChunk*)chunks_[i];
		size_t writeLength = length < chunk->byteCount_ ? length_ : chunk->byteCount_;
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