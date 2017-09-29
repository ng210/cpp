#ifndef __STRING_H
#define __STRING_H

#include "Base/Object.h"

NAMESPACE_FRMWRK_BEGIN

class String;

class String_ : public Object_ {
friend class String;
protected:
	const char *buffer_;
	size_t iLength_;
public:
	String_(void);
	virtual ~String_(void);

	void assign(char *szStr);
	//void assign(const char *szStr);
	//void assign(String_& str);
	char* toCString();
	int compareTo(const char* str);

	size_t length();

};

class String : public Object {
public:
	String(void);
	String(char *szStr);
	String(const char *szStr);

	String& operator=(char *szStr);
	DEFINE_COMMON_OPERATORS(String)
	void assign(char *szStr);

	// Inherited from Object
	DEFINE_FUNC0(String, toCString, char*);
	int compareTo(String& str);
	DEFINE_FUNC0(String, length, size_t);

};

//class String : public Ref<String_> {
//protected:
//	void assign(char *szStr);
//
//public:
//	String(void);
//	String(char *szStr);
//	String(const char *szStr);
//	operator const char*();
//
////	String& operator=(char *szStr);
////	String& operator=(const char *szStr);
////	String& operator=(String str);
//	String& operator=(const NullObject &ref);
//
//	size_t getLength();
//};

NAMESPACE_FRMWRK_END

#endif
