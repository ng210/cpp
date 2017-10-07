#include "base/str.h"
#include "base/String.h"
#include "base/MemoryMgr.h"
#include "base/Null.h"
#include "base/Array.h"

#include <stdio.h>

NAMESPACE_FRMWRK_USE

const String_ String_::emptyInstance_("");

/*****************************************************************************
* String_ (structure)
*****************************************************************************/
String_::String_(void) {
	buffer_ = NULL;
	length_ = 0;
}
String_::String_(const char *p) {
	buffer_ = strdup(p, length_); 
}
String_::String_(char *p) {
	buffer_ = (char*)p;
	length_ = strlen(p);
}
String_::~String_(void) {
	DEL_(buffer_);
	//DELARR(buffer_);
}
String String_::toString() {
	return String(this->buffer_);
}
int String_::compareTo(Object_* str) {
	return strncmp(buffer_, ((String_*)str)->buffer_, length_);
}

#define THIS ((String_&)*ptr_)
#define PTR(o) ((String_&)*(o.ptr_))
/*****************************************************************************
* Object (reference)
*****************************************************************************/
const String String::Empty(&String_::emptyInstance_);
String::String(void) : Object(NULL) {
	ptr_ = NEW_(String_);
}
String::String(const String_* str) : Object(NULL) {
	ptr_ = (Object_*)str;
	addRef_();
}
String::String(String& ref) : Object(NULL) {
	ptr_ = ref.ptr_;
	addRef_();
}
String::String(const Null& ref) : Object(ref) {
}
String::String(char *str) : Object(NULL) {
	ptr_ = NEW_(String_, str);
}
String::String(const char *str) : Object(NULL) {
	ptr_ = NEW_(String_, str);
}
String::~String(void) {
}
/*****************************************************************************
* Methods
*****************************************************************************/
void String::empty_() {
	delRef_();
	ptr_ = String::Empty.ptr_;
	addRef_();
}

String String::operator=(const String& ref) {
	delRef_();
	ptr_ = ref.ptr_;
	addRef_();
	return *this;
}
//String String::operator=(String& ref) {
//	delRef_();
//	ptr_ = ref.ptr_;
//	addRef_();
//	return *this;
//}
//String String::operator=(const Null& ref) {
//	null_();
//	return *this;
//}
char String::operator[](int ix) {
	char ch = '\0';
	if (ix > 0 && ix < THIS.length_) {
		ch = THIS.buffer_[ix];
	}
	return ch;
}
String String::operator+(String& str) {
	size_t len = THIS.length_ + PTR(str).length_ ;
	char* buffer = NEWARR(char, len + 1);
	strncpy(buffer, THIS.length_, THIS.buffer_);
	strncpy(&buffer[THIS.length_], PTR(str).length_, PTR(str).buffer_);
	buffer[len] = '\0';
	return String(buffer);
}
String String::operator+(const char* str) {
	return *this + String(str);
}
bool String::startsWith(String& str) {
	return startsWith(PTR(str).buffer_);
}
bool String::startsWith(char* str) {
	return (indexOf(str) == 0);
}
long long String::indexOf(String& str) {
	return indexOf(PTR(str).buffer_);
}
long long String::indexOf(const char* str) {
	long long res = -1;
	size_t len1 = THIS.length_;
	for (size_t i = 0; i < len1; i++) {
		char ch = THIS.buffer_[i];
		if (ch == str[0]) {
			bool found = true;
			for (size_t j = 1; str[j] != '\0'; j++) {
				if (THIS.buffer_[i+j] != str[j]) {
					found = false;
					break;
				}
			}
			if (found) {
				res = i;
				break;
			}
		}
	}
	return res;
}
long long String::indexOf(const char* str, size_t offset) {
	long long res = -1;
	size_t len1 = THIS.length_;
	for (size_t i = offset; i < len1; i++) {
		char ch = THIS.buffer_[i];
		if (ch == str[0]) {
			bool found = true;
			for (size_t j = 1; str[j] != '\0'; j++) {
				if (THIS.buffer_[i + j] != str[j]) {
					found = false;
					break;
				}
			}
			if (found) {
				res = i;
				break;
			}
		}
	}
	return res;
}

