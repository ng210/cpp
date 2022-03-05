#include <stdio.h>
#include <stdarg.h>

#ifdef _DEBUG
#include <windows.h>
#include "base/Tracer.h"
#include "base/MemoryMgr.h"

NS_FW_BASE_BEGIN

Tracer* tracer_ = NULL;

void Tracer::log(const TCHAR* format, ...) {
	va_list args;
	va_start(args, format);
	vprintf(format, args);
	va_end(args);
}

void Tracer::debug(const TCHAR* format, const TCHAR* file, int line, ...) {
	// create "1> {file}({line}): {text}
	TCHAR buffer[1024] = { '\0' };
	va_list args;
	va_start(args, line);
	if (format != NULL) {
		wvsprintf(buffer, format, args);
	}
	log("1>%s(%d): %s", file, line, buffer);
}

const TCHAR* Tracer::getLastError() {
	return NULL;
}


NS_FW_BASE_END

#endif