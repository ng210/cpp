#ifndef __BASEDEFS_H
#define __BASEDEFS_H

//#include <stddef.h>

#ifdef _DEBUG
#include "base/debug.h"
#endif

//*********************************************************
#ifndef NULL
#define NULL 0
#endif

#ifndef BYTE
#define BYTE unsigned char
#define byte BYTE
#endif
#ifndef WORD
#define WORD unsigned short
#define word WORD
#endif
#ifndef DWORD
#define DWORD unsigned long
#define dword DWORD
#endif

#define ALIGN4(a) ALIGN(a, 4)
#define ALIGN(a, n) (a+n-1) & ~(n-1)

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

//*********************************************************
#define NS_FW_BASE fmw
#define NS_FW_BASE_BEGIN namespace NS_FW_BASE {
#define NS_FW_BASE_END }
#define NS_FW_BASE_USE using namespace NS_FW_BASE;

//*********************************************************
// declare a field with inline getter and setter
// arguments: type, name
#define PROP(t, n) \
	public: t n##_; \
	public: inline t n() { return n##_; };\
			inline void n(t v) { n##_ = v; };

//*********************************************************
// declare a readonly field with getter only
#define PROP_R(t, n)\
	private: t n##_;\
	public: inline t n() { return n##_; };

#endif

//*********************************************************
