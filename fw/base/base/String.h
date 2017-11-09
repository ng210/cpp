#ifndef __STRING_H
#define __STRING_H

#include "base/Object.h"

NS_FW_BASE_BEGIN

/*****************************************************************************
* String
*****************************************************************************/
class String : public Object {
	static const String emptyInstance_;
protected:
	char *buffer_;
	PROP_R(protected, size_t, length, );
public:
	String(void);
	String(char *p);
	String(const char *p);
	String(String&);
	virtual ~String(void);

	static const char* empty();

	const char* getType();
	char* toString();
	int strcmp(const char*);
	int compareTo(Object* obj);
	void* valueOf();

	//String* operator=(const String*);
	char operator[](int);
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
	String* substr(long long, long long = 0);
	String* substring(long long, long long = 0);
	String** split(String*);
	String** split(const char*);
	String* toLowerCase();
	String* toUpperCase();
	String* trim();
};

NS_FW_BASE_END

#endif
