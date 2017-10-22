#include <stdio.h>
#include "base/Object.h"
#include "base/Null.h"
#include "base/String.h"
#include "base/MemoryMgr.h"

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
	refCount_++;
#ifdef _DEBUG
	dump("addRef");
#endif
	return refCount_;
}
int Object_::delRef_() {
	refCount_--;
#ifdef _DEBUG
	dump("delRef");
#endif
	return refCount_;
}
const char* Object_::getType() {
	return "object";
}
String Object_::toString() {
	String str("[Object]");
	return str;
}
void* Object_::valueOf() {
	return this;
}

int Object_::compareTo(Object_* obj) {
	return (this->iHash_ - obj->iHash_) & 0xFFFFFFFF;
}

#ifdef _DEBUG
void Object_::dump(const char* label) {
	if (Object::isDebugOn) {
		if (label == NULL) {
			printf(" *** (%s)%llX.refCount_ = %d\n", getType(), (long long)this, refCount_);
		}
		else {
			printf(" *** %s: (%s)%llX.refCount_ = %d\n", label, getType(), (long long)this, refCount_);

		}
	}
}
#endif

/*****************************************************************************
 * Object (reference)
 *****************************************************************************/
#ifdef _DEBUG
bool Object::isDebugOn = false;
#endif

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
String Object::getType() {
	return String(ptr_->getType());
}
int Object::compareTo(Object& obj) {
	return ptr_->compareTo(obj.ptr_);
}

String Object::toString() {
	return ptr_->toString();
}

bool Object::operator==(Object& ref) {
	return (compareTo(ref) == 0);
}

void* Object::valueOf() {
	return ptr_->valueOf();
}

#ifdef _DEBUG
void Object::dump(const char* label) {
	ptr_->dump(label);
}
#endif