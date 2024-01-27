#ifndef __CONSOLEAPP_H
#define __CONSOLEAPP_H

#include <windows.h>
#include "runtime.h"
#include "utils/iconsole.h"

NS_FW_BASE_USE

//*****************************************************************************
// CONSOLE
//*****************************************************************************
#define CONSOLE_BUFFER_LENGTH 1024*1024

class Console : public IConsole {
private: NS_FW_BASE::POINT position_;
protected: PROP_R(HANDLE, hOutput);
protected: PROP_R(HANDLE, hInput);
protected: PROP_R(CONSOLE_SCREEN_BUFFER_INFO, consoleScreenBufferInfo);
protected: PROP_R(CONSOLE_CURSOR_INFO, consoleCursorInfo);
protected: PROP_R(char*, consoleBuffer);

public:
	Console();
	~Console();

	void showCursor(bool status);
	void vprintf(const char* const format, va_list args);
	void dump(const byte* const data, int length, int width = 8);
	NS_FW_BASE::POINT* const gotoxy(int x, int y);
	NS_FW_BASE::POINT* const movexy(int x, int y);
	void setcolor(int col);
	void clearscreen();

	int keyPressed();
	CHAR getKey();
};

extern int _main(NS_FW_BASE::Map* args);

const char*& getWorkingDir();
IConsole* const getConsole();

int main(int, char**);

#endif