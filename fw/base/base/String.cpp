#include "base/str.h"
#include "base/String.h"
#include "base/MemoryMgr.h"

#include <stdio.h>

NS_FW_BASE_BEGIN

const String String::emptyInstance_("");

/*****************************************************************************
* String (structure)
*****************************************************************************/
String::String(void) {
	buffer_ = NULL;
	length_ = 0;
}
String::String(char *p) {
	buffer_ = (char*)p;
	length_ = strlen(p);
}
String::String(const char *p) {
	buffer_ = strdup(p, length_);
}
String::String(String& str) {
	buffer_ = strdup(str.buffer_, length_);
}
String::~String(void) {
	FREE(buffer_);
}
/*****************************************************************************
* Object
*****************************************************************************/
const char* String::getType() {
	return "string";
}
const char* String::toString() {
	return this->buffer_;
}
int String::compareTo(Object* str) {
	return strncmp(buffer_, str->toString(), length_);
}
void* String::valueOf() {
	return buffer_;
}
/*****************************************************************************
* Methods
*****************************************************************************/
//String* String::operator=(const String* str) {
//	ptr_ = ref.ptr_;
//	addRef_();
//	return *this;
//}
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
	if (ix > 0 && ix < length_) {
		ch = buffer_[ix];
	}
	return ch;
}
String* String::concat(String* str) {
	size_t len = length_ + str->length_ ;
	char* buffer = MALLOC (char, len + 1);
	strncpy(buffer, length_, buffer_);
	strncpy(&buffer[length_], str->length_, str->buffer_);
	buffer[len] = '\0';
	return NEW_(String, buffer);
}
String* String::concat(const char* str) {
	size_t len2 = strlen(str);
	size_t len = length_ + len2;
	char* buffer = MALLOC(char, len + 1);
	strncpy(buffer, length_, buffer_);
	strncpy(&buffer[length_], len2, str);
	buffer[len] = '\0';
	return NEW_(String, buffer);
}
bool String::endsWith(String* str) {
	return endsWith(str->buffer_);
}
bool String::endsWith(char* str) {
	size_t offs = length_ - strlen(str);
	return (lastIndexOf(str) == offs);
}
long long String::indexOf(String* str) {
	return indexOf(str->buffer_);
}
long long String::indexOf(const char* str) {
	return indexOf(str, 0);
}
long long String::indexOf(const char* str, size_t offset) {
	long long res = -1;
	size_t len1 = length_;
	for (size_t i = offset; i < len1; i++) {
		char ch = buffer_[i];
		if (ch == str[0]) {
			bool found = true;
			for (size_t j = 1; str[j] != '\0'; j++) {
				if (buffer_[i + j] != str[j]) {
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
long long String::lastIndexOf(String* str) {
	return lastIndexOf(str->buffer_);
}
long long String::lastIndexOf(char* str) {
	long long res = -1;
	size_t len1 = length_;
	size_t len2 = strlen(str);
	long long len = (long long)(len1 - len2);
	for (long long i = len; i >= 0; i--) {
		char ch = buffer_[i];
		if (ch == str[0]) {
			bool found = true;
			for (size_t j = 1; str[j] != '\0'; j++) {
				if (buffer_[i + j] != str[j]) {
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
//Array String::match(RegExp&);
String* String::replace(String* oldValue, String* newValue) {
	return replace(oldValue->buffer_, newValue->buffer_);
}
String* String::replace(const char* oldValue, const char* newValue) {
	long long arr[128];
	int count = 0;
	long long len = 0;
	size_t len1 = length_;
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
	char *buffer = MALLOC(char, len + 1);

	buffer[0] = '\0';
	size_t offs = 0;
	char *p = buffer;
	for (int i = 0; i < count; i++) {
		len = arr[i] - offs;
		strncpy(p, len, &buffer_[offs]);
		p += len;
		strncpy(p, len3, newValue);
		p += len3;
		offs = arr[i] + len2;
	}
	strncpy(p, len1 - offs, &buffer_[offs]);
	return NEW_(String, buffer);
}
//String String::replace(RegExp& old, String& new)
//String String::replace(Array&, Array&);
String* String::substr(long long start, long long length) {
	String* str;
	size_t len = length;
	if (start > (long long)length_) {
		str = NEW_(String, "");
	} else {
		if (start < 0) {
			start += length_;
			if (start < 0) {
				start = 0;
			}
		}
		if (len <= 0 || len > length_ - start) {
			len = length_ - start;
		}
		char *buffer = MALLOC(char, len + 1);
		strncpy(buffer, len, &buffer_[start]);
		str = NEW_(String, buffer);
	}
	return str;
}
String* String::substring(long long start, long long end) {
	if (start < 0) start = 0;
	if (end <= 0) end = length_;
	if (start > end) {
		size_t tmp = start;
		start = end;
		end = tmp;
	}
	return substr(start, end - start);
}
String** String::split(String* str) {
	return split(str->buffer_);
}
String** String::split(const char* str) {
	String* arr[128];
	size_t count = 0;
	size_t offs = 0;
	size_t len = strlen(str);
	for (size_t i = 0; i < length_; ) {
		long long ix = indexOf(str, i);
		if (ix == -1) {
			ix = length_;
		}
		arr[count] = NEW_(String);
		arr[count]->buffer_ = NS_FW_BASE::substr(buffer_, i, ix - i);
		count++;
		i += ix + len;
	}
	String** res = (String**)MALLOC(String*, count);
	memcpy((char*)res, (char*)arr, count * sizeof(String*));
	return res;
}
bool String::startsWith(String* str) {
	return startsWith(str->buffer_);
}
bool String::startsWith(char* str) {
	return (indexOf(str) == 0);
}
String* String::toLowerCase() {
	size_t len = length_;
	char *buffer = MALLOC(char, len + 1);
	for (size_t i = 0; i < len; i++) {
		char ch = buffer_[i];
		if (ch >= 'A' && ch <= 'Z') {
			ch |= 0x20;
		}
		buffer[i] = ch;
	}
	buffer[len] = '\0';
	return NEW_(String, buffer);
}
String* String::toUpperCase() {
	size_t len = length_;
	char *buffer = MALLOC(char, len + 1);
	for (size_t i = 0; i < len; i++) {
		char ch = buffer_[i];
		if (ch >= 'a' && ch <= 'zB') {
			ch &= (0xff - 0x20);
		}
		buffer[i] = ch;
	}
	buffer[len] = '\0';
	return NEW_(String, buffer);
}
String* String::trim() {
	String* str;
	size_t len = length_;
	// skip leading whitespaces
	size_t i = 0;
	char ch;
	while ((ch = buffer_[i]) != '\0' && (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r')) i++;
	size_t j = len;
	while (--j > i) {
		ch = buffer_[j];
		if (ch != ' ' && ch != '\t' && ch != '\n' && ch != '\r') {
			break;
		}
	}
	if (i <= j) {
		len = j - i + 1;
		char *buffer = MALLOC(char, len + 1);
		strncpy(buffer, len, &buffer_[i]);
		str = NEW_(String, buffer);
	} else {
		str = NEW_(String, "");
	}
	return str;
}

NS_FW_BASE_END