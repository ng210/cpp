#ifndef __FILE_H
#define __FILE_H

#include "basedef.h"
#include "utils/Buffer.h"

NS_FW_BASE_BEGIN

class File {
public:
	static BYTE* read(const char* path);
	static size_t write(const char* path, BYTE* data, size_t length);
	static size_t write(const char* path, Buffer* data);
};

NS_FW_BASE_END

#endif