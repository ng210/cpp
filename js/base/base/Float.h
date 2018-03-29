#ifndef __FLOAT_H
#define __FLOAT_H

#include "base/Number.h"

NS_FW_BASE_BEGIN

//#define TYPE_FLOAT GET_TYPE(Float)

/*****************************************************************************
* Float
*****************************************************************************/
class Float : public Number {
	friend class RunTime;
	DECL_TYPE(Float);
	static void initialize();
	static void shutDown();
public:

	Float();
	Float(float);
	virtual ~Float();

	virtual char* toString();
	virtual int compareTo(Object*);
	virtual void* valueOf();
};

NS_FW_BASE_END

#endif