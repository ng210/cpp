#include "Iterator.h"

#include "Base/MemoryMgr.h"

NAMESPACE_FRMWRK_USE

//*********************************************************
Iterator_::Iterator_(void) {
	item_ = &null;
	index_ = 0;
}
Iterator_::Iterator_(size_t iBegin) {
	item_ = &null;
	index_ = iBegin;
}
Iterator_::~Iterator_(void) {
}

/******************************************************************************
 * Iterator ref
 *****************************************************************************/
Iterator::Iterator(void) {
	NEW_(Iterator_, ptr_);
}

Iterator::Iterator(size_t iBegin) {
	NEW_(Iterator_, ptr_, iBegin);
}

Object& Iterator::get(void) {
	return *((Iterator_*)ptr_)->item_;
}

size_t Iterator::getIndex() {
	return ((Iterator_*)ptr_)->index_;
}

void Iterator::set(Object& obj) {
	((Iterator_*)ptr_)->item(&obj);
}
void Iterator::setIndex(size_t ix) {
	((Iterator_*)ptr_)->index(ix);
}
void Iterator::set(Object& obj, size_t ix) {
	((Iterator_*)ptr_)->item_ = &obj;
	((Iterator_*)ptr_)->index_ = ix;
}

// Inherited from Object
//char* toString();
//int compareTo(Object &);
