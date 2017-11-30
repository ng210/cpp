#include "base/str.h"
#include "base/Object.h"
#include "base/Boolean.h"
#include "base/String.h"
#include "base/Number.h"
#include "base/Integer.h"
#include "base/MemoryMgr.h"

#ifdef _DEBUG
#include <stdio.h>
#endif

NS_FW_BASE_BEGIN

Object* Object::nullInstance_ = NULL;
const Type* Object::classType_;

void Object::initialize() {
	Object::nullInstance_ = NEW_(Object);
	Object::classType_ = ADD_TYPE(Object);
}
void Object::shutDown() {
	DEL_(Object::nullInstance_);
}

//const Type* Object::classType() {
//	return Object::classType_;
//}

Object* Object::null() {
	return Object::nullInstance_;
}

/*****************************************************************************
* Object
*****************************************************************************/
Object::Object(void) {
	hash_ = (long long)this;
}
Object::~Object(void) {
}
char* Object::toString() {
	return (this != Null) ? strdup("[Object]") : strdup("Null");
}
bool Object::toBool() {
	return this != Object::nullInstance_;
}
Number* Object::toNumber() {
	return NEW_(Number);
}
void* Object::valueOf() {
	return this;
}
int Object::strcmp(const char* str) {
	char* buf = toString();
	int res = strncmp(buf, str, -1);
	FREE(buf);
	return res;
}
int Object::compareTo(Object* obj) {
	return (this->hash_ - obj->hash_) & 0xFFFFFFFF;
}

Object::operator String*() {
	return NEW_(String, this->toString());
}

#ifdef _DEBUG
bool Object::isDebugOn = false;
void Object::dump(const char* label) {
	if (Object::isDebugOn) {
		if (label == NULL) {
			printf(" *** (%s)%llX %llX\n", ((Type*)getType())->name(), (long long)this, hash_);
		} else {
			printf(" *** %s: (%s)%llX %llX\n", label, ((Type*)getType())->name(), (long long)this, hash_);
		}
	}
}
#endif

NS_FW_BASE_END
