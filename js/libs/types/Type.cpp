#include "base/Type.h"
#include "base/str.h"
#include "base/Object.h"

NS_FW_BASE_BEGIN

Type::Type() : code_(0) {
	name_ = NULL;
}

size_t Type::typeCount_ = 0;
Type Type::types_[1024];

const Type* Type::char_ = NULL;
const Type* Type::bool_ = NULL;
const Type* Type::short_ = NULL;
const Type* Type::int_ = NULL;
const Type* Type::size_t_ = NULL;
const Type* Type::float_ = NULL;
const Type* Type::double_ = NULL;

#define ADD_TYPE_(t) Type::add(STRINGIFY(t), sizeof(t));

void Type::initialize() {
	memset(types_, 0, sizeof(Type) * MAX_TYPE_COUNT);
	Type::char_ = ADD_TYPE_(char);
	Type::bool_ = ADD_TYPE_(bool);
	Type::short_ = ADD_TYPE_(short);
	Type::int_ = ADD_TYPE_(int);
	Type::size_t_ = ADD_TYPE_(size_t);
	Type::float_ = ADD_TYPE_(float);
	Type::double_ = ADD_TYPE_(double);
}
void Type::shutDown() {
}

Type* Type::getTypeByName(const char* name) {
	Type* type = NULL;
	size_t len = strlen(name);
	for (size_t i = 0; i < Type::typeCount_; i++) {
		type = &Type::types_[i];
		if (strncmp(type->name_, name, len) == 0) {
			break;
		}
	}
	return type;
}

Type* Type::add(const char* name, size_t size, const char* parent) {
	Type* type = &Type::types_[typeCount_];
	type->code_ = typeCount_++;
	type->size_ = size;
	type->name_ = name;
	type->parentType_ = parent != NULL ? Type::getTypeByName(parent) : NULL;
	return type;
}

const size_t Type::code() const {
	return code_;
}
const size_t Type::size() const {
	return size_;
}
const char* Type::name() const {
	return name_;
}
const Type* Type::parentType() const {
	return parentType_;
}

bool Type::instanceOf(Object* obj, const char* type) {
	bool res = false;
	if (obj != NULL) {
		const Type* t = obj->getType();
		while (t != NULL) {
			if (strncmp(t->name_, type, 256) == 0) {
				res = true;
				break;
			}
			t = (Type*)t->parentType_;
		}
	}
	return res;
}
bool Type::instanceOf(Object* obj, size_t t) {
	Type& type = Type::types_[t];
	return Type::instanceOf(obj, type.name_);
}

const Type* Type::getChar() {
	return Type::char_;
}
const Type* Type::getBool() {
	return Type::bool_;
}
const Type* Type::getShort() {
	return Type::short_;
}
const Type* Type::getInt() {
	return Type::int_;
}
const Type* Type::getSize_t() {
	return Type::size_t_;
}
const Type* Type::getFloat() {
	return Type::float_;
}
const Type* Type::getDouble() {
	return Type::double_;
}

//
//bool Type::toBool(Object* obj) {
//	bool res = false;
//	const Type* t = obj->getType();
//	const size_t code = t->code_;
//	switch (true) {
//		if (t == TYPE_OBJECT) {
//			res = this == Object::null();
//			break;
//		}
//		if (t->code() == TYPE_STRING) {
//			res = ((String*)this)->length() == 0;
//			break;
//		}
//		if (t->code() == TYPE_BOOLEAN) {
//			res = (bool)this->valueOf();
//			break;
//		}
//		if (t->instanceOf(TYPE_NUMBER)) {
//			Number* n = (Number*)this;
//			res = !Number::isNaN(n) && n->toLong() != 0;
//			break;
//		}
//
//		//if (code == TYPE_INTEGER) {
//		//	res = Number::isNaN(obj) && (int)obj->valueof() != 0;
//		//	break;
//		//}
//		//if (code == TYPE_LONG) {
//		//	res = Number::isNaN(obj) && (long long)obj->valueof() != 0;
//		//	break;
//		//}
//		//if (code == TYPE_FLOAT) {
//		//	res = Number::isNaN(obj) && (float)obj->valueof() != 0.0f;
//		//	break;
//		//}
//		//if (code == TYPE_DOUBLE) {
//		//	res = Number::isNaN(obj) && (double)obj->valueof() != 0.0;
//		//	break;
//		//}
//		break;
//	}
//	return res;
//}

NS_FW_BASE_END