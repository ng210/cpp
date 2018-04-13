#ifndef __STRING_H
#define __STRING_H

#include <stdarg.h>
#include "str.h"
//#include "collection/array.h"

NS_FW_BASE_BEGIN

char*	str_concat(const char* str1, const char* str2);
char*	str_concat(int argc, ...);
int		str_ends(const char* str, const char* part);
int		str_indexOf(const char* str, const char* part, UINT32 offset = 0);
char*	str_join(int argc, const char* filler, ...);
char*	str_join(char** args, const char* filler);
int		str_lastIndexOf(const char* str, const char* part);
char*	str_replace(const char* str, const char* oldStr, const char* newStr);
int		str_starts(const char* str, const char* part);
char*	str_substr(const char* str, int start, int end = 0);
char*	str_substring(const char* str, int start, int length = 0x3ffffff);
char**	str_split_(const char* str, const char* splitter);
char*	str_ltrim(const char* src, const char* str = " \t\n");
char*	str_rtrim(const char* src, const char* str = " \t\n");
char*	str_trim(const char* src, const char* str = " \t\n");

NS_FW_BASE_END

#endif

//class Array;
///*****************************************************************************
//* String
//*****************************************************************************/
//class String {
//	static const String* emptyInstance_;
//protected:
//	char *value_;
//	PROP_R(size_t, length);
//public:
//	String(void);
//	String(char *p);
//	String(const char *p);
//	String(String&);
//	virtual ~String(void);
//
//	static const char* empty();
//
//	int strcmp(const char*);
//	char* toString();
//	bool toBool();
//	Number* toNumber();
//	int compareTo(Object*);
//	void* valueOf();
//
//	//String* operator=(const String*);
//	char operator[](int);
//	bool operator==(Object&);
//	String& operator=(String&);
//	String& operator=(const char*);
//	String* concat(String*);
//	String* concat(const char*);
//	bool startsWith(String*);
//	bool startsWith(char*);
//	long long indexOf(String*);
//	long long indexOf(const char*);
//	long long indexOf(const char*, size_t);
//	long long lastIndexOf(String*);
//	long long lastIndexOf(char*);
//	bool endsWith(String*);
//	bool endsWith(char*);
//	static String* join(String**, String*);
//	static String* join(String**, const char* = NULL, size_t length = 0);
//	//? match(RegExp&);
//	String* replace(String*, String*);
//	String* replace(const char*, const char*);
//	//String replace(Array&, Array&);
//	String* substr(long long, size_t = 0);
//	String* substring(long long, long long = 0);
//	String** split_(String*);
//	String** split_(const char*);
//	Array* split(String*);
//	Array* split(const char*);
//	String* toLowerCase();
//	String* toUpperCase();
//	String* trim();
//};
