#include "base/MemoryMgr.h"
#include "str.h"

NAMESPACE_FRMWRK_USE

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
size_t strlen(const char *str) {
	size_t len = 0;
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
	return strdup(src, len);
}
//*********************************************************
int strncmp(const char *left, const char *right, size_t len) {
	int res = 0;
	for (size_t i = 0; i < len && left[i] != '\0' && right[i] != '\0' && res == 0; i++) {
		res = left[i] - right[i];
	}
	return res;
}
//*********************************************************
void memcpy(char *dst, const char *src, size_t len) {
	for (size_t i = 0; i < len; i++) {
		dst[i] = src[i];
	}
}
//*********************************************************
void memset(void *dst, const char value, size_t len) {
	for (size_t i = 0; i < len; i++) {
		((char*)dst)[i] = value;
	}
}
//*********************************************************
void strnncpy(char*& dst, size_t& size, size_t start, const char* src, const size_t length, const size_t offset) {
	if (start + length > size) {
		size_t newSize = (size << 1);
		char *tmp = MALLOC(char, newSize);
		memcpy(tmp, dst, start);
		size = newSize;
		DEL_(dst);
		dst = tmp;
	}
	strncpy(&dst[start], length, &src[offset]);
}