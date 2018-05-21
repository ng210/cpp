#include <stdarg.h>
#include <wchar.h>
#include <stdio.h>
#include <windows.h>
#include "base/tracer-win.h"
#include "base/str.h"

NS_FW_BASE_BEGIN

// ANSI char Tracer
void TracerWinA::log(const void* format, ...) {
	char message[1024];
	va_list args;
	va_start(args, format);
	vsprintf_s(message, (const char*)format, args);
	OutputDebugStringA(message);
}
void TracerWinA::logLastError(const void* file, int line) {
	char buffer[256] = "";
	DWORD dwError = GetLastError();
	if (dwError != 0) {
		FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			dwError,
			0,	//MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPSTR)&buffer,
			256,
			NULL
		);
		if (buffer[0] == '\0') {
			NS_FW_BASE::strncpy(buffer, 256, "Unknown error!");
		}
		log("1>%s(%d): (%08x) %s", file, line, dwError, buffer);
	}
}

// Wide char Tracer
void TracerWinW::log(const void* format, ...) {
	wchar_t message[1024];
	va_list args;
	va_start(args, format);
	wvsprintfW(message, (const wchar_t*)format, args);
	OutputDebugStringW(message);
}
void TracerWinW::logLastError(const void* file, int line) {
	const wchar_t* buffer = NULL;
	DWORD dwError = GetLastError();
	if (dwError != 0) {
		FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			dwError,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPWSTR)&buffer,
			0, NULL
		);
		log("1>%s(%d): %s", file, line, buffer);
	}
}

NS_FW_BASE_END
