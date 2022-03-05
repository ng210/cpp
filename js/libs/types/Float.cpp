#include "Float.h"
#include <stdio.h>
#include "base/str.h"
#include "base/MemoryMgr.h"

NS_FW_BASE_BEGIN

const Type* Float::classType_;

void Float::initialize() {
	Float::classType_ = ADD_TYPE(Float);	// Type::add(STRINGIFY(NS_FW_BASE)"::float");
}
void Float::shutDown() {
}

Float::Float(float f) : Number() {
	value_.f = f;
}

Float::~Float() {

}

char* Float::toString() {
	char str[256];
	snprintf(str, 256, "%f", value_.f);
	return strdup(str);
}

int Float::compareTo(Object* obj) {
	char* buf = obj->toString();
	int res = strcmp(buf);
	FREE(buf);
	return res;
}

void* Float::valueOf() {
	return (void*)(long long)value_.f;
}

NS_FW_BASE_END