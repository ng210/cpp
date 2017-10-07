#include "Null.h"
#include "base/MemoryMgr.h"
#include "base/String.h"

#include <stdio.h>

NAMESPACE_FRMWRK_USE

const Null_ Null_::instance_;
const Null Null::instance_;

Null_::Null_(void) {
}
Null_::~Null_(void) {
}

String Null_::toString() {
	return String("null");
}

Null::Null(void) : Object(NULL) {
	ptr_ = (Object_*)&Null_::instance_;
	addRef_();
}
Null::Null(Null& ref) : Object(ref) {
}
Null::~Null() {
}
