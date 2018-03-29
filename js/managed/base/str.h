#ifndef __STR_H
#define __STR_H

#include "base/base_defs.h"

size_t strlen(const char *str);
char* strncpy(char *dst, size_t len, const char *src);
char* strdup(const char *szStr, size_t& iLength);
char* strdup(const char *szStr);
int strncmp(const char *left, const char *right, size_t len);
void memcpy(char *dst, const char *src, size_t len);
void memset(void *dst, const char value, size_t len);
void strnncpy(char*& dst, size_t& size, size_t start, const char* src, const size_t length, const size_t offset);
#endif