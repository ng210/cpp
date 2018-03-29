#include "consoleapp.h"
#include <stdio.h>

NS_FW_BASE_USE

int main(int argc, char** argv) {
	int error = 0;
#ifdef _DEBUG
	//MemoryMgr::isDebugOn = true;
	Debug::initialize(/*DEBUG_UNICODE | DEBUG_MEMORY*/);
#endif
	//RunTime::initialize();
	Map* args = NEW_(Map, 255*sizeof(char), 255*sizeof(char));
	args->compare(Map::compareStr);
	for (int i = 0; i < argc; i++) {
		int count = 0;
		Array* tokens = Array::strsplit(argv[i], "=");
		if (tokens != NULL && tokens->length() > 0) {
			char* key = strtrim(*(char**)tokens->getAt(0));
			char* value = count > 1 ? strtrim(*(char**)tokens->getAt(1)) : "1";
			args->put(key, value);
			FREE(key);
			if (count > 1) FREE(value);
			for (UINT32 j = 0; j < tokens->length(); j++) {
				FREE(*(char**)tokens->getAt(j));
			}
			DEL_(tokens);
		}
	}
	error = _main(args);
	DEL_(args);

	//RunTime::shutDown();
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