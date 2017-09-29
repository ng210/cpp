#ifndef __DEBUG_H
#define __DEBUG_H

#ifdef _WINDOWS
#include "windows.h"
#endif

void _VS_debug(const char *szFormat, ...);

#ifdef _DEBUG
void _VS_printLastError(const char *szText, char *szFileName, int iLine);
#define SYS(fn) fn; _VS_printLastError(NULL, __FILE__, __LINE__)
#else
#define SYS(fn) fn
#endif

#endif