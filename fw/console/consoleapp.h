#ifndef __CONSOLEAPP_H
#define __CONSOLEAPP_H

#include <windows.h>
#include "runtime.h"

//*****************************************************************************
// CONSOLE
//*****************************************************************************
#define CONSOLE_BUFFER_LENGTH 1024*1024

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


class Console {
protected: PROP_R(HANDLE, hOutput);
protected: PROP_R(HANDLE, hInput);
protected: PROP_R(CONSOLE_SCREEN_BUFFER_INFO, consoleScreenBufferInfo);
protected: PROP_R(CONSOLE_CURSOR_INFO, consoleCursorInfo);
protected: PROP_R(char*, consoleBuffer);

public:
	Console();
	~Console();

	void showCursor(bool status);
	void printf(const char* const format, ...);
	void vprintf(const char* const format, va_list args);
	void dump(const byte* const data, int length, int width = 8);
	COORD* gotoxy(int x, int y);
	COORD* movexy(int x, int y);
	void setcolor(int col);
	void clearscreen();

	int keyPressed();
	CHAR getKey();
};

extern int _main(NS_FW_BASE::Map* args);

const char*& getWorkingDir();
Console* const getConsole();

int main(int, char**);

#endif