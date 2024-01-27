#include <stdio.h>
#include "console/consoleapp.h"

NS_FW_BASE_USE

static const char* workingDir_;
Console* console_;

const char*& getWorkingDir() {
	return workingDir_;
}
IConsole* const getConsole() { return console_; }
int main(int argc, char** argv) {
	int error = 0;
#ifdef _DEBUG
	Debug::initialize(/*DEBUG_UNICODE | DEBUG_MEMORY*/);
	//MemoryMgr::isDebugOn = true;
#endif
	//RunTime::initialize();
	MemoryMgr::resetMemDbgInfo();
	console_ = NEW_(Console);

	Map* args = NEW_(Map, MAP_USE_REF, MAP_USE_REF, Map::hashingStr, Collection::compareStr);
	PathInfo* workingDir = NEW_(PathInfo, argv[0]);
	workingDir_ = workingDir->path();
	for (int i = 0; i < argc; i++) {
		int count = 0;
		size_t pos = strcspn(argv[i], "=");
		char* key = substr(argv[i], 0, pos);
		char* value = (argv[i][pos] != '\0') ? substr(argv[i], pos+1, NS_FW_BASE::strlen(argv[i])) : NS_FW_BASE::strdup("1");
		args->put(key, value);
	}

	error = _main(args);

	//MAP_FOREACH(args, FREE(key); FREE(value););
	for (int i = 0; i < args->size(); i++) {
		FREE(args->keys()->get(i));
		FREE(args->values()->get(i));
	}
	DEL_(args);
	DEL_(workingDir);
	DEL_(console_);

#ifdef _DEBUG
	//void* exceptions[2] = {
	//	(void*)String::empty(),
	//	(void*)Object::null()
	//};
	MemoryMgr::checkMemDbgInfo(0, NULL);

	Debug::destroy();
#endif

	return error;
}

//*****************************************************************************
// CONSOLE
//*****************************************************************************
Console::Console() {
	hOutput_ = GetStdHandle(STD_OUTPUT_HANDLE);
	hInput_ = GetStdHandle(STD_INPUT_HANDLE);
	consoleCursorInfo_ = {};
	consoleBuffer_ = MALLOC(char, CONSOLE_BUFFER_LENGTH);
	GetConsoleScreenBufferInfo(hOutput_, &consoleScreenBufferInfo_);
	position_ = { 0, 0 };
}
Console::~Console() {
	FREE(consoleBuffer_);
	consoleBuffer_ = NULL;
}

void Console::showCursor(bool status) {
	SYSPR(GetConsoleCursorInfo(hOutput_, &consoleCursorInfo_));
	consoleCursorInfo_.bVisible = status;
	SYSPR(SetConsoleCursorInfo(hOutput_, &consoleCursorInfo_));
}

void Console::vprintf(const char* const format, va_list args) {
	vsprintf_s(consoleBuffer_, CONSOLE_BUFFER_LENGTH, format, args);
	DWORD dwBytesWritten = 0;
	DWORD reserved = 0;
	SYSPR(WriteConsole(hOutput_, consoleBuffer_, NS_FW_BASE::strlen(consoleBuffer_), (LPDWORD)&dwBytesWritten, &reserved));
}
NS_FW_BASE::POINT* const Console::gotoxy(int x, int y) {
	SYSPR(GetConsoleScreenBufferInfo(hOutput_, &consoleScreenBufferInfo_));
	consoleScreenBufferInfo_.dwCursorPosition.X = x;
	consoleScreenBufferInfo_.dwCursorPosition.Y = y;
	SYSPR(SetConsoleCursorPosition(hOutput_, consoleScreenBufferInfo_.dwCursorPosition));
	position_.x = (int)consoleScreenBufferInfo_.dwCursorPosition.X;
	position_.y = (int)consoleScreenBufferInfo_.dwCursorPosition.Y;
	return &position_;
}
NS_FW_BASE::POINT* const Console::movexy(int x, int y) {
	SYSPR(GetConsoleScreenBufferInfo(hOutput_, &consoleScreenBufferInfo_));
	consoleScreenBufferInfo_.dwCursorPosition.X += x;
	consoleScreenBufferInfo_.dwCursorPosition.Y += y;
	SYSPR(SetConsoleCursorPosition(hOutput_, consoleScreenBufferInfo_.dwCursorPosition));
	position_.x = (int)consoleScreenBufferInfo_.dwCursorPosition.X;
	position_.y = (int)consoleScreenBufferInfo_.dwCursorPosition.Y;
	return &position_;
}

void Console::setcolor(int col) {
	SYSPR(SetConsoleTextAttribute(hOutput_, col & 0x0f));
}

void Console::clearscreen() {
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	int result;
	SYSFN(result, GetConsoleScreenBufferInfo(hOutput_, &csbi));
	if (result) {
		var size = csbi.dwSize.X * csbi.dwSize.Y;
		unsigned long count = 0;
		COORD topLeft = { 0, 0 };
		SYSFN(result, FillConsoleOutputCharacter(hOutput_, (TCHAR)' ', size, topLeft, &count));
		if (result) {
			SYSFN(result, GetConsoleScreenBufferInfo(hOutput_, &csbi));
			if (result) {
				SYSFN(result, FillConsoleOutputAttribute(hOutput_, csbi.wAttributes, size, topLeft, &count));
				if (result) {
					SYSFN(result, SetConsoleCursorPosition(hOutput_, topLeft));
				}
			}
		}
	}
}

void Console::dump(const byte* const data, int length, int width) {
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
			printf("%s\n", line);
			p = line;
			size = 1023;
			count = 0;
		}
	}
	if (p != line) {
		printf("%s\n", line);
	}
}

int Console::keyPressed() {
	DWORD eventCount = 0;
	SYSPR(GetNumberOfConsoleInputEvents(hInput_, &eventCount));
	return eventCount;
}

CHAR Console::getKey() {
	INPUT_RECORD buffer;
	DWORD eventCount = 0;
	SYSPR(ReadConsoleInput(hInput_, &buffer, 1, &eventCount));
	return buffer.Event.KeyEvent.uChar.AsciiChar;
}