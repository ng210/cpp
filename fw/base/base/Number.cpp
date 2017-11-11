#include "Number.h"

NS_FW_BASE_BEGIN

const Type* Number::classType_;

bool Number::isNaN(Number* n) {
	return n->isNaN_;
}

void Number::initialize() {
	Number::classType_ = Type::add(STRINGIFY(NS_FW_BASE)"::number");
}
void Number::shutDown() {
}

Number::Number() {
	value_.l = 0;
	isNaN_ = false;
}

int Number::toInt() {
	return value_.i;
}
long long Number::toLong() {
	return value_.l;
}
float Number::toFloat() {
	return value_.f;
}
double Number::toDouble() {
	return value_.d;
}

NS_FW_BASE_END