#ifndef __LOGCTRL_H
#define __LOGCTRL_H

#include "win/editctrl.h"
#include "utils/iconsole.h"
#include "utils/buffer.h"

NS_FW_WIN_BEGIN

class LogCtrl : public EditCtrl, public IConsole {
	static char* windowClassName_;
	POINT position_;
	int color_;
	Buffer logBuffer_;
	char* charBuffer_;
	int linesCount_;
	HBRUSH bgBrush_;
protected: PROP_R(COLORREF, bgColor);
public:
	LogCtrl();
	~LogCtrl();

	char* const registerWindowClass();
	void create(Window* parent, char* name, LONG style = NULL, DWORD exStyle = NULL);

	void showCursor(bool status);
	void vprintf(const char* const format, va_list args);
	POINT* const gotoxy(int x, int y);
	POINT* const movexy(int x, int y);
	void setcolor(int col);
	void bgColor(COLORREF color);
	void clearscreen();

	char* getBuffer();
};

NS_FW_WIN_END

#endif