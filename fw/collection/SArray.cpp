//*****************************************************************************
// SArray - static array
//*****************************************************************************

#include "Collection/SArray.h"
#include "Base/MemoryMgr.h"

NAMESPACE_FRMWRK_USE

//*********************************************************
SArray_::SArray_() {
	iCapacity_ = 0;
	data_ = NULL;
}
//*********************************************************
SArray_::SArray_(size_t iCount) {
	NEWARR(Object, data_, iCount);
	iCapacity_ = iCount;
}
//*********************************************************
SArray_::~SArray_(void) {
	//_VS_debug("SArray::~SArray\n");
}
//*********************************************************
void SArray_::add(Object &obj) {
	if (iCapacity_ > iCount_) {
		data_[iCount_] = obj;
		iCount_++;
	}
	else {
		// signal error
		;
	}
}
//*********************************************************
void SArray_::remove(Object &obj) {
	Iterator it = begin(0);
	for (size_t i=0; i<iCount_; i++) {
		if (data_[i].compareTo(obj) == 0) {
			removeAt(i);
			break;
		}
	}
}
//*********************************************************
// insert at index
void SArray_::insertAt(size_t ix, Object &obj) {
	if (iCount_ < iCapacity_) {
		// shift items down
		for (size_t i = iCount_; i>ix; i--) {
			data_[i+1] = data_[i];
		}
		data_[ix] = obj;
	}
}
//*********************************************************
// get item at index
Object& SArray_::getAt(size_t ix) {
	return (ix <= iCount_) ? data_[ix] : null;
}
//*********************************************************
// set item at index
void SArray_::setAt(size_t ix, Object& obj) {
	if (ix <= iCount_) {
		data_[ix] = obj;
	}
}
//*********************************************************
// remove item at index
void SArray_::removeAt(size_t ix) {
	if (ix <= iCount_) {
		// shift items up
		for (size_t i = ix; i<iCount_; i++) {
			data_[i] = data_[i + 1];
		}
		data_[iCount_] = null;
		iCount_--;
	}
}
//*********************************************************
// Begin iteration
Iterator& SArray_::begin(size_t iStart) {
	Iterator *it = new Iterator(iStart);
	return *it;
}
//*********************************************************
// Next iteration step
int SArray_::next(Iterator &it) {
	int ret = 0;
	size_t ix = it.getIndex();
	if (ix < iCount_) {
		it.set(data_[ix], ix);
		it.setIndex(++ix);
		ret = 1;
	}
	return ret;
}
//*********************************************************
// Previous iteration step
int SArray_::prev(Iterator &it) {
	int ret = 0;
	size_t ix = it.getIndex();
	if (ix > 0) {
		ix--;
		it.set(data_[ix], ix);
		it.setIndex(ix);
		ret = 1;
	}
	return ret;
}
//*********************************************************
// Seek to position
Iterator& SArray_::seek(size_t pos) {
	Iterator& it = (Iterator&)null;
	NEW(Iterator, it, pos);
	return it;
}
//*********************************************************
Object& SArray_::first(Iterator &it) {
	Object& ret = null;
	if (iCount_ > 0) {
		it.set(data_[0], 0);
	}
	return ret;
}
//*********************************************************
Object& SArray_::last(Iterator &it) {
	Object& ret = null;
	if (iCount_ > 0) {
		size_t ix = iCount_ - 1;
		it.set(data_[ix], ix);
	}
	return ret;
}

//*****************************************************************************
// Static Array ref
//*****************************************************************************
SArray::SArray(size_t iCount) {
	NEW_(SArray_, ptr_, iCount);
}

