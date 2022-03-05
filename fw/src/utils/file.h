#ifndef __FILE_H
#define __FILE_H

#include "basedef.h"
#include "utils/buffer.h"

NS_FW_BASE_BEGIN

class File {
public:
	static bool exists(const char* path);
	static size_t read(const char* path, UINT8** buffer, size_t offset = 0, size_t byteCount = 0);
	static size_t read(const char* path, Buffer* buffer, size_t offset = 0, size_t byteCount = 0);
	static size_t write(const char* path, UINT8* data, size_t length);
	static size_t write(const char* path, Buffer* data);
	static size_t getSize(const char* path);
};

NS_FW_BASE_END

#endif