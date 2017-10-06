#include "MemoryMgr.h"
#include <stdio.h>
#include <string.h>
#include <malloc.h>

NAMESPACE_FRMWRK_USE

#ifdef _DEBUG
_MEM_DBG_INFO *MemoryMgr::memDbgInfos_ = NULL;
_MEM_DBG_INFO *MemoryMgr::nextFreeEntry_ = NULL;
int MemoryMgr::nMemDbgInfos_ = 0;
//bool MemoryMgr::_MEM_DBG = false;
#endif

bool MemoryMgr::isDebugOn = false;

void MemoryMgr::initialize_() {
	int iSize = sizeof(_MEM_DBG_INFO) * _MEM_DBG_SIZE;
	_MEM_DBG_INFO* p = (_MEM_DBG_INFO*)malloc(iSize);
	memset(p, 0, iSize);
	int i = 0;
	for (; i<_MEM_DBG_SIZE-1; i++) {
		p[i].ptr.next = &p[i+1];
	}
	p[i].ptr.next = NULL;
	MemoryMgr::nextFreeEntry_ = p;
	MemoryMgr::memDbgInfos_ = p;
}

void* MemoryMgr::alloc(size_t size) {
	//void *p = NULL;
	//// insert pointer into table
	//if (nRefTable_ < MAXOBJREFCOUNT) {
	//	// allocate memory for object + refTable pointer
		void *p = malloc(size);
	//	// get next free entry
	//	OBJREF *r = MemoryMgr::nextFreeEntry_;
	//	// set next free
	//	MemoryMgr::nextFreeEntry_ = r->ptr.entry;
	//	r->ptr.ref = (Object*)p;
	//	nRefTable_++;
	//}
	//_VS_debug("alloc:%x\n", (int)p);
	return p;
}

void MemoryMgr::free(void *p) {
	//_VS_debug("free:%x\n", (int)p);
	::free(p);
}

void* operator new(size_t iSize) {
	return MemoryMgr::alloc(iSize);
}

//void* operator new[](size_t iSize) {
//	return MemoryMgr::alloc(iSize);
//}

void operator delete(void *p) {
	MemoryMgr::free(p);
}

// TODO: check this!
//void operator delete[](void *p) {
//	MemoryMgr::free(p);
//}

#ifdef _DEBUG
void* MemoryMgr::addMemDbgInfo(void *p, char *szFile, int iLine) {
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
	}
	if (MemoryMgr::isDebugOn) {
		_VS_debug("1>%s(%d) : Alloc at 0x%08X\n", szFile, iLine, p);
	}
		return p;
}

int MemoryMgr::delMemDbgInfo(Object& obj, char *szFile, int iLine) {
	return delMemDbgInfo(obj.ptr_, szFile, iLine);
}



int MemoryMgr::delMemDbgInfo(void *ptr, char *szFile, int iLine) {
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
			_VS_debug("1>%s(%d) : Free at 0x%08X\n", szFile, iLine, ptr);
		}
	} else {
		_VS_debug("1>%s(%d) : Attempt to free invalid block at 0x%08X\n", szFile, iLine, ptr);
	}
	return bFound;
}

void MemoryMgr::checkMemDbgInfo() {
	int iFound = 0;
	for (int i=0; i<MemoryMgr::nMemDbgInfos_; i++) {
		_MEM_DBG_INFO *mb = &MemoryMgr::memDbgInfos_[i];
		if (mb->szFile != NULL) {
			_VS_debug("1>%s(%d) : Memory leak at 0x%08X\n", mb->szFile, mb->iLine, mb->ptr.address);
			iFound++;
		}
	}
	if (iFound > 0) {
		_VS_debug("%d unallocated blocks found.\n", iFound);
	} else {
		_VS_debug("No unallocated blocks found :)\n");
	}
}
//*********************************************************
#endif
