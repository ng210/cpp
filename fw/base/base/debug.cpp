#include <stdio.h>
#include <stdarg.h>

#ifdef _DEBUG
#include "base/debug.h"

#ifdef _WINDOWS

void _VS_printLastError(const char *szText, char *szFileName, int iLine) {
	char *szBuffer= NULL;
	DWORD dwError = GetLastError();

	if (dwError != 0) {
		if (szText == NULL) szText = "*** ";
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			dwError,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR) &szBuffer,
			0, NULL );
		size_t iLen = strlen(szBuffer);
		if (iLen > 2 && (szBuffer[iLen-2] == '\n' || szBuffer[iLen-2] == '\r')) szBuffer[iLen-2] = '\0';
		_VS_debug("1>%s(%d) : %s%s\n", szFileName, iLine, szText, szBuffer);
	}
	LocalFree(szBuffer);
}

void _VS_debug(const char *szFormat, ...) {
#ifdef wvsprintf
	char szMessage[512];
	va_list args;
	va_start(args, szFormat);
	wvsprintf(szMessage, szFormat, args);
	OutputDebugString(szMessage);
#endif
}

#else
int _debug_last_error = 0;

void _VS_printLastError(const char *szText, char *szFileName, int iLine) {
	_VS_debug("1>%s(%d) : error code #%08x\n", szFileName, iLine, szText, _debug_last_error);
}

void _VS_debug(const char *szFormat, ...) {
	va_list args;
	va_start(args, szFormat);
	vprintf(szFormat, args);
}

#endif

#endif
