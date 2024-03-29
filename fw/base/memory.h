#ifndef __MEMORY_H
#define __MEMORY_H

#include "basedef.h"

NS_FW_BASE_BEGIN

#ifdef _DEBUG
	typedef struct _MEM_DBG_INFO_ {
		union {
			_MEM_DBG_INFO_ *next;
			void *address;
		} ptr;
		TCHAR *szFile;
		int iLine;
		void *padding;
	} _MEM_DBG_INFO;

	#define _MEM_DBG_SIZE_BITS 20	// 1M entries
	#define _MEM_DBG_SIZE (1<<_MEM_DBG_SIZE_BITS)
	#define _MEM_DBG_SIZE_MASK (_MEM_DBG_SIZE-1)

	//#define NEW(T,...) *(T*)MemoryMgr::addMemDbgInfo(new T(__VA_ARGS__), __FILE__, __LINE__)
	#define NEW_(T,...) (T*)MemoryMgr::addMemDbgInfo(new T(__VA_ARGS__), __FILE__, __LINE__)
	#define NEWARR(T,s) (T*)MemoryMgr::addMemDbgInfo(new T[s], __FILE__, __LINE__)
	//#define DEL(v) if (MemoryMgr::delMemDbgInfo((Object&)v, __FILE__, __LINE__)) { delete v; }
	#define DELARR(v) if (MemoryMgr::delMemDbgInfo(v, __FILE__, __LINE__)) { delete[] v; }
	#define DEL_(v) if (v != NULL && MemoryMgr::delMemDbgInfo((void*)v, __FILE__, __LINE__)) { delete v; }
	#define MALLOC(t, s) (t*)MemoryMgr::addMemDbgInfo(MemoryMgr::alloc(sizeof(t)*(size_t)(s)), __FILE__, __LINE__)
	#define REALLOC(p, t, s) (t*)MemoryMgr::addMemDbgInfo(MemoryMgr::realloc(p, sizeof(t)*(s)), __FILE__, __LINE__, p)
	#define FREE(p) if (p != NULL && MemoryMgr::delMemDbgInfo((void*)p, __FILE__, __LINE__)) { MemoryMgr::free((void*)p); }

#else
	#define NEW_(T,...) new T(__VA_ARGS__)
	#define NEWARR(T,s) new T[s]
	#define DELARR(v) delete[] v
	#define DEL_(v) delete v
	#define MALLOC(t, s) (t*)MemoryMgr::alloc(sizeof(t)*s)
	#define REALLOC(p, t, s) (t*)MemoryMgr::realloc(p, sizeof(t)*s)
	#define FREE(p) MemoryMgr::free(p)
#endif

	#define READ(p, t) *(t*)p; p += sizeof(t)
	#define WRITE(p, v, t) *(t*)p = v; p += sizeof(t)

class MemoryMgr {
	static void initialize_();
#ifdef _DEBUG
	static _MEM_DBG_INFO *memDbgInfos_;
	static int nMemDbgInfos_;
	static _MEM_DBG_INFO *refTable_;
	static _MEM_DBG_INFO *nextFreeEntry_;
public:
	static bool isDebugOn;
	static void* addMemDbgInfo(void *p, const char *szFile, int iLine, void *o = NULL);
	static int delMemDbgInfo(void *p, const char *szFile, int iLine);
	static void checkMemDbgInfo(size_t, void**);
#endif
public:
	static void resetMemDbgInfo();

	static void* alloc(size_t size);
	static void* realloc(void* p, size_t size);
	static void free(void *p);
};

NS_FW_BASE_END

void* operator new(size_t s, void* ptr);
void operator delete(void* ptr, void* unused);

#endif
