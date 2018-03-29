#include <stdio.h>
#include <stdarg.h>

#ifdef _DEBUG
#include <windows.h>
#include "base/debug.h"
#include "base/MemoryMgr.h"

NS_FW_BASE_BEGIN

Tracer* tracer_ = NULL;

void Tracer::logW(LPCWSTR format, ...) {
	va_list args;
	va_start(args, format);
	vwprintf(format, args);
	va_end(args);
}
void Tracer::logA(LPCSTR format, ...) {
	va_list args;
	va_start(args, format);
	vprintf(format, args);
	va_end(args);
}

LPCWSTR Tracer::getLastErrorW() {
	return NULL;
}
LPCSTR Tracer::getLastErrorA() {
	return NULL;
}

void Tracer::debugW(LPCWSTR format, LPCWSTR file, int line, ...) {
	// create "1> {file}({line}): {text}
	wchar_t buffer[1024] = { '\0' };
	va_list args;
	va_start(args, line);
	if (format != NULL) {
		wvsprintfW(buffer, format, args);
	}
	logW(L"1>%s(%d): %s", file, line, buffer);
}

void Tracer::debugA(LPCSTR format, LPCSTR file, int line, ...) {
	// create "1> {file}({line}): {text}
	char buffer[1024];
	va_list args;
	va_start(args, line);
	vsprintf(buffer, format, args);
	logA("1>%s(%d): %s", file, line, buffer);
}

void TracerWin::logW(LPCWSTR format, ...) {
	wchar_t message[1024];
	va_list args;
	va_start(args, format);
	wvsprintfW(message, format, args);
	OutputDebugStringW(message);
}

void TracerWin::logA(LPCSTR format, ...) {
	char message[1024];
	va_list args;
	va_start(args, format);
	vsprintf(message, format, args);
	OutputDebugStringA(message);
}

LPCWSTR TracerWin::getLastErrorW() {
	LPCWSTR buffer = NULL;
	DWORD dwError = GetLastError();

	if (dwError != 0) {
		FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			dwError,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPWSTR)&buffer,
			0, NULL
		);
	}
	return buffer;
}

LPCSTR TracerWin::getLastErrorA() {
	LPCSTR buffer = NULL;
	DWORD dwError = GetLastError();

	if (dwError != 0) {
		FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			dwError,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPSTR)&buffer,
			0, NULL
		);
	}
	return buffer;
}


//DEBUG_PRINT* Debug::print_ = &Debug::print;
//DEBUG_LASTERROR* Debug::lastError_ = &Debug::lastErrorFn;

//void Debug::init(DEBUG_FLAGS flags) {
//	Debug::print_ = (flags & DEBUG_WINDOWS) ? &Debug::debugWinFn : &Debug::debugFn;
//	Debug::lastError_ = (flags & DEBUG_WINDOWS) ? &Debug::lastErrorWinFn : &Debug::lastErrorFn;
//	MemoryMgr::isDebugOn = (flags & DEBUG_MEMORY);
//}

//void Debug::print(const LPTSTR format, const LPTSTR file, int line, ...) {
//	va_list args;
//	va_start(args, format);
//#if UNICODE
//	TCHAR fileW[512];
//	int len = mbstowcs(fileW, file, strlen(file));
//	vwprintf(format, args);
//#else
//	vwprintf(format, args);
//#endif
//}

//void Debug::printWin(const LPTSTR format, const LPTSTR file, int line, ...) {
//#ifdef wvsprintf
//	WCHAR szMessage[1024];
//	//WCHAR szFormatW[512];
//	va_list args;
//	va_start(args, format);
//	//MultiByteToWideChar(CP_OEMCP, 0, szFormat, -1, szFormatW, 512);
//	wvsprintf(szMessage, szFormat, args);
//	OutputDebugString(szMessage);
//#endif
//}
//
//void Debug::lastErrorFn(const LPTSTR text, char *file, int line) {
//	Debug::print_(L"1>%s(%d) : error code #%08x\n", file, line, text, 0);
//}
//
//void Debug::lastErrorWinFn(const LPTSTR szText, char *szFileName, int iLine) {
//	LPTSTR szBuffer = NULL;
//	DWORD dwError = GetLastError();
//	
//	if (dwError != 0) {
//		LPTSTR wszText = (szText == NULL)  ? szText : L"Error: ";
//		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
//			NULL,
//			dwError,
//			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
//			(LPTSTR) &szBuffer,
//			0, NULL
//		);
//		char szFormat[512];
//		WCHAR szFormatW[1024];
//		//size_t iLen = strlen(szBuffer);
//		//if (iLen > 2 && (szBuffer[iLen-2] == '\n' || szBuffer[iLen-2] == '\r')) szBuffer[iLen-2] = '\0';
//		sprintf(szFormat, "1>%s(%d)", szFileName, iLine);
//		MultiByteToWideChar(CP_OEMCP, 0, szFormat, -1, szFormatW, 1024);
//		Debug::print_(L"%s : %s%s\n", szFormatW, szText, szBuffer);
//	}
//	LocalFree(szBuffer);
//}

//#ifdef _WINDOWS
//#include <windows.h>
//void _VS_printLastError(const char *szText, char *szFileName, int iLine) {
//	char *szBuffer= NULL;
//	DWORD dwError = GetLastError();
//
//	if (dwError != 0) {
//		if (szText == NULL) szText = "*** ";
//		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
//			NULL,
//			dwError,
//			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
//			(LPTSTR) &szBuffer,
//			0, NULL );
//		size_t iLen = strlen(szBuffer);
//		if (iLen > 2 && (szBuffer[iLen-2] == '\n' || szBuffer[iLen-2] == '\r')) szBuffer[iLen-2] = '\0';
//		_VS_debug("1>%s(%d) : %s%s\n", szFileName, iLine, szText, szBuffer);
//	}
//	LocalFree(szBuffer);
//}
//
//void _VS_debug(const char *szFormat, ...) {
//#ifdef wvsprintf
//	char szMessage[512];
//	va_list args;
//	va_start(args, szFormat);
//	wvsprintf(szMessage, szFormat, args);
//	OutputDebugString(szMessage);
//#endif
//}
//
//#else
//int _debug_last_error = 0;
//
//void _VS_debug(const char *szFormat, ...) {
//	va_list args;
//	va_start(args, szFormat);
//	vprintf(szFormat, args);
//}
//
//void _VS_printLastError(const char *szText, char *szFileName, int iLine) {
//	_VS_debug("1>%s(%d) : error code #%08x\n", szFileName, iLine, szText, _debug_last_error);
//}
//#endif

NS_FW_BASE_END

#endif
