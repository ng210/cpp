#ifndef __PATHINFO_H
#define __PATHINFO_H

#include "basedef.h"

NS_FW_BASE_BEGIN

class PathInfo {
	char* buffer_;
	void init(const char*);
protected: PROP_R(char*, path);
protected: PROP_R(char*, fileName);
protected: PROP_R(char*, extension);
public:
	PathInfo(char*);
	PathInfo(const char*);
	~PathInfo();
};

NS_FW_BASE_END

#endif