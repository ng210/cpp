#include "base/Type.h"
#include "base/str.h"

NS_FW_BASE_BEGIN

Type::Type() : code_(0) {
	name_ = NULL;
}

size_t Type::typeCount_ = 0;
Type Type::types_[1024];

void Type::initialize() {
	memset(types_, 0, sizeof(Type) * MAX_TYPE_COUNT);
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

Type* Type::add(const char* name, const char* parent) {
	Type* type = &Type::types_[typeCount_];
	type->code_ = typeCount_++;
	type->name_ = name;
	type->parentType_ = parent != NULL ? Type::getTypeByName(parent) : NULL;
	return type;
}

const size_t Type::code() const {
	return code_;
}
const char* Type::name() const {
	return name_;
}
const Type* Type::parentType() const {
	return parentType_;
}


bool Type::instanceOf(const char* type) const {
	bool res = false;
	const Type* t = this;
	while (t != NULL) {
		if (strncmp(t->name_, type, 256) == 0) {
			res = true;
			break;
		}
		t = (Type*)t->parentType_;
	}
	return res;
}
bool Type::instanceOf(size_t t) const {
	Type& type = Type::types_[t];
	return Type::instanceOf(type.name_);
}


NS_FW_BASE_END