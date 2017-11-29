#include "runtime.h"

NS_FW_BASE_BEGIN

NS_FW_BASE_USE

void RunTime::initialize() {
	// initialize classes
	Type::initialize();
	Object::initialize();
	String::initialize();
	Boolean::initialize();
	Number::initialize();
	Integer::initialize();
	Float::initialize();

	Array::initialize();
	Map::initialize();
}
void RunTime::shutDown() {
	// shutdown classes
	Map::shutDown();
	Array::shutDown();

	Float::shutDown();
	Integer::shutDown();
	Number::shutDown();
	Boolean::shutDown();
	String::shutDown();
	Object::shutDown();
	Type::shutDown();
}

NS_FW_BASE_END