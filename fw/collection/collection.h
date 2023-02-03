#ifndef __COLLECTION_H
#define __COLLECTION_H

#include "basedef.h"
#include "base/str.h"

NS_FW_BASE_BEGIN

//*****************************************************************************
// Base class collection
//*****************************************************************************
class Collection;

typedef union Key {
	int i;
	void* p;

	Key() { p = NULL; }
	Key(void* p) { this->p = p; }
	Key(int i) { this->i = i; }
} Key;

typedef int (COLLECTIONCALLBACK)(void* value, Key key, UINT32 ix, Collection* collection, void* args);

class Collection {
protected: PROP_R(int, length);
protected: PROP(COLLECTIONCALLBACK*, compare);
protected:
public:
	Collection();
	virtual void* add(Key key, void* item = NULL) = 0;
	virtual void* get(Key key) = 0;
	virtual void* insert(Key key, void* item = NULL) = 0;
	virtual void remove(Key key) = 0;
	virtual void set(Key key, void* item) = 0;
	virtual void set(void** pOldItem, void* newItem) = 0;

	virtual int apply(COLLECTIONCALLBACK, ...) = 0;
	virtual void fill(void* value);
	virtual void* search(Key key, int& ix, COLLECTIONCALLBACK* compare = NULL);

	//virtual void* getAt(int ix) = 0;
	//virtual void removeAt(int ix) = 0;
	//virtual void setAt(int ix, void* item) = 0;
	//virtual void setItem(void* item, void* data) = 0;

	//virtual int findIndex(void* key, COLLECTIONCALLBACK* compare = NULL) = 0;

	//inline UINT32 length() { return length_;  }

	static int compareByRef(void* item, Key key, UINT32 ix, Collection* collection, void* args) {
		return (int)((size_t)item - (size_t)key.p);
	}
	static int compareStr(void* item, Key key, UINT32 ix, Collection* collection, void* args) {
		return strncmp((char*)item, (char*)key.p);
	}
	static int compareStrReverse(void* item, Key key, UINT32 ix, Collection* collection, void* args) {
		return strncmp((char*)key.p, (char*)item);
	}
	static int compareInt(void* item, Key key, UINT32 ix, Collection* collection, void* args) {
		int i = *(int*)item;
		return i - key.i;
	}

};

NS_FW_BASE_END

#endif