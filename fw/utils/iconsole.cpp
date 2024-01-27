#include <stdarg.h>
#include "utils/iconsole.h"

NS_FW_BASE_USE

void IConsole::printf(const char* const format, ...) {
	va_list args;
	va_start(args, format);
	vprintf(format, args);
	va_end(args);
}