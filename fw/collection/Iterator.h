#ifndef __ITERATOR_H
#define __ITERATOR_H

#include "Base/base_defs.h"
#include "Base/Object.h"

//#include "Collection/Collection.h"

NAMESPACE_FRMWRK_BEGIN

class Iterator;
class Iterator_ : public Object_ {
	friend class Iterator;
	//friend class Collection_;
	PROP(protected, Object*, item, );
	PROP(protected, size_t, index, );
public:
	Iterator_(void);
	Iterator_(size_t iBegin);
	~Iterator_(void);
	//// pointer of current item
	//void* pItem;
	//// slot of current item
	//COLLECTIONITEM ppItem;
	//// global ordinal of current item
	//int iItem;
	//// Iterator of parent object
	//Iterator *pParentIterator;
	//// invalid item at index -1, used as pre-step item
	//COLLECTIONITEM pPreStepItem;
	//// ordinal of item within parent
	//int iLocal;
	//// auxillary data
	//void *pData;

	//Iterator(void **ppStart, int iStart);
	//~Iterator();
};


class Iterator : public Object {
public:
	Iterator(void);
	Iterator(size_t iBegin);

	DEFINE_COMMON_OPERATORS(Iterator);

	Object& get();
	size_t getIndex();
	void set(Object& obj);
	void setIndex(size_t ix);
	void set(Object& obj, size_t ix);
};
//typedef Iterator* (Collection::*LPBEGIN)(size_t iStart);
//typedef int (Collection::*LPNEXT)(Iterator *it);

NAMESPACE_FRMWRK_END

#endif