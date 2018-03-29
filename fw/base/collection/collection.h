#ifndef __COLLECTION_H
#define __COLLECTION_H

#include "basedef.h"

NS_FW_BASE_BEGIN
class Collection;
typedef int (CollectionCallback)(void*, unsigned int, Collection*, void*);

//*****************************************************************************
// Base class collection
//*****************************************************************************
class Collection {
protected:
	UINT32 length_;
public:
	virtual void* add(void* item = NULL) = 0;
	virtual void* insertAt(int ix, void* item = NULL) = 0;
	virtual void removeAt(int ix) = 0;
	virtual void* getAt(int ix) = 0;
	virtual void* setAt(int ix, void* item) = 0;
	virtual int findIndex(void* key, CollectionCallback* compare = NULL) = 0;

	inline UINT32 length() { return length_;  }
};

//template<typename T> class Collection {
//	PROP_R(size_t, length);
//public:
//	virtual T add(T item = NULL) = 0;
//	virtual T insertAt(int ix, T item = NULL) = 0;
//	virtual T removeAt(int ix) = 0;
//	virtual T getAt(int ix) = 0;
//	virtual T setAt(int ix, void* item) = 0;
//	virtual int findIndex(Compare* compare) = 0;
//};

NS_FW_BASE_END
#endif