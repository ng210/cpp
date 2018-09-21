#include "utils/directory.h"
#include "base/memory.h"
#include "base/string.h"
#include "base/debug.h"

NS_FW_BASE_BEGIN

Array* Directory::getFiles(const char* path, const char* filterString) {
	Array* files = NEW_(Array, MAX_PATH);
	char fullPath[MAX_PATH];
	int length = strlen(path);
	if (length < MAX_PATH - 3) {
		char* ptr = fullPath;
		strncpy(ptr, length, path);
		ptr += length;
		if (ptr[-1] != '\\') {
			*ptr++ = '\\';
		}
		*ptr++ = '*';
		*ptr++ = '\0';
		char** filters = NULL;
		if (filterString != NULL) {
			filters = str_split(filterString, ";");	// \0?
		} else {
			filters = (char**)MALLOC(char*, 2);
			filters[0] = (char*)&filters[1];
			filters[1] = NULL;
		}
		char* filter;
		int fi = 0;
		WIN32_FIND_DATAA findFileData;
		BOOL hasNext = false;
		char fileName[MAX_PATH];
		while ((filter = filters[fi++]) != NULL) {
			// findfirst
			HANDLE SYSFN(hFile, FindFirstFile(fullPath, &findFileData));
			if (hFile != INVALID_HANDLE_VALUE) {
				// findnext
				do {
					if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
						; // recursion
					}
					else {
						// check filter
						if (str_ends(findFileData.cFileName, filter)) {
							// ensure necessary length of item (although, cFileName is currently MAX_PATH long)
							strncpy(fileName, MAX_PATH, findFileData.cFileName);
							files->add(fileName);
						}
					}
					SYSFN(hasNext, FindNextFile(hFile, &findFileData));
				} while (hasNext);
			} else {
				// no plugins found!
				;
			}
			//findclose
			SYSPR(FindClose(hFile));
		}
		FREE(filters);		
	}
	return files;
}

NS_FW_BASE_END