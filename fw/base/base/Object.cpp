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
	Object::classType_ = Type::add(STRINGIFY(NS_FW_BASE)"::object");
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
	return strdup("[Object]");
}
Number* Object::toNumber() {
	return NULL;
}
bool Object::toBool() {
	bool res = false;
	const Type* t = getType();
	while (true) {
		if (t->code() == TYPE_OBJECT) {
			res = this == Object::null();
			break;
		}
		if (t->code() == TYPE_STRING) {
			res = ((String*)this)->length() == 0;
			break;
		}
		if (t->code() == TYPE_BOOLEAN) {
			res = (bool)this->valueOf();
			break;
		}
		if (t->instanceOf(TYPE_NUMBER)) {
			Number* n = (Number*)this;
			res = !Number::isNaN(n) && n->toLong() != 0;
			break;
		}

		//if (code == TYPE_INTEGER) {
		//	res = Number::isNaN(obj) && (int)obj->valueof() != 0;
		//	break;
		//}
		//if (code == TYPE_LONG) {
		//	res = Number::isNaN(obj) && (long long)obj->valueof() != 0;
		//	break;
		//}
		//if (code == TYPE_FLOAT) {
		//	res = Number::isNaN(obj) && (float)obj->valueof() != 0.0f;
		//	break;
		//}
		//if (code == TYPE_DOUBLE) {
		//	res = Number::isNaN(obj) && (double)obj->valueof() != 0.0;
		//	break;
		//}
		break;
	}
	return res;
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
