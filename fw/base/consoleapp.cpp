#include "consoleapp.h"
#include <stdio.h>

NS_FW_BASE_USE

int main(int argc, char** argv) {
	int error = 0;
	//MemoryMgr::isDebugOn = true;
	Array* args = NEW_(Array);
	for (int i = 0; i < argc; i++) {
		args->push(NEW_(String, (const char*)argv[i]));
	}
	error = _main(args);

	args->cleanUp();
	DEL_(args);
#ifdef _DEBUG
	void* exceptions[2] = {
		(void*)String::empty(),
		(void*)Object::null()
	};
	MemoryMgr::checkMemDbgInfo(2, exceptions);
#endif
	return error;
}