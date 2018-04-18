#include "consoleapp.h"
#include <stdio.h>

NS_FW_BASE_USE

static char* workingDir_;

char* getWorkingDir() {
	return workingDir_;
}

int main(int argc, char** argv) {
	int error = 0;
#ifdef _DEBUG
	//MemoryMgr::isDebugOn = true;
	Debug::initialize(/*DEBUG_UNICODE | DEBUG_MEMORY*/);
#endif
	//RunTime::initialize();
	Map* args = NEW_(Map, MAP_USE_REF, MAP_USE_REF, Map::hashingStr, Collection::compareStr);
	PathInfo* workingDir = NEW_(PathInfo, argv[0]);
	workingDir_ = workingDir->path();
	for (int i = 0; i < argc; i++) {
		int count = 0;
		size_t pos = strcspn(argv[i], "=");
		char* key = substr(argv[i], 0, pos);
		char* value = (argv[i][pos] != '\0') ? substr(argv[i], pos+1, NS_FW_BASE::strlen(argv[i])) : NS_FW_BASE::strdup("1");
		args->put(key, value);
	}
	error = _main(args);
	MAP_FOREACH(args, FREE(key); FREE(value));
	DEL_(args);
	DEL_(workingDir);

#ifdef _DEBUG
	//void* exceptions[2] = {
	//	(void*)String::empty(),
	//	(void*)Object::null()
	//};
	MemoryMgr::checkMemDbgInfo(0, NULL);

	Debug::destroy();
#endif

	return error;
}