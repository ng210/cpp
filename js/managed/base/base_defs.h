#ifndef __BASEDEFS_H
#define __BASEDEFS_H

#include <stddef.h>

#ifdef _DEBUG
#include "base/debug.h"
#endif

//*********************************************************
#ifndef NULL
#define NULL ((void*)0)
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

#define ALIGN4(n) ((n & 0x03) ? ((n>>2)+1)<<2 : n)

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

//*********************************************************
#define NAMESPACE_FRMWRK fmw
#define NAMESPACE_FRMWRK_BEGIN namespace NAMESPACE_FRMWRK {
#define NAMESPACE_FRMWRK_END }
#define NAMESPACE_FRMWRK_USE using namespace NAMESPACE_FRMWRK;

//*********************************************************
// declare a field with inline getter and setter
// arguments: scope, type, name, prefix
#define PROP(s, t, n, p) \
	s: t p##n##_; \
	public: inline t n() { return p##n##_; };\
			inline void n(t v) { p##n##_ = v; };

//*********************************************************
// declare a readonly field with getter only
#define PROP_R(s, t, n, p)\
	s: t p##n##_;\
	public: inline t n() { return p##n##_; };

#endif

//*********************************************************
