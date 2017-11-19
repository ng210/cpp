#ifndef __NUMBER_H
#define __NUMBER_H

#include "base/Object.h"

NS_FW_BASE_BEGIN

#define TYPE_NUMBER (Number::classType()->code())

/*****************************************************************************
* Integer
*****************************************************************************/
class Number : public Object {
	friend class RunTime;
	DECL_TYPE(Number);
	static void initialize();
	static void shutDown();
	PROP_R(bool, isNaN);
public:
	union {
		int i;
		long long l;
		float f;
		double d;
	} value_;

	Number();

	static bool isNaN(Number*);

	int toInt();
	long long toLong();
	float toFloat();
	double toDouble();
};


NS_FW_BASE_END

#endif