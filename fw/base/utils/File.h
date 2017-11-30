#ifndef __FILE_H
#define __FILE_H

#include "base/base_defs.h"
#include "utils/Buffer.h"

NS_FW_BASE_BEGIN

class File {
public:
	static char* read(const char*);
	static char* read(String*);

	static size_t write(const char*, char*, size_t);
	static size_t write(String*, char*, size_t);
	static size_t write(const char*, Buffer&);
};

NS_FW_BASE_END

#endif