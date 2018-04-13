#ifndef __SOURCE_H

#define __SOURCE_H

#include "collection/parray.h"

NS_FW_BASE_USE

class Source {
private: PROP_R(const char*, name);
private: PROP_R(PArray, includes);
private: PROP(size_t, flag);
private: PROP_R(PArray*, content);
public:
	Source(void);
	Source(const char* name);
	~Source(void);
	void init(const char* name);
	void addInclude(Source *inc);
	size_t hasIncludes();
	Source* getInclude(UINT32 ix);
};

#endif