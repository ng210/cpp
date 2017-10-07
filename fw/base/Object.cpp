#include "base/Object.h"
#include "base/Null.h"
//#include "base/str.h"
#include "base/String.h"
#include "base/MemoryMgr.h"
#include <stdio.h>

NAMESPACE_FRMWRK_USE

/*****************************************************************************
* Object_ (structure)
*****************************************************************************/
Object_::Object_(void) {
	iHash_ = (long long)this;
	refCount_ = 1;
}
Object_::~Object_(void) {

}
int Object_::addRef_() {
	return ++refCount_;
}
int Object_::delRef_() {
	return --refCount_;
}
String Object_::toString() {
	String str("[Object]");
	return str;
}

int Object_::compareTo(Object_* obj) {
	return (this->iHash_ - obj->iHash_) & 0xFFFFFFFF;
}

/*****************************************************************************
 * Object (reference)
 *****************************************************************************/
Object::Object(void* ptr) {
}
void Object::addRef_() {
	ptr_->addRef_();
}
void Object::delRef_() {
	if (ptr_->delRef_() == 0) {
		if (ptr_ != Null::instance_.ptr_) {
			DEL_(ptr_);
			ptr_ = NULL;/* Null::instance_.ptr_;
			addRef_();*/
		}
		else {
			printf("Fatal: null cannot be deleted!\n");
		}
	}
}
void Object::null_() {
	delRef_();
	ptr_ = Null::instance_.ptr_;
	addRef_();
}

Object::Object(void) {
	ptr_ = NEW_(Object_);
	//NEW_(Object_, ptr_);
}
Object::Object(Object& ref) {
	ptr_ = ref.ptr_;
	addRef_();
}
Object::Object(const Null& ref) {
	ptr_ = Null::instance_.ptr_;
	addRef_();
}
Object::~Object(void) {
	delRef_();
}
Object& Object::operator=(Object& ref) {
	delRef_();
	ptr_ = ref.ptr_;
	addRef_();
	return *this;
}
Object& Object::operator=(const Null& ref) {
	null_();
	return *this;
}
int Object::compareTo(Object& obj) {
	return ptr_->compareTo(obj.ptr_);
}

String Object::toString() {
	String str("[Object]");
	return ptr_->toString();
}

bool Object::operator==(Object& ref) {
	return (compareTo(ref) == 0);
}

#ifdef _DEBUG
void Object::dump(Object& obj, const char* label) {
	if (label == NULL) {
		printf("%08llX.ref:%d\n", ((unsigned long long)obj.ptr_) & 0xffffffff, obj.ptr_->refCount_);
	}
	else {
		printf("%s: %08llX.ref:%d\n", label, ((unsigned long long)obj.ptr_) & 0xffffffff, obj.ptr_->refCount_);
	}
}
#endif