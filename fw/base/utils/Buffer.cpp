#include "Buffer.h"
#include "base/MemoryMgr.h"

NS_FW_BASE_BEGIN

const Type* Buffer::classType_;

Buffer::Buffer() {
	init(TYPE_CHAR, 65536);
}

Buffer::Buffer(const Type* type, size_t length) {
	init(type, length);
	type_ = type;
	buffer_ = NEWARR(char, length * type->size());
}

Buffer::~Buffer() {
	DEL_(buffer_)
}

void Buffer::init(const Type* type, const size_t length) {
	buffer_ = NULL;
	length_ = length;
	type_ = type;
	position_ = 0;
}

size_t Buffer::getSize() {
	return type_->size();
}

#define BUFFER_SET(t) void Buffer::set(t v, size_t ix) { ((t*)buffer_)[ix] = v; }
#define BUFFER_PUSH(t) void Buffer::push(t v) { ((t*)buffer_)[position_++] = v; }

BUFFER_SET(char)
BUFFER_SET(int)
BUFFER_SET(size_t)
BUFFER_SET(float)
BUFFER_SET(double)

BUFFER_PUSH(char)
BUFFER_PUSH(int)
BUFFER_PUSH(size_t)
BUFFER_PUSH(float)
BUFFER_PUSH(double)

NS_FW_BASE_END