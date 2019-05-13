#ifndef __DIRECTORY_H
#define __DIRECTORY_H

#include <windows.h>
#include "collection/array.h"

NS_FW_BASE_BEGIN

typedef int ENUMDIRENTRYPROC(const char* fileName, const char* path, void* args);

class Directory {
	static ENUMDIRENTRYPROC getFilesProc;
public:
	static Array* getFiles(const char* path, bool isRecursive= false, const char* filterString = NULL);
	static void enumEntries(const char* path, ENUMDIRENTRYPROC* proc, bool isRecursive, void* args = NULL, const char* filterString = NULL);
	static void enumEntries(const char* path, ENUMDIRENTRYPROC* proc, bool isRecursive, void* args = NULL, char** filters = NULL);
};

NS_FW_BASE_END

#endif