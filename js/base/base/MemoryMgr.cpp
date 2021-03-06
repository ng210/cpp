#include <stdio.h>
#include <malloc.h>
#include "base/MemoryMgr.h"
#include "base/String.h"
#include "base/str.h"


NS_FW_BASE_USE

NS_FW_BASE_BEGIN

void* MemoryMgr::alloc(size_t size) {
	void *p = ::malloc(size);
	return p;
}
void* MemoryMgr::realloc(void* p, size_t size) {
	void *q = ::realloc(p, size);
	return q;
}

void MemoryMgr::free(void *p) {
	//DEBUG("free:%x\n", (int)p);
	::free(p);
}

//void* operator new(size_t iSize) {
//	return MemoryMgr::alloc(iSize);
//}

//void* operator new[](size_t iSize) {
//	return MemoryMgr::alloc(iSize);
//}

//void operator delete(void *p) {
//	MemoryMgr::free(p);
//}

#ifdef _DEBUG
#include "base/debug.h"

_MEM_DBG_INFO *MemoryMgr::memDbgInfos_ = NULL;
_MEM_DBG_INFO *MemoryMgr::nextFreeEntry_ = NULL;
int MemoryMgr::nMemDbgInfos_ = 0;

bool MemoryMgr::isDebugOn = false;

void MemoryMgr::initialize_() {
	int iSize = sizeof(_MEM_DBG_INFO) * _MEM_DBG_SIZE;
	_MEM_DBG_INFO* p = (_MEM_DBG_INFO*)malloc(iSize);
	memset(p, 0, iSize);
	int i = 0;
	for (; i<_MEM_DBG_SIZE - 1; i++) {
		p[i].ptr.next = &p[i + 1];
	}
	p[i].ptr.next = NULL;
	MemoryMgr::nextFreeEntry_ = p;
	MemoryMgr::memDbgInfos_ = p;
}

void* MemoryMgr::addMemDbgInfo(void *p, TCHAR *szFile, int iLine, void *old) {
	if (old != NULL) {
		bool found = false;
		for (int i = 0; i < MemoryMgr::nMemDbgInfos_; i++) {
			_MEM_DBG_INFO *mb = &MemoryMgr::memDbgInfos_[i];
			if (mb->ptr.address == old) {
				found = true;
				mb->iLine = iLine;
				mb->ptr.address = p;
				mb->szFile = szFile;
				break;
			}
		}
		if (found) {
			if (MemoryMgr::isDebugOn) {
				LOG(L"1>%s(%d): Realloc at 0x%lX=>0x%lX\n", szFile, iLine, old, p);
			}
		} else {
			LOG(L"1>%s(%d): Bad realloc at 0x%lX\n", szFile, iLine, old);
		}
	} else {
		if (MemoryMgr::nMemDbgInfos_ < _MEM_DBG_SIZE) {
			if (MemoryMgr::memDbgInfos_ == NULL) {
				MemoryMgr::initialize_();
			}
			_MEM_DBG_INFO& mb = *MemoryMgr::nextFreeEntry_;
			MemoryMgr::nextFreeEntry_ = mb.ptr.next;
			mb.iLine = iLine;
			mb.ptr.address = p;
			mb.szFile = szFile;
			MemoryMgr::nMemDbgInfos_++;
		} else {
			;
		}
		if (MemoryMgr::isDebugOn) {
			LOG(L"1>%s(%d): Alloc at 0x%lX\n", szFile, iLine, p);

		}
	}
	return p;
}

int MemoryMgr::delMemDbgInfo(void *ptr, TCHAR *szFile, int iLine) {
	int bFound = false;
	for (int i=0; i<MemoryMgr::nMemDbgInfos_; i++) {
		_MEM_DBG_INFO *mb = &MemoryMgr::memDbgInfos_[i];
		if (mb->szFile != NULL && mb->ptr.address == ptr) {
			bFound = true;
			mb->szFile = NULL;
			mb->ptr.next = MemoryMgr::nextFreeEntry_;
			MemoryMgr::nextFreeEntry_ = mb;
			break;
		}
	}
	if (bFound) {
		if (MemoryMgr::isDebugOn) {
			LOG(L"1>%s(%d): Free at 0x%lX\n", szFile, iLine, ptr);
		}
	} else {
		LOG(L"1>%s(%d): Attempt to free invalid block at 0x%lX\n", szFile, iLine, ptr);
	}
	return bFound;
}

void MemoryMgr::checkMemDbgInfo(size_t count, void** exceptions) {
	int found = 0;
	for (int i=0; i<MemoryMgr::nMemDbgInfos_; i++) {
		_MEM_DBG_INFO *mb = &MemoryMgr::memDbgInfos_[i];
		if (mb->szFile != NULL) {
			bool isException = false;
			for (size_t j = 0; j < count; j++) {
				if (exceptions[j] == mb->ptr.address) {
					isException = true;
					break;						
				}
			}
			if (!isException) {
				LOG(L"1>%s(%d): Memory leak at 0x%lX\n", mb->szFile, mb->iLine, mb->ptr.address);
				found++;
			}
		}
	}
	if (found > 0) {
		LOG(L"%d unallocated blocks found.\n", found);
	} else {
		LOG(L"No unallocated blocks found :)\n");
	}
}
//*********************************************************
#else
void MemoryMgr::initialize_() {
}
#endif

NS_FW_BASE_END