#include "base/str.h"
#include "base/String.h"
#include "base/MemoryMgr.h"
#include "collection/Array.h"
#include "base/Number.h"

#include <stdio.h>

NS_FW_BASE_BEGIN

const String* String::emptyInstance_ = NULL;
const Type* String::classType_;

void String::initialize() {
	// add singleton empty instance
	String::emptyInstance_ = NEW_(String, "");
	// add type entry
	classType_ = ADD_TYPE(String);	// Type::add(STRINGIFY(NS_FW_BASE)"::string");
}
void String::shutDown() {
	DEL_(String::emptyInstance_);
}

/*****************************************************************************
* String (structure)
*****************************************************************************/
String::String(void) {
	value_ = NULL;
	length_ = 0;
}
String::String(char *p) {
	value_ = (char*)p;
	length_ = strlen(p);
}
String::String(const char *p) {
	value_ = strdup(p, length_);
}
String::String(String& str) {
	value_ = strdup(str.value_, length_);
}
String::~String(void) {
	FREE(value_);
}

const char* String::empty() {
	return String::emptyInstance_->value_;
}

/*****************************************************************************
* Object
*****************************************************************************/
char* String::toString() {
	return strdup(value_, length_);
}
bool String::toBool() {
	return length_ != 0;
}
Number* String::toNumber() {
	// parseDouble
	return NEW_(Number);
}
int String::strcmp(const char* str) {
	return strncmp(value_, str, length_);
}
int String::compareTo(Object* str) {
	char* buf = str->toString();
	int res = strncmp(value_, buf, length_);
	FREE(buf);
	return res;
}
void* String::valueOf() {
	return value_;
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
		ch = value_[ix];
	}
	return ch;
}
bool String::operator==(Object& o) {
	return this->strcmp((const char*)o.toString());
}
String& String::operator=(String& str) {
	DEL_(value_);
	value_ = strdup(str.value_);
	length_ = str.length_;
	return *this;
}
String& String::operator=(const char* str) {
	DEL_(value_);
	value_ = strdup(str);
	length_ = strlen(str);
	return *this;
}
String* String::concat(String* str) {
	size_t len = length_ + str->length_ ;
	char* buffer = MALLOC (char, len + 1);
	strncpy(buffer, length_, value_);
	strncpy(&buffer[length_], str->length_, str->value_);
	buffer[len] = '\0';
	return NEW_(String, buffer);
}
String* String::concat(const char* str) {
	size_t len2 = strlen(str);
	size_t len = length_ + len2;
	char* buffer = MALLOC(char, len + 1);
	strncpy(buffer, length_, value_);
	strncpy(&buffer[length_], len2, str);
	buffer[len] = '\0';
	return NEW_(String, buffer);
}
bool String::endsWith(String* str) {
	return endsWith(str->value_);
}
bool String::endsWith(char* str) {
	size_t offs = length_ - strlen(str);
	return (lastIndexOf(str) == offs);
}
long long String::indexOf(String* str) {
	return indexOf(str->value_);
}
long long String::indexOf(const char* str) {
	return indexOf(str, 0);
}
long long String::indexOf(const char* str, size_t offset) {
	long long res = -1;
	size_t len1 = length_;
	for (size_t i = offset; i < len1; i++) {
		char ch = value_[i];
		if (ch == str[0]) {
			bool found = true;
			for (size_t j = 1; str[j] != '\0'; j++) {
				if (value_[i + j] != str[j]) {
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
	return lastIndexOf(str->value_);
}
long long String::lastIndexOf(char* str) {
	long long res = -1;
	size_t len1 = length_;
	size_t len2 = strlen(str);
	long long len = (long long)(len1 - len2);
	for (long long i = len; i >= 0; i--) {
		char ch = value_[i];
		if (ch == str[0]) {
			bool found = true;
			for (size_t j = 1; str[j] != '\0'; j++) {
				if (value_[i + j] != str[j]) {
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
String* String::join(String** arr, String* sep) {
	return String::join(arr, sep->value_, sep->length_);
}
String* String::join(String** arr, const char* sep, size_t sepLength) {
	if (sepLength == 0 && sep != NULL) {
		sepLength = strlen(sep);
	}
	size_t len = 0;
	size_t i = 0;
	String* str;
	while ((str = arr[i++]) != NULL) {
		len += str->length_;
		len += sepLength;
	}
	i--;
	len -= sepLength;
	char* buffer = MALLOC(char, len+1);
	size_t start = 0;
	for (size_t j=0; j<i; j++) {
		str = arr[j];
		len = str->length_;
		strncpy(&buffer[start], len, str->value_);
		start += len;
		if (sepLength > 0 && j < i - 1) {
			strncpy(&buffer[start], sepLength, sep);
			start += sepLength;
		}
	}
	return NEW_(String, buffer);
}
//Array String::match(RegExp&);
String* String::replace(String* oldValue, String* newValue) {
	return replace(oldValue->value_, newValue->value_);
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
		strncpy(p, len, &value_[offs]);
		p += len;
		strncpy(p, len3, newValue);
		p += len3;
		offs = arr[i] + len2;
	}
	strncpy(p, len1 - offs, &value_[offs]);
	return NEW_(String, buffer);
}
//String String::replace(RegExp& old, String& new)
//String String::replace(Array&, Array&);
String* String::substr(long long start, size_t length) {
	String* str = NULL;
	if (start < 0) {
		start += length_;
		if (start < 0) {
			start = 0;
		}
	}
	size_t st = start;
	if (st < length_) {
		size_t remains = length_ - start;
		if (remains > length) remains = length;
		char *buffer = MALLOC(char, remains + 1);
		strncpy(buffer, remains, &value_[start]);
		str = NEW_(String, buffer);
	} else {
		str = NEW_(String, "");
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
String** String::split_(String* str) {
	return split_(str->value_);
}
String** String::split_(const char* str) {
	String* arr[128];
	size_t count = 0;
	size_t offs = 0;
	size_t len = strlen(str);
	for (size_t i = 0; i < length_; ) {
		long long ix = indexOf(str, i);
		if (ix == -1) {
			ix = length_;
		}
		arr[count++] = NEW_(String, NS_FW_BASE::substr(value_, i, ix - i));
		i = ix + len;
	}
	arr[count++] = NULL;
	String** res = (String**)MALLOC(String*, count);
	memcpy((char*)res, (char*)arr, count * sizeof(String*));
	return res;
}
Array* String::split(String* str) {
	return split(str->value_);
}
Array* String::split(const char* str) {
	Array* arr = NEW_(Array);
	size_t count = 0;
	size_t offs = 0;
	size_t len = strlen(str);
	for (size_t i = 0; i < length_; ) {
		long long ix = indexOf(str, i);
		if (ix == -1) {
			ix = length_;
		}
		arr->push(NEW_(String, NS_FW_BASE::substr(value_, i, ix - i)));
		i = ix + len;
	}
	return arr;
}
bool String::startsWith(String* str) {
	return startsWith(str->value_);
}
bool String::startsWith(char* str) {
	return (indexOf(str) == 0);
}
String* String::toLowerCase() {
	size_t len = length_;
	char *buffer = MALLOC(char, len + 1);
	for (size_t i = 0; i < len; i++) {
		char ch = value_[i];
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
		char ch = value_[i];
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
	while ((ch = value_[i]) != '\0' && (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r')) i++;
	size_t j = len;
	while (--j > i) {
		ch = value_[j];
		if (ch != ' ' && ch != '\t' && ch != '\n' && ch != '\r') {
			break;
		}
	}
	if (i <= j) {
		len = j - i + 1;
		char *buffer = MALLOC(char, len + 1);
		strncpy(buffer, len, &value_[i]);
		str = NEW_(String, buffer);
	} else {
		str = NEW_(String, "");
	}
	return str;
}

NS_FW_BASE_END