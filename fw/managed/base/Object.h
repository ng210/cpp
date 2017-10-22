#ifndef __OBJECT_H
#define __OBJECT_H

#include "base/base_defs.h"

NAMESPACE_FRMWRK_BEGIN



/*****************************************************************************
* Object_ (structure)
*****************************************************************************/
class String; 
class Object;
class Object_ {
	friend class Object;

	PROP_R(protected, long long, Hash, i);
	int refCount_;

protected:
	int addRef_();
	int delRef_();

	Object_(void);
	virtual ~Object_(void);

	virtual const char* getType();
	virtual String toString();
	virtual int compareTo(Object_ *obj);
	virtual void* valueOf();

#ifdef _DEBUG
	void dump(const char* = NULL);
#endif
};

/*****************************************************************************
* Object (reference)
*****************************************************************************/
class Null;
class MemoryMgr;
class Array_;
class Object {
	//friend class Null;
	friend class MemoryMgr;
	friend class Array_;
protected:
	Object_* ptr_;

	Object(void*);

	void addRef_();
	void delRef_();
	void null_();

public:
	Object(void);
	Object(Object&);
	Object(const Null&);
	virtual ~Object(void);

	Object& operator=(Object& ref);
	Object& operator=(const Null& ref);
	String getType();

#ifdef _DEBUG
	void dump(const char* = NULL);
	static bool isDebugOn;
#endif

	String toString();
	int compareTo(Object& ref);
	bool operator==(Object& ref);
	void* valueOf();
};

//*********************************************************

typedef Object Function(size_t, ...);


NAMESPACE_FRMWRK_END

#endif