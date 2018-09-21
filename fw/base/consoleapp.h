#ifndef __CONSOLEAPP_H
#define __CONSOLEAPP_H

#include <windows.h>
#include "runtime.h"

//*****************************************************************************
// CONSOLE
//*****************************************************************************
class Console {
protected: PROP_R(HANDLE, hConsole);
protected: PROP_R(CONSOLE_SCREEN_BUFFER_INFO, consoleScreenBufferInfo);
protected: PROP_R(CONSOLE_CURSOR_INFO, consoleCursorInfo);
protected: PROP_R(char*, consoleBuffer);

#define CONSOLE_BUFFER_LENGTH 1024*1024

public:
	Console();
	~Console();

	void showCursor(bool status);
	void printf(const char* const format, ...);
	void vprintf(const char* const format, va_list args);
	COORD* gotoxy(int x, int y);
};

extern Console console;

extern int _main(NS_FW_BASE::Map* args);

const char* getWorkingDir();

int main(int, char**);

#endif