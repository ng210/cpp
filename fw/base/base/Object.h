#ifndef __OBJECT_H
#define __OBJECT_H

#include "base/base_defs.h"

NS_FW_BASE_BEGIN

/*****************************************************************************
* Object
*****************************************************************************/
class Object {
	static Object nullInstance_;
	PROP_R(protected, long long, Hash, i);
public:
	Object(void);
	virtual ~Object(void);

	static Object* null();

	virtual int strcmp(const char*);
	virtual const char* getType();
	virtual char* toString();
	virtual int compareTo(Object*);
	virtual void* valueOf();

#ifdef _DEBUG
	static bool isDebugOn;
	void dump(const char* = 0);
#endif
	//*********************************************************
};

NS_FW_BASE_END

#endif