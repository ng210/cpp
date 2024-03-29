#include <stdio.h>
#include <malloc.h>
#include "base/memory.h"
//#include "base/String.h"
#include "base/str.h"
#include "base/debug.h"

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
	::free(p);
}

#ifdef _DEBUG
#include "base/debug.h"

_MEM_DBG_INFO *MemoryMgr::memDbgInfos_ = NULL;
_MEM_DBG_INFO *MemoryMgr::nextFreeEntry_ = NULL;
int MemoryMgr::nMemDbgInfos_ = 0;

bool MemoryMgr::isDebugOn = false;

void MemoryMgr::initialize_() {
	if (MemoryMgr::memDbgInfos_ != NULL) {
		free(MemoryMgr::memDbgInfos_);
	}

	size_t iSize = sizeof(_MEM_DBG_INFO) * _MEM_DBG_SIZE;
	_MEM_DBG_INFO* p = (_MEM_DBG_INFO*)malloc(iSize);
	memset(p, 0, iSize);
	int i = 0;
	for (; i<_MEM_DBG_SIZE - 1; i++) {
		p[i].ptr.next = &p[i + 1];
	}
	p[i].ptr.next = NULL;
	MemoryMgr::nextFreeEntry_ = p;
	MemoryMgr::memDbgInfos_ = p;
	MemoryMgr::nMemDbgInfos_ = 0;
}

void* MemoryMgr::addMemDbgInfo(void *p, const char *szFile, int iLine, void *old) {
	if (old != NULL) {
		bool found = false;
		for (int i = 0; i < MemoryMgr::nMemDbgInfos_; i++) {
			_MEM_DBG_INFO *mb = &MemoryMgr::memDbgInfos_[i];
			if (mb->ptr.address == old) {
				found = true;
				mb->iLine = iLine;
				mb->ptr.address = p;
				mb->szFile = (char*)szFile;
				break;
			}
		}
		if (found) {
			if (MemoryMgr::isDebugOn) {
				LOG("1>%s(%d): Realloc at 0x%llX=>0x%llX\n", szFile, iLine, old, p);
			}
		} else {
			LOG("1>%s(%d): Bad realloc at 0x%llX\n", szFile, iLine, old);
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
			mb.szFile = (char*)szFile;
			MemoryMgr::nMemDbgInfos_++;
		} else {
			printf("Debug Info table full");
		}
		if (MemoryMgr::isDebugOn) {
			LOG("1>%s(%d): Alloc at 0x%llX\n", szFile, iLine, p);

		}
	}
	return p;
}

int MemoryMgr::delMemDbgInfo(void *ptr, const char *szFile, int iLine) {
	int bFound = false;
	_MEM_DBG_INFO *mb = NULL;
	for (int i=0; i<MemoryMgr::nMemDbgInfos_; i++) {
		mb = &MemoryMgr::memDbgInfos_[i];
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
			LOG("1>%s(%d): Free at 0x%llX\n", szFile, iLine, ptr);
		}
	} else {
		LOG("1>%s(%d): Attempt to free invalid block at 0x%llX\n", szFile, iLine, ptr);
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
				LOG("1>%s(%d): Memory leak at 0x%llX\n", mb->szFile, mb->iLine, mb->ptr.address);
				found++;
			}
		}
	}
	if (MemoryMgr::isDebugOn) {
		if (found > 0) {
			LOG("%d unallocated blocks found.\n", found);
		} else {
			LOG("No unallocated blocks found :)\n");
		}
	}
}

//*********************************************************
#else
void MemoryMgr::initialize_() {
}
#endif

void MemoryMgr::resetMemDbgInfo() {
	MemoryMgr::initialize_();
}

NS_FW_BASE_END

void* operator new(size_t s, void* ptr) { return ptr; }
void operator delete(void* ptr, void* unused) { return; }
