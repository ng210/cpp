#ifndef __STRING_H
#define __STRING_H

#include "base/Object.h"

NAMESPACE_FRMWRK_BEGIN

/*****************************************************************************
* Object_ (structure)
*****************************************************************************/
class String;
class String_ : public Object_ {
	friend class String;
	String_(char *p);
	String_(const char *p);
	static const String_ emptyInstance_;
protected:
	char *buffer_;
	size_t length_;

	String_(void);
	virtual ~String_(void);

	String toString();
	int compareTo(Object_* str);
};

/*****************************************************************************
* Object (reference)
*****************************************************************************/
class Null;
class Array;
class String : public Object {
	friend class Object;
	String(const String_*);
	void empty_();
public:
	//static const String emptyInstance_;
	static const String Empty;

	String(void);
	String(String&);
	String(const Null&);
	String(char*);
	String(const char*);
	~String(void);

	//String operator=(String&);
	String operator=(const String&);
	//String operator=(const Null&);
	char operator[](int);
	String operator+(String&);
	String operator+(const char*);
	bool startsWith(String&);
	bool startsWith(char*);
	long long indexOf(String&);
	long long indexOf(const char*);
	long long indexOf(const char*, size_t);
	long long lastIndexOf(String&);
	long long lastIndexOf(char*);
	bool endsWith(String&);
	bool endsWith(char*);
	//? match(RegExp&);
	String replace(String&, String&);
	String replace(const char*, const char*);
	//String replace(Array&, Array&);
	String substr(long long, long long = 0);
	String substring(long long, long long = 0);
	Array split(String&);
	Array split(const char*);
	String toLowerCase();
	String toUpperCase();
	String trim();

	const char* toCString();
	//void empty();
};

NAMESPACE_FRMWRK_END

#endif
