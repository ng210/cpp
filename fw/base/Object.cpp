#include "base/Object.h"
#include "base/str.h"
#include <stdio.h>

NAMESPACE_FRMWRK_USE

/*****************************************************************************
* Object_ (pointer)
*****************************************************************************/
Object_::Object_(void) {
	iHash_ = (long long)this;
}
Object_::~Object_(void) {

}
bool Object_::operator==(Object_* obj) {
	return (compareTo(obj) == 0);
}
Object_::operator const char*() {
	return toCString();
}
char* Object_::toCString() {
#ifdef _DEBUG
	//return "[Object %s]";
	char tmp[128];
	snprintf(tmp, 128, "[Object 0x%08X]", (int)iHash_);
	return strdup_(tmp);
#else
	return strdup_("[Object]");
#endif
}
int Object_::compareTo(Object_* obj) {
	return (this->iHash_ - obj->iHash_) & 0xFFFFFFFF;
}

/*****************************************************************************
 * Object (reference)
 *****************************************************************************/
Object::Object(void) {
	NEW_(Object_, ptr_);
}
Object::~Object(void) {
}

char* Object::toCString() {
	return ptr_->toCString();
}
int Object::compareTo(Object& ref) {
	return ptr_->compareTo(ref.ptr_);
}
bool Object::operator==(Object& ref) {
	return (compareTo(ref) == 0);
}
Object::operator const char*() {
	return toCString();
}