#ifndef __DIRECTORY_H
#define __DIRECTORY_H

#include <windows.h>
#include "collection/array.h"

NS_FW_BASE_BEGIN

class Directory {
public:
	static Array* getFiles(const char* path, const char* filters = NULL);
};

NS_FW_BASE_END

#endif