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
	//friend class String;

	PROP_R(protected, long long, Hash, i);
	int refCount_;

protected:
	int addRef_();
	int delRef_();

	Object_(void);
	virtual ~Object_(void);

	virtual String toString();
	virtual int compareTo(Object_ *obj);
};

/*****************************************************************************
* Object (reference)
*****************************************************************************/
class Null;
class MemoryMgr;
class Object {
	friend class MemoryMgr;
protected:
	Object_* ptr_;

	Object(void*);

	void addRef_();
	void delRef_();
	void null_();

public:
	Object(void);
	Object(Object&);
	virtual ~Object(void);

	Object& operator=(Object& ref);
	Object& operator=(const Null& ref);

#ifdef _DEBUG
	static void dump(Object&, const char*);
#endif

	String toString();
	int compareTo(Object& ref);
	bool operator==(Object& ref);
};

NAMESPACE_FRMWRK_END

#endif