long long String::lastIndexOf(String& str) {
	return lastIndexOf(PTR(str).buffer_);
}
long long String::lastIndexOf(char* str) {
	long long res = -1;
	size_t len1 = THIS.length_;
	size_t len2 = strlen(str);
	long long len = (long long)(len1 - len2);
	for (long long i = len; i >= 0; i--) {
		char ch = THIS.buffer_[i];
		if (ch == str[0]) {
			bool found = true;
			for (size_t j = 1; str[j] != '\0'; j++) {
				if (THIS.buffer_[i + j] != str[j]) {
					found = false;
					break;
				}
			}
			if (found) {
				res = i;
				break;
			}
		}
	}
	return res;
}
bool String::endsWith(String& str) {
	return endsWith(PTR(str).buffer_);
}
bool String::endsWith(char* str) {
	size_t offs = THIS.length_ - strlen(str);
	return (lastIndexOf(str) == offs);
}
//Array String::match(RegExp&);
String String::replace(String& oldValue, String& newValue) {
	return replace(PTR(oldValue).buffer_, PTR(newValue).buffer_);
}
String String::replace(const char* oldValue, const char* newValue) {
	long long arr[128];
	int count = 0;
	long long len = 0;
	size_t len1 = THIS.length_;
	size_t len2 = strlen(oldValue);
	size_t len3 = strlen(newValue);
	for (size_t i = 0; i < len1; ) {
		long long res = indexOf(oldValue, i);
		if (res == -1) {
			len += len1 - i;
			break;
		}
		len += res - i + len3;
		arr[count++] = res;
		i = res + len2;
	}
	char *buffer = NEWARR(char, len + 1);

	buffer[0] = '\0';
	size_t offs = 0;
	char *p = buffer;
	for (int i = 0; i < count; i++) {
		len = arr[i] - offs;
		strncpy(p, len, &THIS.buffer_[offs]);
		p += len;
		strncpy(p, len3, newValue);
		p += len3;
		offs = arr[i] + len2;
	}
	strncpy(p, len1 - offs, &THIS.buffer_[offs]);
	return String(buffer);
}
//String String::replace(RegExp& old, String& new)
//String String::replace(Array&, Array&);
String String::substr(long long start, long long length) {
	String str;
	size_t len = length;
	if (start > (long long)THIS.length_) {
		str = String::Empty;
	} else {
		if (start < 0) {
			start += THIS.length_;
			if (start < 0) {
				start = 0;
			}
		}
		if (len <= 0 || len > THIS.length_ - start) {
			len = THIS.length_ - start;
		}
		char *buffer = NEWARR(char, len + 1);
		strncpy(buffer, len, &THIS.buffer_[start]);
		str = String(buffer);
	}
	return str;
}
String String::substring(long long start, long long end) {
	if (start < 0) start = 0;
	if (end <= 0) end = THIS.length_;
	if (start > end) {
		size_t tmp = start;
		start = end;
		end = tmp;
	}
	return substr(start, end - start);
}
Array String::split(String& str) {
	return split(PTR(str).buffer_);
}
Array String::split(const char* str) {
	Array arr;
	size_t offs = 0;
	long long res = indexOf(str, offs);
	if (res == -1) {

	}
	return arr;
}
String String::toLowerCase() {
	size_t len = THIS.length_;
	char *buffer = NEWARR(char, len + 1);
	for (size_t i = 0; i < len; i++) {
		char ch = THIS.buffer_[i];
		if (ch >= 'A' && ch <= 'Z') {
			ch |= 0x20;
		}
		buffer[i] = ch;
	}
	buffer[len] = '\0';
	return String(buffer);
}
String String::toUpperCase() {
	size_t len = THIS.length_;
	char *buffer = NEWARR(char, len + 1);
	for (size_t i = 0; i < len; i++) {
		char ch = THIS.buffer_[i];
		if (ch >= 'a' && ch <= 'zB') {
			ch &= (0xff - 0x20);
		}
		buffer[i] = ch;
	}
	buffer[len] = '\0';
	return String(buffer);
}
String String::trim() {
	String str;
	size_t len = THIS.length_;
	// skip leading whitespaces
	size_t i = 0;
	char ch;
	while ((ch = THIS.buffer_[i]) != '\0' && (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r')) i++;
	size_t j = len;
	while (--j > i) {
		ch = THIS.buffer_[j];
		if (ch != ' ' && ch != '\t' && ch != '\n' && ch != '\r') {
			break;
		}
	}
	if (i <= j) {
		len = j - i + 1;
		char *buffer = NEWARR(char, len + 1);
		strncpy(buffer, len, &THIS.buffer_[i]);
		str = String(buffer);
	} else {
		str = String::Empty;
	}
	return str;
}

const char* String::toCString() {
	return THIS.buffer_;
}

#undef THIS
#undef PTR
