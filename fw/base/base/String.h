#ifndef __STRING_H
#define __STRING_H

#include "base/Object.h"

NS_FW_BASE_BEGIN

#define TYPE_STRING GET_TYPE(String)

class Array;
/*****************************************************************************
* String
*****************************************************************************/
class String : public Object {
	friend class RunTime;
	DECL_TYPE(String);
	static const String* emptyInstance_;
	static void initialize();
	static void shutDown();
protected:
	char *value_;
	PROP_R(size_t, length);
public:
	String(void);
	String(char *p);
	String(const char *p);
	String(String&);
	virtual ~String(void);

	static const char* empty();

	int strcmp(const char*);
	char* toString();
	bool toBool();
	Number* toNumber();
	int compareTo(Object*);
	void* valueOf();

	//String* operator=(const String*);
	char operator[](int);
	bool operator==(Object&);
	String& operator=(String&);
	String& operator=(const char*);
	String* concat(String*);
	String* concat(const char*);
	bool startsWith(String*);
	bool startsWith(char*);
	long long indexOf(String*);
	long long indexOf(const char*);
	long long indexOf(const char*, size_t);
	long long lastIndexOf(String*);
	long long lastIndexOf(char*);
	bool endsWith(String*);
	bool endsWith(char*);
	static String* join(String**, String*);
	static String* join(String**, const char* = NULL, size_t length = 0);
	//? match(RegExp&);
	String* replace(String*, String*);
	String* replace(const char*, const char*);
	//String replace(Array&, Array&);
	String* substr(long long, size_t = 0);
	String* substring(long long, long long = 0);
	String** split_(String*);
	String** split_(const char*);
	Array* split(String*);
	Array* split(const char*);
	String* toLowerCase();
	String* toUpperCase();
	String* trim();
};

NS_FW_BASE_END

#endif
