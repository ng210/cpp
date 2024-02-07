#include <stdarg.h>
#include <stdio.h>
#include "utils/iconsole.h"

NS_FW_BASE_USE

void IConsole::printf(const char* const format, ...) {
	va_list args;
	va_start(args, format);
	vprintf(format, args);
	va_end(args);
}

void IConsole::dump(const byte* const data, int length, int width) {
	var count = 0;
	char line[1024];
	int size = 1023;
	var p = line;
	for (var i = 0; i < length; i++) {
		unsigned char db = data[i];
		sprintf_s(p, size, "%02x ", db);
		size -= 3;
		p += 3;
		if (++count == width) {
			printf("%s\r\n", line);
			p = line;
			size = 1023;
			count = 0;
		}
	}
	if (p != line) {
		printf("%s\r\n", line);
	}
}