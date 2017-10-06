#include "base/Object.h"
#include "base/base64.h"
#include "base/MemoryMgr.h"
#include "base/String.h"

NAMESPACE_FRMWRK_USE

//const Ref<Object_> null(new Object_());

Object_::Object_(void) {
	iRefCount_ = 1;
	iHash_ = (long long)this;
	//szHashText_ = Base64::encode(iHash_);
}

Object_::~Object_(void) {
	//delete szHashText_;
}

char* Object_::toCString() {
	return Base64::encode(iHash_);	//szHashText_;
}

int Object_::compareTo(Object_ *ref) {
	return (int)(iHash_ - ref->iHash_);
}

/*****************************************************************************/
Object::Object(void) {
	NEW_(Object_, ptr_);
}

Object::Object(Object &ref) {
	// add new reference
	ptr_ = ref.ptr_;
	ptr_->addRef();
}

Object::~Object(void) {
	delRef();
}

Object& Object::operator=(const Object &ref) {
	if (ptr_ != ref.ptr_) {
		// Delete old reference
		delRef();
		// Set new reference
		ptr_ = ref.ptr_;
		ptr_->addRef();
	}
	return *this;
}

void Object::delRef() {
	int nRef = ptr_->delRef();
	if (nRef == 0) {
		DEL_( ptr_);
		ptr_ = NULL;
	}
}

Object::operator const char*() {
	return ptr_->toCString();
}

String& Object::toString() {
	String& str;
	NEW(String, str, toCString());
	return str;
}

bool Object::operator==(Object &ref) {
	return (ptr_->compareTo(ref.ptr_) == 0);
}


/*****************************************************************************/
char* NullObject_::toCString() {
	return "null";
}

NullObject::NullObject(void) {
	NEW_(NullObject_, ptr_);
}

const NullObject NullObject::null_;

//bool Object_::operator==(Object_& obj) {
//	return this->compareTo(&obj) == 0;
//}
//
//int Object_::compareTo(Object_ *obj) {
//	return this->iHash_ == obj->iHash_;
//}
