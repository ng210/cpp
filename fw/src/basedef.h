#ifndef __BASEDEF_H
#define __BASEDEF_H

#include <stddef.h>
#include "types.h"

#ifndef NULL
#define NULL 0
#endif

#define out

#define ALIGN4(a) ALIGN(a, 4)
#define ALIGN(a, n) (((size_t)a+n-1) & ~(n-1))

#ifndef offsetof
#define offsetof(t,f) ((size_t)&((t*)0)->f)
#endif

#ifndef arraysize
#ifdef ARRAYSIZE
#define arraysize ARRAYSIZE
#else
#define arraysize(a) (sizeof(a)/sizeof(a[0]))
#endif
#endif

#define STRINGIFY(str) #str
#define WIDEN2(x) L ## x
#define WIDEN(x) WIDEN2(x)
#define __WFILE__ WIDEN(__FILE__)
#define __WDATE__ WIDEN(__DATE__)
//*********************************************************
#define NS_FW_BASE fmw
#define NS_FW_BASE_BEGIN namespace NS_FW_BASE {
#define NS_FW_BASE_END }
#define NS_FW_BASE_USE using namespace NS_FW_BASE;

//*********************************************************
// declare a field with inline getter and setter
// arguments: type, name
#define PROP(t, n) \
			t n##_; \
	public: inline t n() { return n##_; };\
			inline void n(t v) { n##_ = v; };
	
//*********************************************************
// declare a readonly field with getter only
#define PROP_R(t, n)\
	public: inline t n() { return n##_; } \
		t n##_
	
// declare a field with a reference getter
#define PROP_REF(t, n)\
			t n##_; \
	public: inline t& n() { return n##_; }
#endif

#define TMPL(X, R, U) template <typename U> R X<U>
#define TMPL2(X, R, U, V) template <typename U, typename V> R X<U,V>

#ifdef UNICODE
#define __TFILE__ __WFILE__
#define __TDATE__ __WDATE__
#else
#define __TFILE__ __FILE__
#define __TDATE__ __DATE__
#endif

//*********************************************************
NS_FW_BASE_BEGIN

NS_FW_BASE_END