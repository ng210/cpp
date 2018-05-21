#ifndef __COLLECTION_H
#define __COLLECTION_H

#include "basedef.h"
#include "types.h"
#include "base/str.h"

NS_FW_BASE_BEGIN

//*****************************************************************************
// Base class collection
//*****************************************************************************
class Collection {
protected: PROP_R(UINT32, length);
protected:
public:
	virtual void* add(void* item = NULL) = 0;
	virtual void* insertAt(int ix, void* item = NULL) = 0;
	virtual void removeAt(int ix) = 0;
	virtual void* getAt(int ix) = 0;
	virtual void setAt(int ix, void* item) = 0;
	//virtual int findIndex(void* key, CollectionCallback* compare = NULL) = 0;

	//inline UINT32 length() { return length_;  }

	static int compareByRef(void* item, UINT32 ix, Collection* collection, void* key) {
		return (int)((size_t)item - (size_t)key);
	}
	static int compareStr(void* item, UINT32 ix, Collection* collection, void* key) {
		return strncmp((char*)item, (char*)key);
	}
	static int compareInt(void* item, UINT32 ix, Collection* collection, void* key) {
		int i = *(int*)item;
		return i - *(int*)key;
	}

};

typedef int (CollectionCallback)(void* value, unsigned int ix, Collection* collection, void* args);

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