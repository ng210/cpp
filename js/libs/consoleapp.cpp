#include "consoleapp.h"
#include <stdio.h>

NS_FW_BASE_USE

int main(int argc, char** argv) {
	int error = 0;
#ifdef _DEBUG
	//MemoryMgr::isDebugOn = true;
	tracer_ = new Tracer();
#endif
	RunTime::initialize();
	Array* arr = NEW_(Array);
	for (int i = 0; i < argc; i++) {
		arr->push(NEW_(String, (const char*)argv[i]));
	}
	String* argSeparator = NEW_(String, "=");
	Map* args = NEW_(Map, arr, argSeparator);
	DEL_(argSeparator);
	arr->cleanUp();
	DEL_(arr);
	error = _main(args);
	args->keys()->cleanUp();
	args->values()->cleanUp();
	args->cleanUp();
	DEL_(args);

	RunTime::shutDown();
#ifdef _DEBUG
	//void* exceptions[2] = {
	//	(void*)String::empty(),
	//	(void*)Object::null()
	//};
	MemoryMgr::checkMemDbgInfo(0, NULL);
	delete tracer_;
#endif

	return error;
}