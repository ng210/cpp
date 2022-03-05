#include "utils/directory.h"
#include "base/memory.h"
#include "base/string.h"
#include "base/debug.h"

NS_FW_BASE_BEGIN

int Directory::getFilesProc(const char* fileName, const char* path, void* args) {
	Array* files = (Array*)args;
	files->add((char*)fileName);
	return 1;
}

Array* Directory::getFiles(const char* path, bool isRecursive, const char* filterString) {
	Array* files = NEW_(Array, MAX_PATH);
	Directory::enumEntries(path, Directory::getFilesProc, isRecursive, files, filterString);
	return files;
}

void Directory::enumEntries(const char* path, ENUMDIRENTRYPROC* proc, bool isRecursive, void* args, char** filters) {
	char fullPath[MAX_PATH];
	int length = strlen(path);
	if (length < MAX_PATH - 3) {
		// prepare input path: "<path>\\*\0"
		char* ptr = fullPath;
		strncpy(ptr, length, path);
		ptr += length;
		if (ptr[-1] != '\\') {
			*ptr++ = '\\';
		}
		*ptr++ = '*';
		*ptr++ = '\0';

		WIN32_FIND_DATA findFileData;
		BOOL hasNext = true;
		char fileName[MAX_PATH];
		HANDLE SYSFN(hFile, FindFirstFile(fullPath, &findFileData));
		if (hFile != NULL && hFile != INVALID_HANDLE_VALUE) {
			while (hasNext) {
				// check for recursion
				if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
					if (strncmp(findFileData.cFileName, ".") != 0 && strncmp(findFileData.cFileName, "..") != 0) {
						if (isRecursive) {
							char* subdir = str_format("%s\\%s", path, findFileData.cFileName);
							Directory::enumEntries(subdir, proc, isRecursive, args, filters);
							FREE(subdir);
						}
					}
				}
				else {
					// check filters
					int fi = 0;
					bool isMatching = false;
					while (filters[fi] != NULL) {
						if (str_ends(findFileData.cFileName, filters[fi])) {
							isMatching = true;
							break;
						}
						fi++;
					}
					if (isMatching) {
						// get path
						strncpy(fileName, MAX_PATH, findFileData.cFileName);
						// callback
						if (proc(fileName, path, args) == 0) {
							break;
						}
					}
				}
				SYSFN(hasNext, FindNextFile(hFile, &findFileData));
			}
			SYSPR(FindClose(hFile));
		}
		else {
			// no matching file found!
			;
		}
		// findclose
		//SYSPR(FindClose(hFile));
		//while ((filter = filters[fi++]) != NULL) {
		//	// findfirst
		//	HANDLE SYSFN(hFile, FindFirstFile(fullPath, &findFileData));
		//	if (hFile != INVALID_HANDLE_VALUE) {
		//		// findnext
		//		do {
		//			if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
		//				; // recursion
		//			}
		//			else {
		//				// check filter
		//				if (str_ends(findFileData.cFileName, filter)) {
		//					// ensure necessary length of item (although, cFileName is currently MAX_PATH long)
		//					strncpy(fileName, MAX_PATH, findFileData.cFileName);
		//					files->add(fileName);
		//				}
		//			}
		//			SYSFN(hasNext, FindNextFile(hFile, &findFileData));
		//		} while (hasNext);
		//	}
		//}
	}
}

void Directory::enumEntries(const char* path, ENUMDIRENTRYPROC* proc, bool isRecursive, void* args, const char* filterString) {
	char** filters = NULL;
	if (filterString != NULL) {
		// filters extracted from filter string
		filters = str_split(filterString, ";");	// \0?
	}
	else {
		// no filters
		filters = (char**)MALLOC(char*, 2);
		filters[0] = (char*)&filters[1];
		filters[1] = NULL;
	}
	Directory::enumEntries(path, proc, isRecursive, args, filters);
	int i = 0;
	char* filter = NULL;
	while ((filter = filters[i++]) != NULL) {
		FREE(filter);
	}
	FREE(filters);
}


NS_FW_BASE_END