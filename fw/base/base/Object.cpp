#include "base/Object.h"
#include "base/String.h"
#include "base/MemoryMgr.h"

#ifdef _DEBUG
#include <stdio.h>
#endif

NS_FW_BASE_BEGIN

/*****************************************************************************
* Object
*****************************************************************************/
Object::Object(void) {
	iHash_ = (long long)this;
}
Object::~Object(void) {
}
const char* Object::getType() {
	return "object";
}
const char* Object::toString() {
	return "[Object]";
}
void* Object::valueOf() {
	return this;
}

int Object::compareTo(Object* obj) {
	return (this->iHash_ - obj->iHash_) & 0xFFFFFFFF;
}

#ifdef _DEBUG
bool Object::isDebugOn = false;
void Object::dump(const char* label) {
	if (Object::isDebugOn) {
		if (label == NULL) {
			printf(" *** (%s)%llX %llX\n", getType(), (long long)this, iHash_);
		}
		else {
			printf(" *** %s: (%s)%llX %llX\n", label, getType(), (long long)this, iHash_);

		}
	}
}
#endif

NS_FW_BASE_END