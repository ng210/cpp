#ifndef __OBJECT_H
#define __OBJECT_H

#include "base_defs.h"
//#include "Ref.h"
//#include "MemoryMgr.h"
#include <vadefs.h>
#include <stdarg.h>

#ifdef USE_TYPES
#include "Type.h"
#endif

NAMESPACE_FRMWRK_BEGIN

class Object;
class String;
class Object_ {
friend class Object;
#ifdef USE_TYPES
private:
	Type type_;
public:
	Type getType() { return m_type; };
#endif
	PROP(protected, int, RefCount, i);
	//PROP(protected, OBJREF*, ObjRef, p);
	PROP_R(protected, long long, Hash, i);
	//PROP_R(protected, char*, HashText, sz);
protected:
	Object_(void);
	inline int addRef() {
		return ++iRefCount_;
	}
	inline int delRef() {
		return --iRefCount_;
	}
	virtual ~Object_(void);
public:
	virtual char* toString();
	virtual String& toString_();
	virtual int compareTo(Object_*);
};

#define DEFINE_PROC0(t, m) void m() { ((t##_*)ptr_)->m(); }
#define DEFINE_PROC1(t, m, at1, an1) void m(at1 an1) { ((t##_*)ptr_)->m(an1); }
#define DEFINE_PROC2(t, m, at1, an1, at2, an2) void m(at1 an1, at2 an2) { ((t##_*)ptr_)->m(an1, an2); }
#define DEFINE_PROC3(t, m, at1, an1, at2, an2, at3, an3) void m(at1 an1, at2 an2, at3 an3) { ((t##_*)ptr_)->m(an1, an2, an3); }
#define DEFINE_PROC4(t, m, at1, an1, at2, an2, at3, an3, at4, an4) void m(at1 an1, at2 an2, at3 an3, at4 an4) { ((t##_*)ptr_)->m(an1, an2, an3, an4); }

#define DEFINE_FUNC0(t, m, r) r m() { return ((t##_*)ptr_)->m(); }
#define DEFINE_FUNC1(t, m, r, at1, an1) r m(at1 an1) { return ((t##_*)ptr_)->m(an1); }
#define DEFINE_FUNC2(t, m, r, at1, an1, at2, an2) r m(at1 an1, at2 an2) { return ((t##_*)ptr_)->m(an1, an2); }
#define DEFINE_FUNC3(t, m, r, at1, an1, at2, an2, at3, an3) r m(at1 an1, at2 an2, at3 an3) { return ((t##_*)ptr_)->m(an1, an2, an3); }
#define DEFINE_FUNC4(t, m, r, at1, an1, at2, an2, at3, an3, at4, an4) r m(at1 an1, at2 an2, at3 an3, at4 an4) { return ((t##_*)ptr_)->m(an1, an2, an3, an4); }

/*****************************************************************************/
class String;
//class NullObject;
class Object {
	//friend class NullObject;
	//friend class String;
protected:
	Object_ *ptr_;
	inline DEFINE_FUNC0(Object, addRef, int);
	//inline int addRef() {
	//	return ++(ptr_->iRefCount_);
	//}
	void delRef();
	inline Object_* getPtr() { return ptr_; }
public:
	Object(void);
	Object(Object&);
	virtual ~Object();

	Object& operator=(const Object &ref);

	operator const char*();
	DEFINE_FUNC0(Object, toString, char*);
	DEFINE_FUNC0(Object, toString_, String&);
	int compareTo(Object &ref) { return ptr_->compareTo(ref.ptr_); };
	bool operator==(Object &ref) { return (ptr_->compareTo(ref.ptr_) == 0); }
};

/*****************************************************************************/
class NullObject_ : public Object_ {
public:
	char* toString();
};

class NullObject : public Object {
	NullObject(void);
public:
	static const NullObject null_;
	DEFINE_FUNC0(NullObject, toString, char*);
};

/*****************************************************************************/
#define DEFINE_COMMON_OPERATORS(t) \
	t& operator=(const t &ref) { \
		if (ptr_ != ref.ptr_) { \
			delRef(); ptr_ = ref.ptr_; ((t##_*)ptr_)->addRef(); } \
		return *this; \
	}; \
	t& operator=(NullObject &ref) { \
		Object_ *p = ((t)ref).getPtr(); \
		if (ptr_ != p) { \
			delRef(); ptr_ = p; addRef(); \
		} \
		return *this; \
	};

NAMESPACE_FRMWRK_END

#define null *(NullObject*)&NAMESPACE_FRMWRK::NullObject::null_

#endif