////*********************************************************
//// Collection implementation
////*********************************************************
//void SArray::init(size_t iItemSize, size_t iCount, int bRef) {
//	// set reference flag
//	Collection::init(bRef);
//	// initial size is 0
//	m_iCount = 0;
//	// item size is 4 bytes aligned
//	m_iItemSize = ALIGN4(iItemSize);
//	// calculate total capacity in bytes
//	m_iCapacity = iCount * m_iItemSize;
//	// allocate memory for array
//	NEWBUF(m_pData, m_iCapacity);
//}
////*********************************************************
//void SArray::destroy() {
//	empty();
//	// free memory
//	DEL(m_pData);
//}
////*********************************************************
//void SArray::empty() {
//	// reset count, size
//	m_iCount = 0;
//	m_iCount = 0;
//}
////*********************************************************
//COLLECTIONITEM SArray::add() {
//	COLLECTIONITEM pItem;
//	pItem.p = NULL;
//	// capacity limit reached?
//	if (m_iCount != m_iCapacity) {
//		// not yet, allocate space for item
//		pItem.p = (void*)(m_pData + m_iCount);
//		m_iCount += m_iItemSize;
//		m_iCount++;
//	}
//	return pItem;
//}
////*********************************************************
//void SArray::remove(void *p) {
//	size_t ix = 0;
//	COLLECTIONITEM item;
//	item.p = m_pData;
//	if (m_bRef) {
//		for (;ix<m_iCount; ix++) {
//			if (item.ref[ix] == p) {
//				break;
//			}
//			//item.ref++;
//		}
//	} else {
//		for (;ix<m_iCount; ix++) {
//			if (item.p == p) {
//				break;
//			}
//			item.i += m_iItemSize;
//		}
//	}
//	return removeAt(ix);
//}
////*********************************************************
//// Enumerable implementation
////*********************************************************
///*
//void SArray::clear()
//{
//	if (m_iCount > 0)
//	{
//		Collection::clear();
//	}
//	empty();
//}
//*/
////*********************************************************
////void* SArray::apply(ACTION* pAction, void* pArgs, int& ix)
////{
////	void *pRes = NULL;
////	void *p = m_pData;
////	for (ix=0; (size_t)ix<m_iCount; ix++)
////	{
////		void *pItem = (m_bRef) ? *(void**)p : p;
////		//void *p_ = &((void**)m_pData)[ix];
////		if (!(*pAction)(this, pItem, ix, pArgs))
////		{
////			pRes = pItem;
////			break;
////		}
////		*(int*)&p += m_iItemSize;
////	}
////	return pRes;
////}
////*********************************************************
//Iterator* SArray::begin(size_t iStart) {
//	Iterator *it;
//	NEW(it, Iterator, (void**)m_pData, iStart);
//	iStart--;
//	it->ppItem.i += iStart * m_iItemSize;
//	return it;
//}
////*********************************************************
//int SArray::next(Iterator *it) {
//	int iRet = (size_t)(++it->iItem) < m_iCount;
//	if (iRet) {
//		it->ppItem.i += m_iItemSize;
//		it->pItem = (m_bRef) ? *it->ppItem.ref : it->ppItem.p;
//	}
//	return iRet;
//}
////*********************************************************
//int SArray::prev(Iterator *it) {
//	int iRet = (size_t)(--it->iItem) >= 0;
//	if (iRet) {
//		it->ppItem.i -= m_iItemSize;
//		it->pItem = (m_bRef) ? *it->ppItem.ref : it->ppItem.p;
//	}
//	return iRet;
//}
////*********************************************************
//COLLECTIONITEM SArray::seek(Iterator *it, long pos) {
//	COLLECTIONITEM pItem = getSlotAt(pos);
//	if (pItem.p != NULL) {
//		it->ppItem.p = m_pData;
//		it->ppItem.i += pos * m_iItemSize;
//		it->iItem = pos;
//		it->pItem = (m_bRef) ? *it->ppItem.ref : it->ppItem.p;
//	}
//	return pItem;
//}
////*********************************************************
//// IndexedCollection implementation
////*********************************************************
//COLLECTIONITEM SArray::insertAt(int ix) {
//	// add one element
//	COLLECTIONITEM pItem = add();
//	// add successful
//	if (pItem.p != NULL) {
//		// move elements up
//		COLLECTIONITEM pSlot = getSlotAt(ix);
//		// slot of requested item
//		pItem = pSlot;
//		if (pItem.p != NULL) {
//			// points to the last but one dword
//			int *src = (int*)&m_pData[ix*m_iItemSize];
//			int *dst = (int*)((int)src + m_iItemSize);
//			// number of dwords to copy
//			int i = (m_iCount - 1 - ix)*m_iItemSize>>2;
//			while (i > 0) {
//				i--;
//				dst[i] = src[i];
//			}
//		}
//	}
//	return pItem;
//}
////*********************************************************
//void* SArray::getAt(int ix) {
//	void *p = NULL;
//	if ((size_t)ix < m_iCount) {
//		if (m_bRef) {
//			p = ((void**)m_pData)[ix];
//		} else {
//			p = (void*)&m_pData[ix*m_iItemSize];
//		}
//	}
//	return p;
//}
////*********************************************************
//void SArray::setAt(int ix, void *pItem) {
//	if ((size_t)ix < m_iCount) {
//		if (m_bRef) {
//			((void**)m_pData)[ix] = pItem;
//		} else {
//			memcpy(&m_pData[ix*m_iItemSize], pItem, m_iItemSize);
//		}
//	}
//}//*********************************************************
//#pragma warning (push)
//#pragma warning (disable:4100)
//void SArray::setAt(int ix, void *pItem, size_t iItemSize) {
//	setAt(ix, pItem);
//}
//#pragma warning (pop)
////*********************************************************
//COLLECTIONITEM SArray::getSlotAt(int ix) {
//	COLLECTIONITEM pp;
//	pp.p = NULL;
//	if ((size_t)ix < m_iCount) {
//		if (m_bRef) {
//			pp.p = &((void**)m_pData)[ix];
//		} else {
//			pp.ref = (void**)&m_pData[ix*m_iItemSize];
//		}
//	}
//	return pp;
//}
////*********************************************************
//void SArray::removeAt(int ix) {
//	if ((size_t)ix < m_iCount) {
//		void **ppRemove;
//		if (m_bRef) {
//			ppRemove = &((void**)m_pData)[ix];
//			//pRemove = *ppRemove;
//		} else {
//			ppRemove = (void**)&m_pData[ix*m_iItemSize];
//			// create copy of item
//			//NEWBUF(pRemove, m_iItemSize);
//			//memcpy(pRemove, ppRemove, m_iItemSize);
//		}
//		// move elements down
//		int iLength = (int)m_pData + m_iCount - (int)ppRemove - m_iItemSize;
//		memcpy(ppRemove, &((char*)ppRemove)[m_iItemSize], iLength);
//		m_iCount -= m_iItemSize;
//		m_iCount--;
//	}
//}
