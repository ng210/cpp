#ifndef __INDEXEDCOLLECTION_H
#define __INDEXEDCOLLECTION_H

#include "Collection/Collection.h"

NAMESPACE_FRMWRK_BEGIN

class IndexedCollection;
//*****************************************************************************
// Indexed Collection abstract base class
//*****************************************************************************
class IndexedCollection_ : public Collection_ {
	friend class IndexedCollection;
public:
	virtual ~IndexedCollection_();

	// insert at index
	virtual void insertAt(size_t ix, Object &obj) = 0;
	// get item at index
	virtual Object& getAt(size_t ix) = 0;
	Object& operator[](int ix) { return getAt(ix); }
	// set item at index
	virtual void setAt(size_t ix, Object& obj) = 0;
	// remove item at index
	virtual void removeAt(size_t ix) = 0;
};
//*****************************************************************************
// Indexed Collection ref base class
//*****************************************************************************
class IndexedCollection : public Collection {
public:
	// insert at index
	DEFINE_PROC2(IndexedCollection, insertAt, size_t, ix, Object&, obj);
	// get item at index
	DEFINE_FUNC1(IndexedCollection, getAt, Object&, size_t, ix);
	Object& operator[](int ix) { return getAt(ix); }
	// set item at index
	DEFINE_PROC2(IndexedCollection, setAt, size_t, ix, Object&, obj);
	// remove item at index
	DEFINE_PROC1(IndexedCollection, removeAt, size_t, ix);
};

NAMESPACE_FRMWRK_END

#endif