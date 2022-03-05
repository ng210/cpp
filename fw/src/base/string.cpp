#include "base/memory.h"
#include "base/str.h"
#include "base/string.h"
#include <string.h>
#include <stdio.h>
//#include "collection/array.h"

NS_FW_BASE_BEGIN

char* str_concat(const char* str1, const char* str2) {
	return str_concat(2, str1, str2);
}
char* str_concat(int argc, ...) {
	va_list args;
	char* parts[256];
	size_t lengths[256];
	va_start(args, argc);
	size_t len = 0;
	for (int i = 0; i < argc; i++) {
		char* part = va_arg(args, char*);
		parts[i] = part;
		len += lengths[i] = strlen(part);
	}
	char* buffer = MALLOC(char, len + 1);
	char* ptr = buffer;
	for (int i = 0; i < argc; i++) {
		len = lengths[i];
		strncpy(ptr, len, parts[i]);
		ptr += len;
	}
	*ptr = '\0';
	return buffer;
}
int str_ends(const char* str, const char* part) {
	size_t offs = strlen(str) - strlen(part);
	return (str_lastIndexOf(str, part) == offs);
}
int str_indexOf(const char* str, const char* part, UINT32 start) {
	int offset = -1;
	char ch;
	for (int i = start; (ch = str[i]) != '\0'; i++) {
		if (ch == part[0]) {
			bool found = true;
			for (size_t j = 1; part[j] != '\0'; j++) {
				if (str[i + j] != part[j]) {
					found = false;
					break;
				}
			}
			if (found) {
				offset = i;
				break;
			}
		}
	}
	return offset;
}
char* str_join(int argc, const char* filler, ...) {
	va_list args;
	va_start(args, filler);
	char* parts[256];
	int i = 0;
	while (i < argc) {
		char* part = va_arg(args, char*);
		parts[i++] = part;
	}
	parts[i] = NULL;
	va_end(args);
	return str_join(parts, filler);
}
char* str_join(char** parts, const char* filler) {
	size_t size = 0;
	char* buffer = NULL;
	char* ptr = NULL;
	size_t ix = 0;
	size_t offset = 0;
	size_t fillerLength = strlen(filler);
	while (parts[ix] != NULL) {
		size_t len = strlen(parts[ix]);
		size_t end = offset + len + fillerLength;
		if (end > size) {
			size += 65536;
			buffer = REALLOC(buffer, char, size);
			ptr = buffer + offset;
		}
		strncpy(ptr, len, parts[ix]);
		ptr += len;
		strncpy(ptr, fillerLength, filler);
		ptr += fillerLength;
		offset = end;
		ix++;
	}
	offset -= fillerLength;
	buffer = REALLOC(buffer, char, offset + 1);
	buffer[offset] = '\0';

	//size_t len = 0;
	//size_t fillerLength = strlen(filler);
	//if (argc == -1) {
	//	for (argc = 0; parts[argc] != NULL; argc++);
	//}
	//char* buffer = NULL;
	//size_t size = 0;
	//for (int i = 0; parts[i] != NULL; i++) {
	//	size_t len = strlen(parts[i]);
	//	size += len;
	//	buffer = REALLOC(buffer, char, size);
	//	strncpy(&buffer[size], len, parts[i]);
	//}

	//size_t* lengths = MALLOC(size_t, argc);
	//for (int i = 0; i < argc; i++) {
	//	len += lengths[argc] = strlen(parts[argc]);
	//	len += fillerLength;
	//}
	//char* buffer = MALLOC(char, len + 1);
	//char* ptr = buffer;
	//for (int i = 0; parts[i] != NULL;) {
	//	len = lengths[i];
	//	strncpy(ptr, len, parts[i]);
	//	ptr += len;
	//	i++;
	//	if (i < argc) {
	//		strncpy(ptr, fillerLength, filler);
	//		ptr += fillerLength;
	//	}
	//}
	//*ptr = '\0';
	return buffer;
}
int str_lastIndexOf(const char* str, const char* part) {
	int res = -1;
	size_t len1 = strlen(str);
	size_t len2 = strlen(part);
	int len = (int)(len1 - len2);
	for (int i = len; i >= 0; i--) {
		char ch = str[i];
		if (ch == part[0]) {
			bool found = true;
			for (size_t j = 1; part[j] != '\0'; j++) {
				if (str[i + j] != part[j]) {
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
char* str_replace(const char* str, const char* oldValue, const char* newValue) {
	int offsets[256];
	int count = 0;
	UINT32 len = 0;
	UINT32 len1 = strlen(str);
	UINT32 len2 = strlen(oldValue);
	UINT32 len3 = strlen(newValue);
	for (UINT32 i = 0; i < len1; ) {
		int offset = str_indexOf(str, oldValue, i);
		if (offset == -1) {
			len += len1 - i;
			break;
		}
		len += offset - i + len3;
		offsets[count++] = offset;
		i = offset + len2;
	}
	char *buffer = MALLOC(char, (size_t)len + 1);
	buffer[0] = '\0';
	UINT32 offset = 0;
	char *ptr = buffer;
	for (int i = 0; i < count; i++) {
		len = offsets[i] - offset;
		strncpy(ptr, len, &str[offset]);
		ptr += len;
		strncpy(ptr, len3, newValue);
		ptr += len3;
		offset = offsets[i] + len2;
	}
	strncpy(ptr, len1 - offset, &str[offset]);
	return buffer;
}
char** str_split(const char* str, const char* splitter) {
	char* parts[128];
	UINT32 count = 0;
	UINT32 offs = 0;
	UINT32 len = strlen(str);
	UINT32 splitLen = strlen(splitter);
	size_t size = 0;
	for (UINT32 i = 0; i < len; ) {
		UINT32 ix = str_indexOf(str, splitter, i);
		if (ix == -1) {
			ix = len;
		}
		if (i != ix) {
			parts[count++] = substr(str, i, (size_t)(ix - i));
		}
		i = ix + splitLen;
	}
	parts[count++] = NULL;
	char** res = MALLOC(char*, count);
	memcpy((char*)res, (char*)parts, count * sizeof(char*));
	return res;
}
char* str_substr(const char* str, int start, int length) {
	if (start < 0) start = 0;
	if (length <= 0) {
		length = 0;
	}
	return length > 0 ? str_substring(str, start, start + length) : strdup("");
}
char* str_substring(const char* str, int start, int end) {
	int len = strlen(str);
	if (start < 0) start = 0;
	if (end <= 0) {
		end += len;
		if (end < 0) {
			end = start;
		}
	} else if (end > len) {
		end = len;
	}
	if (start > end) {
		int tmp = start;
		start = end;
		end = tmp;
	}
	char* buffer = NULL;
	if (start < end) {
		size_t remains = (size_t)end - start;
		buffer = MALLOC(char, remains + 1);
		strncpy(buffer, remains, &str[start]);
	} else {
		buffer = strdup("");
	}
	return buffer;
}
int str_starts(const char* str, const char* part) {
	return (str_indexOf(str, part) == 0);
}
char* str_ltrim(const char* str, const char* part) {
	size_t start = strspn(str, part);
	return start > 0 ? str_substring(str, (int)start) : strdup("");
}
char* str_rtrim(const char* str, const char* part) {
	size_t end = strrspn(str, part);
	return end > 0 ? str_substring(str, 0, (int)end) : strdup("");
}
char* str_trim(const char* str, const char* part) {
	size_t start = strspn(str, part);
	size_t end = strrspn(str, part);
	return start < end ? str_substring(str, (int)start, (int)end) : strdup("");
}
char* str_format(const char* format, ...) {
	char* buffer = MALLOC(char, 65536);
	va_list args;
	va_start(args, format);
	int length = vsprintf_s(buffer, 65536, format, args) + 1;
	REALLOC(buffer, char, length);
	return buffer;
}

//
//String* String::substring(long long start, long long end) {
//	if (start < 0) start = 0;
//	if (end <= 0) end = length_;
//	if (start > end) {
//		size_t tmp = start;
//		start = end;
//		end = tmp;
//	}
//	return substr(start, end - start);
//}
//String** String::split_(String* str) {
//	return split_(str->value_);
//}
//String** String::split_(const char* str) {
//	String* arr[128];
//	size_t count = 0;
//	size_t offs = 0;
//	size_t len = strlen(str);
//	for (size_t i = 0; i < length_; ) {
//		long long ix = indexOf(str, i);
//		if (ix == -1) {
//			ix = length_;
//		}
//		arr[count++] = NEW_(String, NS_FW_BASE::substr(value_, i, ix - i));
//		i = ix + len;
//	}
//	arr[count++] = NULL;
//	String** res = (String**)MALLOC(String*, count);
//	memcpy((char*)res, (char*)arr, count * sizeof(String*));
//	return res;
//}
//Array* String::split(String* str) {
//	return split(str->value_);
//}
//Array* String::split(const char* str) {
//	Array* arr = NEW_(Array);
//	size_t count = 0;
//	size_t offs = 0;
//	size_t len = strlen(str);
//	if (length_ > 0) {
//		for (size_t i = 0; i < length_; ) {
//			long long ix = indexOf(str, i);
//			if (ix == -1) {
//				ix = length_;
//			}
//			arr->push(NEW_(String, NS_FW_BASE::substr(value_, i, ix - i)));
//			i = ix + len;
//		}
//		if (arr->length() == 0) {
//			arr->push(this);
//		}
//	}
//	return arr;
//}
//bool String::startsWith(String* str) {
//	return startsWith(str->value_);
//}
//bool String::startsWith(char* str) {
//	return (indexOf(str) == 0);
//}
//String* String::toLowerCase() {
//	size_t len = length_;
//	char *buffer = MALLOC(char, len + 1);
//	for (size_t i = 0; i < len; i++) {
//		char ch = value_[i];
//		if (ch >= 'A' && ch <= 'Z') {
//			ch |= 0x20;
//		}
//		buffer[i] = ch;
//	}
//	buffer[len] = '\0';
//	return NEW_(String, buffer);
//}
//String* String::toUpperCase() {
//	size_t len = length_;
//	char *buffer = MALLOC(char, len + 1);
//	for (size_t i = 0; i < len; i++) {
//		char ch = value_[i];
//		if (ch >= 'a' && ch <= 'zB') {
//			ch &= (0xff - 0x20);
//		}
//		buffer[i] = ch;
//	}
//	buffer[len] = '\0';
//	return NEW_(String, buffer);
//}
//String* String::trim() {
//	String* str;
//	size_t len = length_;
//	// skip leading whitespaces
//	size_t i = 0;
//	char ch;
//	while ((ch = value_[i]) != '\0' && (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r')) i++;
//	size_t j = len;
//	while (--j > i) {
//		ch = value_[j];
//		if (ch != ' ' && ch != '\t' && ch != '\n' && ch != '\r') {
//			break;
//		}
//	}
//	if (i <= j) {
//		len = j - i + 1;
//		char *buffer = MALLOC(char, len + 1);
//		strncpy(buffer, len, &value_[i]);
//		str = NEW_(String, buffer);
//	} else {
//		str = NEW_(String, "");
//	}
//	return str;
//}

NS_FW_BASE_END