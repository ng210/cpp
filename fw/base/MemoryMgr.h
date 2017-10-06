#ifndef __MEMORYMGR_H
#define __MEMORYMGR_H

#include "base_defs.h"
#include "Object.h"

NAMESPACE_FRMWRK_BEGIN

#ifdef _DEBUG
	typedef struct _MEM_DBG_INFO_ {
		union {
			_MEM_DBG_INFO_ *next;
			void *address;
		} ptr;
		char *szFile;
		int iLine;
		void *padding;
	} _MEM_DBG_INFO;

	#define _MEM_DBG_SIZE_BITS 12	// 512 entries
	#define _MEM_DBG_SIZE (1<<_MEM_DBG_SIZE_BITS)
	#define _MEM_DBG_SIZE_MASK (_MEM_DBG_SIZE-1)
	//static _MEM_DBG_ENTRY *_MEM_DBG_head = NULL;
	//static _MEM_DBG_ENTRY *_MEM_DBG_tail = NULL;

	//#define DBGTRAIL _debug("\t%s\t%d\n", __FILE__, __LINE__)
	//#define NEW(T,v,...) { T *p = new T(__VA_ARGS__); v = *p; MemoryMgr::addMemDbgInfo(p, __FILE__, __LINE__); }
	//#define NEW_(T,p,...) p = new T(__VA_ARGS__); MemoryMgr::addMemDbgInfo(p, __FILE__, __LINE__);
	//#define NEWARR(T,v,s) v = new T[s]; MemoryMgr::addMemDbgInfo(v, __FILE__, __LINE__);	
	#define NEW(T,...) *(T*)MemoryMgr::addMemDbgInfo(new T(__VA_ARGS__), __FILE__, __LINE__)
	#define NEW_(T,...) (T*)MemoryMgr::addMemDbgInfo(new T(__VA_ARGS__), __FILE__, __LINE__)
	#define NEWARR(T,s) (T*)MemoryMgr::addMemDbgInfo(new T[s], __FILE__, __LINE__)
	#define DEL(v) if (MemoryMgr::delMemDbgInfo((Object&)v, __FILE__, __LINE__)) { delete v; }
	#define DEL_(v) if (v != NULL && MemoryMgr::delMemDbgInfo((void*)v, __FILE__, __LINE__)) { delete v; }


	//#define NEW_(T, v, ...) v = new T(__VA_ARGS__); MemoryMgr::_MEM_DBG_alloc(v, __FILE__, __LINE__);
	//#define NEW(T, v, ...) T v = *new T(__VA_ARGS__); ((Object)v).delRef(); MemoryMgr::_MEM_DBG_alloc((Ref<Object_>*)&v, __FILE__, __LINE__);
	//#define NEWBUF(v, c) v = new char[c]; _MEM_DBG_alloc(v, __FILE__, __LINE__)
	//#define NEWARR(T, v, s) T* v = new T[s]; MemoryMgr::_MEM_DBG_alloc(v, __FILE__, __LINE__);
	//#define DEL(v) if (MemoryMgr::_MEM_DBG_free((Ref<Object_>*)&(v), __FILE__, __LINE__)) { delete v; }

#else
	#define NEW_(T,v,...) v = new T(__VA_ARGS__);
	#define NEWARR(T,v,s) v = new T[s];
	#define DEL(v) delete v;
	//#define NEW(T, v, ...) T v = *new T(__VA_ARGS__)
	//#define NEWBUF(v, c) v = new char[c]
	//#define NEWTBUF(v, t, c) v = (t*)new char[c]
	//#define NEWARR(v, t, c) v = new t[c]
	//#define DEL(v) if ((v) != NULL) { delete (v); (v) = NULL; }
#endif

class MemoryMgr {
#ifdef _DEBUG
	static _MEM_DBG_INFO *memDbgInfos_;
	static int nMemDbgInfos_;
	static _MEM_DBG_INFO *refTable_;
	static _MEM_DBG_INFO *nextFreeEntry_;
	static void initialize_();
public:
	static bool isDebugOn;
	static void* addMemDbgInfo(void *p, char *szFile, int iLine);
	static int delMemDbgInfo(Object& obj, char *szFile, int iLine);
	static int delMemDbgInfo(void *p, char *szFile, int iLine);
	static void checkMemDbgInfo();
#endif
public:
	static void* alloc(size_t size);
	static void free(void *p);
};

NAMESPACE_FRMWRK_END

void* operator new(size_t iSize);
//void* operator new[](size_t iSize);
//void* operator new(size_t iSize, void* p);

void operator delete(void *p);
//void operator delete[](void *p);

#endif
