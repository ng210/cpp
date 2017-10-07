#ifndef __ARRAY_H
#define __ARRAY_H

#include "base/Object.h"
#include "base/Null.h"

NAMESPACE_FRMWRK_BEGIN

/*****************************************************************************
* Object_ (structure)
*****************************************************************************/
class Array_ : public Object_ {
	friend class Array;
protected:
	Object* data_;
	size_t length_;

	Array_();
	virtual ~Array_();

	int compareTo(Object_* str);
	String toString();
};


/*****************************************************************************
* Object (reference)
*****************************************************************************/
class Function;
class Array : public Object {
public:
	Array();
	Array(Object&, ...);
	virtual ~Array();

	Array concat(Array&);
	Array operator+(Array&);
	Array fill(Object&, size_t, size_t);
	Array filter(Function&, Object&);
};

NAMESPACE_FRMWRK_END

#endif