#ifndef __STR_H
#define __STR_H

#include "basedef.h"
#include "types.h"

NS_FW_BASE_BEGIN

char* substr(const char *szStr, size_t start, size_t length);
char* strdup(const char *szStr, size_t& length);
char* strdup(const char *szStr);
UINT32 strlen(const char *str);
char* strncpy(char *dst, size_t len, const char *src);
int strncmp(const char *left, const char *right, size_t len = -1);
void strnncpy(char*& dst, size_t& size, size_t start, const char* src, const size_t length, const size_t offset = 0);
void memset(void *dst, const char value, size_t len);

#ifdef _CRT
void* memcpy(void *dest, const void *src, size_t count);

char* strchr(const char* str, char ch);
size_t strspn(const char *dest, const char *src);
size_t strcspn(const char *dest, const char *src);
#endif
size_t strrspn(const char *dest, const char *src);
size_t strrcspn(const char *dest, const char *src);
//char** strsplit(const char* src, const char* separator, int& count);

// extended string functions

NS_FW_BASE_END

#endif