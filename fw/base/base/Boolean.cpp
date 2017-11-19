#include "Boolean.h"

NS_FW_BASE_BEGIN

Boolean* Boolean::trueInstance_;
Boolean* Boolean::falseInstance_;
const Type* Boolean::classType_;

void Boolean::initialize() {
	Boolean::trueInstance_ = new Boolean(true);
	Boolean::falseInstance_ = new Boolean(false);
	Boolean::classType_ = Type::add(STRINGIFY(NS_FW_BASE)"::bool");
}

void Boolean::shutDown() {
	delete Boolean::trueInstance_;
	delete Boolean::falseInstance_;
}

Boolean::Boolean() {
	value_ = false;
}
Boolean::Boolean(bool v) {
	value_ = v;
}
Boolean::~Boolean() {

}

void* Boolean::valueOf() {
	return (void*)value_;
}

NS_FW_BASE_END