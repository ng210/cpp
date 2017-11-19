#ifndef __PATHINFO_H
#define __PATHINFO_H

#include "base/base_defs.h"

NS_FW_BASE_BEGIN

class PathInfo {
	char *buffer_;
	char *path_;
	char *fileName_;
	char *extension_;
public:
	PathInfo(const char *fileName);
	~PathInfo();
	char* getPath();
	char* getFileName();
	char* getExtension();
};

NS_FW_BASE_END

#endif