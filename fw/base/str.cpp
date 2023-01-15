#include <string.h>
#include "base/memory.h"
#include "base/str.h"

NS_FW_BASE_BEGIN

//*********************************************************
bool strnullorempty(const char* szStr) {
	return szStr == NULL || szStr[0] == '\0';
}
//*********************************************************
char* strncpy(char *dst, size_t len, const char *src) {
	size_t i = 0;
	if (dst != NULL && src != NULL) {
		for (; i < len && src[i] != '\0'; i++) {
			dst[i] = src[i];
		}
		dst[i] = '\0';
	}
	return dst;
}
//*********************************************************
UINT32 strlen(const char *str) {
	UINT32 len = 0;
	while (str[len] != '\0') len++;
	return len;
}
//*********************************************************
char* strdup(const char *src, size_t& len) {
	len = strlen(src);
	char* dst = MALLOC(char, len + 1);
	strncpy(dst, len, src);
	return dst;
}
//*********************************************************
char* strdup(const char *src) {
	size_t len = 0;
	return NS_FW_BASE::strdup(src, len);
}
//*********************************************************
char* substr(const char *str, size_t start, size_t length) {
	char* dst = MALLOC(char, length + 1);
	strncpy(dst, length, &str[start]);
	return dst;
}
//*********************************************************
int strncmp(const char *left, const char *right, size_t len) {
	int res = left[0] - right[0];
	for (size_t i = 0; i < len && res == 0; i++) {
		res = left[i] - right[i];
		if (left[i] == '\0' || right[i] == '\0') break;
	}
	return res;
}
//*********************************************************
void memset(void *dst, const char value, size_t len) {
	for (size_t i = 0; i < len; i++) {
		((char*)dst)[i] = value;
	}
}
//*********************************************************
void* memcpy(void *dest, const void *src, size_t count) {
/*	if (count > 16) {
		size_t dwCount = count >> 2;
		char* dwDest = (char*)ALIGN4(dest);
		char* dwSrc = (char*)ALIGN4(src);
		size_t offset = dwDest - (char*)dest;
		for (size_t i = 0; i < offset; i++) {
			((unsigned char*)dest)[i] = ((unsigned char*)src)[i];
		}
		for (size_t i = 0; i < dwCount; i++) {
			((unsigned int*)dwDest)[i] = ((unsigned int*)dwSrc)[i];
		}
		offset += dwCount << 2;
		dest = (void*)((size_t)dest + offset);
		src = (void*)((size_t)src + offset);
		for (size_t i = 0; i < count; i++) {
			((unsigned char*)dest)[i] = ((unsigned char*)src)[i];
		}
	}
	else */{
		for (size_t i = 0; i < count; i++) {
			((unsigned char*)dest)[i] = ((unsigned char*)src)[i];
		}
	}
	return dest;
}
//*********************************************************
int memcmp(byte* src, byte* dst, int length, int& ix) {
	var result = true;
	for (ix = 0; ix < length; ix++) {
		if (src[ix] != dst[ix]) {
			result = false;
			break;
		}
	}
	return result;
}
#ifdef _CRT

//*********************************************************
char* strchr(const char* str, char ch) {
	char* pos = NULL;
	if (str != NULL) {
		int i = 0;
		do {
			if (str[i] == ch) {
				pos = (char*)&str[i];
				break;
			}
		} while (str[i++] != '\0');
	}
	return pos;
}
//*********************************************************
size_t strspn(const char *dest, const char *src) {
	size_t i = 0;
	while (dest[i] != '\0' && strchr(src, dest[i]) != NULL) i++;
	return i;
}
//*********************************************************
size_t strcspn(const char *dest, const char *src) {
	size_t i = 0;
	while (dest[i] != '\0' && strchr(src, dest[i]) == NULL) i++;
	return i;
}
#endif
//*********************************************************
size_t strrspn(const char *dest, const char *src) {
	size_t len = strlen(dest);
	if (len > 0) {
		while (len > 0 && strchr(src, dest[len-1]) != NULL) len--;
	}
	return len;
}
//*********************************************************
size_t strrcspn(const char *dest, const char *src) {
	size_t len = strlen(dest);
	size_t i = 0;
	if (len > 0) {
		i = len - 1;
		while (i >= 0 &&
			strchr(src, dest[i]) == NULL)
			i--;
	}
	return i;
}

//*********************************************************
void strnncpy(char*& dst, size_t& size, size_t start, const char* src, const size_t length, const size_t offset) {
	if (start + length > size) {
		size_t newSize = (size << 1);
		dst = REALLOC(dst, char, newSize);
		size = newSize;
	}
	strncpy(&dst[start], length, &src[offset]);
}


//int strcontains(const char* src, const char* str) {
//	int res = 0;
//	if (src != NULL && str != NULL) {
//		int i = 0, j = 0, k = 0;
//		while (src[i] != '\0') {
//			if (src[i] == str[j]) {
//				k = i;
//				while (src[++i] == str[++j]);
//				if (str[j] == '\0') {
//					res = k;
//					break;
//				}
//				i = k, j = 0;
//			}
//			i++;
//		}
//	}
//	return res;
//}
//*********************************************************
//char** strsplit(const char* src, const char* separator, int& count) {
//	int i = 0;
//	while (src[i] != '\0') {
//		
//	}
//	return NULL;
//}
//*********************************************************
char* strtrim(const char* src, const char* str) {
	size_t offset = strspn(src, str);
	size_t length = strrspn(src, str);
	return substr(src, offset, length - offset);
}

NS_FW_BASE_END