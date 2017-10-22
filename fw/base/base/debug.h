#ifndef __DEBUG_H
#define __DEBUG_H

void _VS_debug(const char *szFormat, ...);

#ifdef _DEBUG
void _VS_printLastError(const char *szText, char *szFileName, int iLine);
#ifdef _WINDOWS
#include "windows.h"
#else
extern int _debug_last_error;
#endif

#define SYS(fn) fn; _VS_printLastError(NULL, __FILE__, __LINE__)
#else
#define SYS(fn) fn
#endif

#endif