#ifndef __ICONSOLE_H
#define __ICONSOLE_H

#include "basedef.h"

NS_FW_BASE_BEGIN

typedef enum ConsoleColors {
	black = 0,
	blue = 1,
	green = 2,
	cyan = 3,
	red = 4,
	magenta = 5,
	yellow = 6,
	gray = 7
} ConsoleColors;

#ifndef POINT
typedef struct tagPOINT {
	long x;
	long y;
} POINT;
#endif

class IConsole {
public:
	virtual void showCursor(bool status) = 0;
	void printf(const char* const format, ...);
	virtual void vprintf(const char* const format, va_list args) = 0;
	virtual void dump(const byte* const data, int length, int width = 8) = 0;
	virtual NS_FW_BASE::POINT* const gotoxy(int x, int y) = 0;
	virtual NS_FW_BASE::POINT* const movexy(int x, int y) = 0;
	virtual void setcolor(int col) = 0;
	virtual void clearscreen() = 0;
};

NS_FW_BASE_END
#endif