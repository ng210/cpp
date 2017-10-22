#ifndef __SARRAY_H
#define __SARRAY_H

#include "Collection/IndexedCollection.h"

NAMESPACE_FRMWRK_BEGIN


#ifndef __CRT
#include <string.h>
#endif

class SArray;
//*****************************************************************************
// Static Array
//*****************************************************************************
class SArray_ : public IndexedCollection_ {
	friend class SArray;
	SArray_(void);
	SArray_(size_t iCount);
protected:
	// Size of allocated space in bytes.
	size_t iCapacity_;
	// Pointer of allocated space.
	Object* data_;

	//void* _remove(int ix, int bRef);
public:
	virtual ~SArray_(void);

	void add(Object &obj);
	void remove(Object &obj);

	// insert at index
	void insertAt(size_t ix, Object &obj);
	// get item at index
	Object& getAt(size_t ix);
	// set item at index
	void setAt(size_t ix, Object& obj);
	// remove item at index
	void removeAt(size_t ix);

	// Begin iteration
	Iterator& begin(size_t iStart);
	// Next iteration step
	int next(Iterator &it);
	// Previous iteration step
	int prev(Iterator &it);
	// Seek to position
	Iterator& seek(size_t pos);
	// Get first item and set iterator
	virtual Object& first(Iterator &it);
	// Get last item and set iterator
	virtual Object& last(Iterator &it);
};

//*****************************************************************************
// Static Array ref
//*****************************************************************************
class SArray : public IndexedCollection {

public:
	SArray(void);
	SArray(size_t iCount);

	DEFINE_COMMON_OPERATORS(SArray)

};

NAMESPACE_FRMWRK_END

#endif