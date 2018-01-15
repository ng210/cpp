#ifndef __FILE_H
#define __FILE_H

#include "base/base_defs.h"
#include "utils/Buffer.h"

NS_FW_BASE_BEGIN

class File {
public:
	static char* read(const char*);
	static char* read(String& path);

	static size_t write(const char* path, char* data, size_t length);
	static size_t write(String& path, char* data, size_t length);
	static size_t write(const char* path, Buffer& data);
	static size_t write(String& path, Buffer& data);
};

NS_FW_BASE_END

#endif