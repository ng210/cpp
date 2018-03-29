#ifndef __ARRAY_H
#define __ARRAY_H

#include "base/Object.h"
#include "base/Null.h"
#include "base/String.h"

NAMESPACE_FRMWRK_BEGIN

/*****************************************************************************
* Object_ (structure)
*****************************************************************************/
class Array_ : public Object_ {
	friend class Array;
protected:
	Object* data_;
	size_t length_;
	size_t capacity_;

	Array_();
	//Array_(size_t, ...);
	Array_(size_t, Object**);
	virtual ~Array_();
	void init_(size_t);

	String join_(String& = String(","));
	void push_(Object&);
	Object& pop_();
	Array splice_(long long, size_t, size_t, va_list);
	size_t unshift_(size_t, va_list);

	const char* getType();
	int compareTo(Object_* str);
	String toString();
	void* valueOf();
};


/*****************************************************************************
* Object (reference)
*****************************************************************************/
class Array : public Object {
public:
	Array();
	Array(Array&);
	Array(const Null&);
	//Array(size_t, Object*);
	Array(size_t argc, ...);
	virtual ~Array();

	Array concat(Array&);
	Array operator+(Array&);
	Object& operator[](long long);
	Array& fill(Object&, size_t = 0, size_t = 0);
	Array filter(Function&, Object& = (Object&)null);
	Object find(Function&, Object& = (Object&)null);
	long long findIndex(Function&, Object& = (Object&)null);
	void forEach(Function&, Object& = (Object&)null);
	long long indexOf(Object&, size_t = 0);
	String join(String&);
	long long lastIndexOf(Object&, size_t = -1);
	Array map(Function&, Object& = (Object&)null);
	Object pop();
	void push(Object&);
	void push(size_t, ...);
	Array& reverse();
	Object shift();
	Array slice(long long = 0, long long = 0);
	Array& sort(Function&);
	Array splice(long long, size_t, size_t = 0, ...);
	size_t unshift(Object&);
	size_t unshift(size_t, ...);

};

NAMESPACE_FRMWRK_END

#endif