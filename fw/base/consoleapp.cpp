#include "consoleapp.h"
#include <stdio.h>

NS_FW_BASE_USE

int main(int argc, char** argv) {
	int error = 0;
#ifdef _DEBUG
	MemoryMgr::isDebugOn = true;
#endif
	RunTime::initialize();
	Array* args = NEW_(Array);
	for (int i = 0; i < argc; i++) {
		args->push(NEW_(String, (const char*)argv[i]));
	}
	error = _main(args);

	args->cleanUp();
	DEL_(args);

	RunTime::shutDown();
#ifdef _DEBUG
	//void* exceptions[2] = {
	//	(void*)String::empty(),
	//	(void*)Object::null()
	//};
	MemoryMgr::checkMemDbgInfo(0, NULL);
#endif

	return error;
}