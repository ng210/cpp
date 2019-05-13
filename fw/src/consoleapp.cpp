#include "consoleapp.h"
#include <stdio.h>

NS_FW_BASE_USE

static char* workingDir_;
Console* console_;

const char* getWorkingDir() {
	return workingDir_;
}

int main(int argc, char** argv) {
	int error = 0;
#ifdef _DEBUG
	Debug::initialize(/*DEBUG_UNICODE | DEBUG_MEMORY*/);
	//MemoryMgr::isDebugOn = true;
#endif
	//RunTime::initialize();
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

	MAP_FOREACH(args, FREE(key); FREE(value););
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
	hConsole_ = GetStdHandle(STD_OUTPUT_HANDLE);
	consoleBuffer_ = MALLOC(char, CONSOLE_BUFFER_LENGTH);
	GetConsoleScreenBufferInfo(hConsole_, &consoleScreenBufferInfo_);
}
Console::~Console() {
	FREE(consoleBuffer_);
	consoleBuffer_ = NULL;
}

void Console::showCursor(bool status) {
	SYSPR(GetConsoleCursorInfo(hConsole_, &consoleCursorInfo_));
	consoleCursorInfo_.bVisible = status;
	SYSPR(SetConsoleCursorInfo(hConsole_, &consoleCursorInfo_));
}
void Console::printf(const char* const format, ...) {
	va_list args;
	va_start(args, format);
	vprintf(format, args);
	va_end(args);
}
void Console::vprintf(const char* const format, va_list args) {
	vsprintf_s(consoleBuffer_, CONSOLE_BUFFER_LENGTH, format, args);
	DWORD dwBytesWritten = 0;
	DWORD reserved;
	SYSPR(WriteConsole(hConsole_, consoleBuffer_, NS_FW_BASE::strlen(consoleBuffer_), (LPDWORD)&dwBytesWritten, &reserved));
}
COORD* Console::gotoxy(int x, int y) {
	SYSPR(GetConsoleScreenBufferInfo(hConsole_, &consoleScreenBufferInfo_));
	consoleScreenBufferInfo_.dwCursorPosition.X += x;
	consoleScreenBufferInfo_.dwCursorPosition.Y += y;
	SYSPR(SetConsoleCursorPosition(hConsole_, consoleScreenBufferInfo_.dwCursorPosition));
	return &consoleScreenBufferInfo_.dwCursorPosition;
}