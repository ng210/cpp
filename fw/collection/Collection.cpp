/******************************************************************************
 *
 * Abstract base class for all Collection classes
 *
 *****************************************************************************/

#include "Collection.h"
#include "Base/MemoryMgr.h"

NAMESPACE_FRMWRK_USE

//*********************************************************
Collection_::Collection_(void) {
	iCount_ = 0;
}
//*********************************************************
Collection_::~Collection_(void) {
	//_VS_debug("Collection::~Collection\n");
	clear();
}

void Collection_::clear() {
	Iterator it = begin(0);
	while (next(it)) {
		remove((Object)it.get());
	}
	end(it);
	iCount_ = 0;
}

Object & Collection_::find(Object& obj, int& ix) {
	Object& res = null;
	ix = -1;
	Iterator it = begin(0);
	while (next(it)) {
		Object &item = it.get();
		if (item.compareTo(obj) == 0) {
			res = item;
			ix = it.getIndex();
			break;
		}
	}
	end(it);
	return res;
}

//*********************************************************
void Collection_::end(Iterator &it) {
	it = null;
}

//*********************************************************
Object& Collection_::first(Iterator &it) {
	it.setIndex(-1);
	next(it);
	return it.get();
}
//*********************************************************
Object& Collection_::last(Iterator &it) {
	it.setIndex(iCount_-1);
	next(it);
	return it.get();
}


/******************************************************************************
 * Collection ref
 *****************************************************************************/
size_t Collection::count() {
	return ((Collection_*)ptr_)->iCount_;
}

Object& Collection::apply(ACTION *pAction, void *args, int &ix) {
	Object& res = null;
	Iterator it = begin(0);
	ix = 0;
	while (next(it)) {
		Object &obj = it.get();
		if (!(*pAction)(*this, obj, ix, args)) {
			res = obj;
			break;
		}
		ix++;
	}
	end(it);
	return res;
}