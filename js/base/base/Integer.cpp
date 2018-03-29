#include "Integer.h"
#include <stdio.h>
#include "base/str.h"
#include "base/MemoryMgr.h"

NS_FW_BASE_BEGIN

const Type* Integer::classType_;

void Integer::initialize() {
	Integer::classType_ = ADD_TYPE(Integer);	// Type::add(STRINGIFY(NS_FW_BASE)"::integer");
}
void Integer::shutDown() {
}

Integer::Integer(int i) : Number() {
	value_.i = i;
}

Integer::~Integer() {

}

char* Integer::toString() {
	char str[256];
	snprintf(str, 256, "%d", value_.i);
	return strdup(str);
}

int Integer::compareTo(Object* obj) {
	char* buf = obj->toString();
	int res = strcmp(buf);
	FREE(buf);
	return res;
}

void* Integer::valueOf() {
	return (void*)(long long)value_.i;
}

NS_FW_BASE_END