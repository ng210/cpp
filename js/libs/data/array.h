#ifndef __ARRAY_H
#define __ARRAY_H

#include "base/Object.h"
#include "base/Function.h"
#include "base/String.h"
#include <stdarg.h>

NS_FW_BASE_BEGIN

#define TYPE_ARRAY GET_TYPE(Array)

typedef int Compare(Object*, Object*);

/*****************************************************************************
* Object_ (structure)
*****************************************************************************/
class Array : public Object {
	friend class RunTime;
	DECL_TYPE(Array);
	Array(size_t);
	static void initialize();
	static void shutDown();
protected:
	Object** data_;
protected: PROP_R(size_t, length);
protected: PROP_R(size_t, capacity);

	char* join_(const char* sep);
	long long binSearch(Object*, size_t, size_t, Compare*);
	void sort(long long, long long, Compare*);
	static int compare(Object*, Object*);
	void init(size_t, va_list = NULL);
public:
	Array();
	Array(int, ...);
	virtual ~Array();

	char* toString();
	int compareTo(Object* obj);
	void* valueOf();

	void cleanUp();
	Array* concat(Array*);
	Object* operator[](long long);
	void fill(Object*, size_t = 0, size_t = 0);
	Array* filter(Function*, Object* = NULL);
	Object* find(Function*, Object* = NULL);
	long long findIndex(Function*, Object* = NULL);
	void forEach(Function*, Object* = NULL);
	Object* get(size_t);
	long long indexOf(Object*, long long = 0);
	String* join(String*);
	String* join(const char*);
	long long lastIndexOf(Object*, long long = -1);
	Array* map(Function*, Object* = NULL);
	Object* pop();
	size_t push(Object*);
	size_t push(const char*);
	size_t push(size_t, ...);
	size_t push_(size_t, va_list);
	Object* put(size_t, Object*);
	Array* reverse();
	long long search(Object* key, Compare* compare = Array::compare);
	Object* shift();
	Array* slice(long long = 0, long long = -1);
	void sort(Compare* = Array::compare);
	Array* splice(long long, size_t, size_t = 0, ...);
	Array* splice(long long, size_t, size_t, va_list);
	size_t unshift(Object*);
	size_t unshift(size_t, ...);
	size_t unshift(size_t, va_list);
};

NS_FW_BASE_END

#endif