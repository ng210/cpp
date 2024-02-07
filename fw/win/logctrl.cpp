#include "win/logctrl.h"

NS_FW_BASE_USE
NS_FW_WIN_USE

char* LogCtrl::windowClassName_ = "EDIT";

LogCtrl::LogCtrl() {
	color_ = 0x00008080;
	position_ = { 0, 0 };
	charBuffer_ = MALLOC(char, 1024 * 1024);
	linesCount_ = 0;
	bgColor(0x00402010);
}

LogCtrl::~LogCtrl() {
	logBuffer_.clear();
	SYSPR(DeleteObject(bgBrush_));
	FREE(charBuffer_);
}

char* const LogCtrl::registerWindowClass() {
	return LogCtrl::windowClassName_;
}

void LogCtrl::create(Window* parent, char* name, LONG style, DWORD exStyle) {
	Ctrl::create(parent, name, style | ES_READONLY | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL, exStyle);
}

void LogCtrl::showCursor(bool status) {
}

void LogCtrl::vprintf(const char* const format, va_list args) {
	wvsprintf(charBuffer_, format, args);
	logBuffer_.write(charBuffer_, NS_FW_BASE::strlen(charBuffer_) + 1, 0, logBuffer_.length() > 0 ? logBuffer_.length() - 1 : 0);
	char* buffer = logBuffer_.getBufferAsType<char>();
	SetWindowText(hWnd_, buffer);
	SendMessage(hWnd_, EM_LINESCROLL, 0, linesCount_);
	linesCount_++;
	FREE(buffer);
}

POINT* const LogCtrl::gotoxy(int x, int y) {
	position_.x = x;
	position_.y = y;
	return &position_;
}

POINT* const LogCtrl::movexy(int x, int y) {
	position_.x += x;
	position_.y += y;
	return &position_;
}

void LogCtrl::setcolor(int col) {
	color_ = col;
}

void LogCtrl::bgColor(COLORREF col) {
	bgColor_ = col;
	SYSPR(DeleteObject(bgBrush_));
	SYSFN(bgBrush_, CreateSolidBrush(col));
}

void LogCtrl::clearscreen() {
	logBuffer_.clear();
	char* buffer = "";
	SetWindowText(hWnd_, buffer);
}

char* LogCtrl::getBuffer() {
	return logBuffer_.getBufferAsType<char>();
}

//void LogCtrl::setText(TCHAR* text) {
//	SYSPR(SetWindowText(hWnd_, text));
//}
