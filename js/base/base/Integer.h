#ifndef __INTEGER_H
#define __INTEGER_H

#include "base/Number.h"

NS_FW_BASE_BEGIN

//#define TYPE_INTEGER GET_TYPE(Integer)

/*****************************************************************************
* Integer
*****************************************************************************/
class Integer : public Number {
	friend class RunTime;
	DECL_TYPE(Integer);
	static void initialize();
	static void shutDown();
public:

	Integer();
	Integer(int);
	virtual ~Integer();

	virtual char* toString();
	virtual int compareTo(Object*);
	virtual void* valueOf();
};

NS_FW_BASE_END

#endif