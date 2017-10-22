#ifndef __MEMORYMGR_H
#define __MEMORYMGR_H

#include "base/Object.h"

NS_FW_BASE_BEGIN

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

	#define NEW(T,...) *(T*)MemoryMgr::addMemDbgInfo(new T(__VA_ARGS__), __FILE__, __LINE__)
	#define NEW_(T,...) (T*)MemoryMgr::addMemDbgInfo(new T(__VA_ARGS__), __FILE__, __LINE__)
	#define NEWARR(T,s) (T*)MemoryMgr::addMemDbgInfo(new T[s], __FILE__, __LINE__)
	#define DEL(v) if (MemoryMgr::delMemDbgInfo((Object&)v, __FILE__, __LINE__)) { delete v; }
	#define DELARR(v) if (MemoryMgr::delMemDbgInfo(v, __FILE__, __LINE__)) { delete[] v; }
	#define DEL_(v) if (v != NULL && MemoryMgr::delMemDbgInfo((void*)v, __FILE__, __LINE__)) { delete v; }
	#define MALLOC(t, s) (t*)MemoryMgr::addMemDbgInfo(MemoryMgr::alloc(sizeof(t)*s), __FILE__, __LINE__)
	#define REALLOC(p, t, s) (t*)MemoryMgr::addMemDbgInfo(MemoryMgr::realloc(p, sizeof(t)*s), __FILE__, __LINE__, true)
	#define FREE(p) if (p != NULL && MemoryMgr::delMemDbgInfo((void*)p, __FILE__, __LINE__)) { MemoryMgr::free((void*)p); }

#else
	#define NEW_(T,v,...) v = new T(__VA_ARGS__);
	#define NEWARR(T,v,s) v = new T[s];
	#define DEL(v) delete v;
	#define MALLOC(t, s) (t*)MemoryMgr::alloc(sizeof(t)*s)
	#define REALLOC(p, t, s) (t*)MemoryMgr::realloc(p, sizeof(t)*s)
	#define FREE(p) MemoryMgr::free(p)
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
	static void* addMemDbgInfo(void *p, char *szFile, int iLine, bool isRealloc = false);
	static int delMemDbgInfo(void *p, char *szFile, int iLine);
	static void checkMemDbgInfo();
#endif
public:
	static void* alloc(size_t size);
	static void* realloc(void* p, size_t size);
	static void free(void *p);
};

NS_FW_BASE_END

void* operator new(size_t iSize);
//void* operator new[](size_t iSize);
//void* operator new(size_t iSize, void* p);

void operator delete(void *p);
//void operator delete[](void *p);

#endif
