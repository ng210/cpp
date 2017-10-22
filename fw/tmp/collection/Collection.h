#ifndef __COLLECTION_H
#define __COLLECTION_H

#include "base/base_defs.h"
#include "base/Object.h"
#include "collection/Iterator.h"

NAMESPACE_FRMWRK_BEGIN

class Collection;
// Type of callback functions for actions on items.
typedef int (ACTION)(Collection &collection, Object &item, int ix, void* args);

//// Type for accessing collection items
//typedef union COLLECTIONITEM {
//	void *p;		// simple pointer
////	void **ref;		// reference
//	Object *obj;	// Object reference
//	int i;			// raw integer value
//} COLLECTIONITEM;

class Collection;
//*****************************************************************************
// Collection abstract base class
//*****************************************************************************
class Collection_ : public Object_ {
	friend class Collection;
protected:
	//// Collection stores references
	//PROP_R(protected, int, Ref, b);
	// Count of items
	PROP_R(protected, size_t, Count, i);
public:
	Collection_(void);
	virtual ~Collection_(void);
	//// init - constructor
	//void init(int bRef);
	//// destroy - destructor
	//virtual void destroy() = 0;

	// Add new item
	virtual void add(Object &obj) = 0;
	//// Add object
	//virtual COLLECTIONITEM addObj(void *pItem, size_t iItemSize);
	// Remove item
	virtual void remove(Object &obj) = 0;

	//*********************************************************
	// Enumerable
	//*********************************************************
	// Apply callback action on each item.
	//virtual void* apply(ACTION* pAction, void* pArgs, int& ix);
	// clear by removing each item
	virtual void clear(void);
	// Find item using callback.
	virtual Object& find(Object& obj, int& ix);
	//*********************************************************
	// Iteration support
	//*********************************************************
	// Begin iteration
	virtual Iterator& begin(size_t iStart) = 0;
	// End iteration (cleanup)
	virtual void end(Iterator &it);
	// Next iteration step
	virtual int next(Iterator &it) = 0;
	// Previous iteration step
	virtual int prev(Iterator &it) = 0;
	// Seek to position
	virtual Iterator& seek(size_t pos) = 0;
	// Get first item and set iterator
	virtual Object& first(Iterator &it);
	// Get last item and set iterator
	virtual Object& last(Iterator &it);
};

#include "Collection/Iterator.h"

//*****************************************************************************
// Collection ref base class
//*****************************************************************************
class Collection : public Object {
public:
	// Return count of items
	size_t count();
	// Add new item
	DEFINE_PROC1(Collection, add, Object&, obj);

	// Remove item
	DEFINE_PROC1(Collection, remove, Object&, obj);

	// clear by removing each item
	DEFINE_FUNC0(Collection, clear, void);
	// Apply callback action on each item.
	virtual Object& apply(ACTION* pAction, void* args, int& ix);
	// Find item
	DEFINE_FUNC2(Collection, find, Object&, Object&, obj, int&, ix);

	//*********************************************************
	// Begin iteration
	DEFINE_FUNC1(Collection, begin, Iterator&, size_t, iStart);
	// End iteration (cleanup)
  	DEFINE_PROC1(Collection, end, Iterator& ,it);
	// Next iteration step
	DEFINE_FUNC1(Collection, next, int, Iterator&, it);
	// Previous iteration step
	DEFINE_FUNC1(Collection, prev, int, Iterator&, it);
	// Seek to position
	DEFINE_FUNC1(Collection, seek, Iterator&, size_t, pos);
	// Get first item and set iterator
	DEFINE_FUNC1(Collection, first, Object&, Iterator&, it);
	// Get last item and set iterator
	DEFINE_FUNC1(Collection, last, Object&, Iterator&, it);
};

#ifdef __CRT
#include "crt.h"
#endif

NAMESPACE_FRMWRK_END

#endif