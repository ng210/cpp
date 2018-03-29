#ifndef __PATHINFO_H
#define __PATHINFO_H

#include "base/base_defs.h"
#include "base/String.h"

NS_FW_BASE_BEGIN

class PathInfo {
	String* path_;
	String* fileName_;
	String* extension_;
public:
	PathInfo(String*);
	PathInfo(const char*);
	~PathInfo();
	void init(const char*);
	String* getPath();
	String* getFileName();
	String* getExtension();
};

NS_FW_BASE_END

#endif