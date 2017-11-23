#ifndef __OBJECT_H
#define __OBJECT_H

#include "base/base_defs.h"
#include "base/Type.h"

NS_FW_BASE_BEGIN

#define DECL_TYPE(t) \
private: static const Type* classType_; \
public: virtual const Type* getType() { return t::classType(); } \
public: static const Type* classType() { return t::classType_; }

#define TYPE_OBJECT GET_TYPE(Object)


/*****************************************************************************
* Object
*****************************************************************************/
class String;
class Number;
class Object {
	friend class RunTime;
	DECL_TYPE(Object);
	//static const Type* classType_;
	static Object* nullInstance_;
	static void initialize();
	static void shutDown();
protected: PROP_R(long long, hash);
//protected: PROP_R(const Type*, type);

public:
	Object(void);
	virtual ~Object(void);

	//static const Type* classType();
	static Object* null();

	virtual int strcmp(const char*);
	virtual char* toString();
	virtual bool toBool();
	virtual Number* toNumber();
	virtual int compareTo(Object*);
	virtual void* valueOf();

	operator String*();

#ifdef _DEBUG
	static bool isDebugOn;
	void dump(const char* = 0);
#endif
	//*********************************************************
};

#define Null Object::null()

NS_FW_BASE_END

#endif