#ifndef __STR_H
#define __STR_H

#include "base/base_defs.h"

NS_FW_BASE_BEGIN

char* substr(char *szStr, size_t start, size_t length);
char* strdup(const char *szStr, size_t& length);
char* strdup(const char *szStr);
size_t strlen(const char *str);
char* strncpy(char *dst, size_t len, const char *src);
int strncmp(const char *left, const char *right, size_t len);
void memcpy(char *dst, const char *src, size_t len);
void memset(void *dst, const char value, size_t len);
void strnncpy(char*& dst, size_t& size, size_t start, const char* src, const size_t length, const size_t offset=0);

NS_FW_BASE_END

#endif