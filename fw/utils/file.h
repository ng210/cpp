#ifndef __FILE_H
#define __FILE_H

#include <stdio.h>
#include "basedef.h"
#include "base/stream.h"
#include "utils/buffer.h"

NS_FW_BASE_BEGIN

class File {
public:
	static bool exists(const char* path);

	static int seek(FILE* fp, long offset, int origin = SEEK_SET);

	static size_t read(const char* path, UINT8** buffer, size_t offset = 0, size_t byteCount = 0);
	static size_t read(const char* path, Buffer* buffer, size_t offset = 0, size_t byteCount = 0);
	
	static size_t write(const char* path, UINT8* data, size_t length, bool update = false);
	static size_t write(FILE* fp, UINT8* buffer, size_t length);

	static size_t write(const char* path, Buffer* data, bool update = false);
	static size_t write(FILE* fp, Buffer* buffer);

	static size_t append(const char* path, UINT8* data, size_t length);
	static size_t append(const char* path, Buffer* data);

	static size_t getSize(const char* path);

	static FILE* open(const char* path, const char* mode = NULL);
};

NS_FW_BASE_END

#endif