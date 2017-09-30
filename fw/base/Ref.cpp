#include "base/Ref.h"

NAMESPACE_FRMWRK_USE

Ref_::Ref_(void) {
	this->iRefCount_ = 1;
}

int Ref_::addRef() {
	return ++this->iRefCount_;
}

int Ref_::delRef() {
	return --this->iRefCount_;
}

Ref_::~Ref_(void) {
}
