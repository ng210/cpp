#include <string.h>
#include "Base/String.h"
#include "Base/MemoryMgr.h"

NAMESPACE_FRMWRK_USE

//*********************************************************
char* strdup(const char *szStr, size_t iLength) {
	char* szRes = 0;
	if (iLength > 0) {
		NEWARR(char, szRes, iLength + 1);
		memcpy(szRes, (void*)szStr, iLength);
		szRes[iLength] = '\0';
	}
	return szRes;
}
//*********************************************************
char* strdup_(const char *szStr) {
	return strdup(szStr, strlen(szStr));
}
/******************************************************************************
 * String_ class
 *  - Entity for String data
 *****************************************************************************/
String_::String_(void) {
	buffer_ = NULL;
	iLength_ = 0;
}

String_::~String_(void) {
	DEL_(buffer_);
	//DELARR(buffer_);
}

char* String_::toCString() {
	return strdup_(buffer_);
}

void String_::assign(char *szStr) {
	iLength_ = 0;
	while (szStr[iLength_++] != '\0');
	DEL_(buffer_);
	buffer_ = strdup_(szStr);
}

int String_::compareTo(const char* str) {
	return strcmp(buffer_, str);
}

size_t String_::length() {
	return iLength_;
}

/******************************************************************************
 * String ref
 *  - Entity for String functions
 *****************************************************************************/

String::String(void) {
	NEW_(String_, ptr_);
	//ptr_->addRef();
}

String::String(char *szStr) {
	NEW_(String_, ptr_);
	assign(szStr);
	//ptr_->addRef();
}

String::String(const char *szStr) {
	NEW_(String_, ptr_);
	assign((char*)szStr);
	//ptr_->addRef();
}

String& String::operator=(char *szStr) {
	delRef();
	NEW_(String_, ptr_);
	assign((char*)szStr);
	return *this;
}

int String::compareTo(String& str) {
	return ((String_*)ptr_)->compareTo(((String_*)str.ptr_)->buffer_);
}


//String& String::operator=(const String &ref) {
//	if (ptr_ != ref.ptr_) {
//		// delete old reference
//		delRef();
//		// set new reference
//		ptr_ = ref.ptr_;
//		((String_*)ptr_)->addRef();
//	}
//	return *this;
//}
//
//String& String::operator=(NullObject &ref) {
//	Object_ *p = ref.getPtr();
//	if (ptr_ != p) {
//		// delete old reference
//		delRef();
//		// set new reference
//		ptr_ = p;
//		addRef();
//	}
//	return *this;
//}

void String::assign(char *szStr) {
	((String_*)ptr_)->assign(szStr);
}

//
//size_t String::getLength() {
//	return ptr_->length_;
//}