#include <stdarg.h>
#include <wchar.h>
#include <stdio.h>
#include "console/tracer.h"

NS_FW_BASE_USE

// ANSI char Tracer
void TracerA::log(const void* format, ...) {
	va_list args;
	va_start(args, format);
	vprintf((const char*)format, args);
	va_end(args);
}
void TracerA::logLastError(const void* file, int line) {
	// no error code, log nothing
}
void TracerA::debug(const void* format, const void* file, int line, ...) {
	// create "1> {file}({line}): {text}
	char buffer[1024];
	va_list args;
	va_start(args, line);
	vsprintf_s(buffer, (const char*)format, args);
	log("1>%s(%d): %s", file, line, buffer);
}
void TracerA::dump(const byte* const data, int length, int width) {
	var count = 0;
	char line[1024];
	int size = 1023;
	var p = line;
	for (var i = 0; i < length; i++) {
		unsigned char db = data[i];
		sprintf_s(p, size, "%02x ", db);
		size -= 3;
		p += 3;
		if (count++ == width) {
			log("%s\n", line);
			p = line;
			size = 1023;
			count = 0;
		}
	}
	if (p != line) {
		log("%s\n", line);
	}
}

// Wide char Tracer
void TracerW::log(const void* format, ...) {
	va_list args;
	va_start(args, format);
	vwprintf((const wchar_t*)format, args);
	va_end(args);

}
void TracerW::logLastError(const void* file, int line) {
	// no error code, log nothing
}
void TracerW::debug(const void* format, const void* file, int line, ...) {
	// create "1> {file}({line}): {text}
	wchar_t buffer[1024] = { '\0' };
	va_list args;
	va_start(args, line);
	if (format != NULL) {
		vswprintf_s(buffer, (const wchar_t*)format, args);
	}
	log(L"1>%s(%d): %s", file, line, buffer);
}
void TracerW::dump(const byte* const data, int length, int width) {
	var count = 0;
	wchar_t line[1024];
	int size = 1023;
	var p = line;
	for (var i = 0; i < length; i++) {
		unsigned char db = data[i];
		swprintf_s(p, size, L"%02x ", db);
		size -= 3;
		p += 3;
		if (count++ == width) {
			log("%s\n", line);
			p = line;
			size = 1023;
			count = 0;
		}
	}
	if (p != line) {
		log("%s\n", line);
	}
}