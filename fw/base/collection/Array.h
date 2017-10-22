#ifndef __ARRAY_H
#define __ARRAY_H

#include "base/Object.h"
#include "base/Function.h"
#include "base/String.h"
#include <stdarg.h>

NS_FW_BASE_BEGIN

/*****************************************************************************
* Object_ (structure)
*****************************************************************************/
class Array : public Object {
	Array(size_t);
protected:
	Object** data_;
	PROP_R(protected, size_t, length, );
	size_t capacity_;
public:
	Array();
	Array(size_t, Object*, ...);
	//Array(size_t, Object**);
	virtual ~Array();
	void init(size_t);

	const char* getType();
	const char* toString();
	int compareTo(Object* obj);
	void* valueOf();

	Array* concat(Array*);
	Object* operator[](long long);
	void fill(Object*, size_t = 0, size_t = 0);
	Array* filter(Function*, Object* = NULL);
	Object* find(Function*, Object* = NULL);
	long long findIndex(Function*, Object* = NULL);
	void forEach(Function*, Object* = NULL);
	long long indexOf(Object*, size_t = 0);
	String* join(String*);
	String* join(const char*);
	const char* join_(const char* sep);
	long long lastIndexOf(Object*, size_t = -1);
	Array* map(Function*, Object* = NULL);
	Object* pop();
	size_t push(Object*);
	size_t push(size_t, ...);
	size_t push(size_t, va_list);
	Array* reverse();
	Object* shift();
	Array* slice(long long = 0, long long = 0);
	void sort(Function*);
	Array* splice(long long, size_t, size_t = 0, ...);
	Array* splice(long long, size_t, size_t, va_list);
	size_t unshift(Object*);
	size_t unshift(size_t, ...);
	size_t unshift(size_t, va_list);
};

NS_FW_BASE_END

#endif