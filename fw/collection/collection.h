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
	void* p;
	byte bytes[8];
	UINT64 l;
	int i;

	Key() { p = NULL; }
	Key(void* p) { this->p = p; }
	Key(int i) { this->l = (UINT64)i; }
	bool operator==(size_t iv) { return l == iv; }
	bool operator!=(size_t iv) { return l != iv; }
	bool operator==(void* pv) { return p == pv; }	
	bool operator!=(void* pv) { return p != pv; }
	//Key operator ()(int v) {
	//	Key k;
	//	k.i = v;
	//	return k;
	//}
} Key;

#define COLLECTION_ARGUMENTS void* value, Key key, Collection* collection, va_list args
typedef void* (COLLECTION_ACTION)(COLLECTION_ARGUMENTS);
typedef int (COLLECTION_COMPARE)(COLLECTION_ARGUMENTS);

class Collection {
protected: PROP_R(int, length);
protected: PROP(COLLECTION_COMPARE*, compare);
protected:
public:
	Collection();
	virtual ~Collection();
	virtual void* add(Key key, void* item = NULL) = 0;
	virtual void* get(Key key) = 0;
	virtual void* insert(Key key, void* item = NULL) = 0;
	virtual void remove(Key key) = 0;
	virtual void set(Key key, void* item) = 0;
	virtual void set(void** pOldItem, void* newItem) = 0;

	virtual int apply(COLLECTION_ACTION, ...) = 0;
	virtual void fill(void* value);
	virtual Collection* map(COLLECTION_ACTION* action, bool removeNull = false, ...) = 0;
	virtual void* search(Key key, Key& found, COLLECTION_COMPARE* compare = NULL);

	virtual void from(Collection* collection);

	//virtual void* getAt(int ix) = 0;
	//virtual void removeAt(int ix) = 0;
	//virtual void setAt(int ix, void* item) = 0;
	//virtual void setItem(void* item, void* data) = 0;

	//virtual int findIndex(void* key, COLLECTIONCALLBACK* compare = NULL) = 0;

	//inline UINT32 length() { return length_;  }

	static COLLECTION_COMPARE compareByRef;
	static COLLECTION_COMPARE compareStr;
	static COLLECTION_COMPARE compareStrReverse;
	static COLLECTION_COMPARE compareInt;
};

NS_FW_BASE_END

#endif