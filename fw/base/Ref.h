#ifndef __REF_H
#define __REF_H

#include "base_defs.h"
#include "base/MemoryMgr.h"

NAMESPACE_FRMWRK_BEGIN

class Ref_ {
protected:
	PROP(protected, int, RefCount, i);
	Ref_(void);
	int addRef();
	int delRef();
	virtual ~Ref_(void);
};

#define DEFINE_PROC0(t, m) void t##:m() { ((t##_*)ptr_)->m(); }
#define DEFINE_PROC1(t, m, at1, an1) t##:void m(at1 an1) { ((t##_*)ptr_)->m(an1); }
#define DEFINE_PROC2(t, m, at1, an1, at2, an2) t##:void m(at1 an1, at2 an2) { ((t##_*)ptr_)->m(an1, an2); }
#define DEFINE_PROC3(t, m, at1, an1, at2, an2, at3, an3) t##:void m(at1 an1, at2 an2, at3 an3) { ((t##_*)ptr_)->m(an1, an2, an3); }
#define DEFINE_PROC4(t, m, at1, an1, at2, an2, at3, an3, at4, an4) t##:void m(at1 an1, at2 an2, at3 an3, at4 an4) { ((t##_*)ptr_)->m(an1, an2, an3, an4); }

#define DEFINE_FUNC0(t, m, r) r t##:m() { return ((t##_*)ptr_)->m(); }
#define DEFINE_FUNC1(t, m, r, at1, an1) r t##:m(at1 an1) { return ((t##_*)ptr_)->m(an1); }
#define DEFINE_FUNC2(t, m, r, at1, an1, at2, an2) r t##:m(at1 an1, at2 an2) { return ((t##_*)ptr_)->m(an1, an2); }
#define DEFINE_FUNC3(t, m, r, at1, an1, at2, an2, at3, an3) r t##:m(at1 an1, at2 an2, at3 an3) { return ((t##_*)ptr_)->m(an1, an2, an3); }
#define DEFINE_FUNC4(t, m, r, at1, an1, at2, an2, at3, an3, at4, an4) r t##:m(at1 an1, at2 an2, at3 an3, at4 an4) { return ((t##_*)ptr_)->m(an1, an2, an3, an4); }

template <class T> class Ref {
protected:
	T* ptr_;	// HINT: T is a subclass of Ref_
	Ref<T>(const Ref<T>& ref) {
		//delRef();
		ptr_ = ref.ptr_;
		ref.addRef();
	}
	Ref<T>(T* obj) {
		//delRef();
		ptr_ = obj;
		obj->addRef();
	}
	void addRef() {
		ptr_->addRef();
	}
	void delRef() {
		if (ptr_->delRef() == 0) {
			DEL_(ptr_);
		}
	}
public:
	Ref<T>& operator=(const Ref<T> &ref) {
		delRef();
		ptr_ = ref.ptr_;
		ptr_->addRef();
		return *this;
	}
	Ref<T>() {
		//NEW_(T, ptr_);
		//((Ref_*)ptr_)->addRef();
	}
	virtual ~Ref<T>() {
		delRef();
	}
};

NAMESPACE_FRMWRK_END

#endif
