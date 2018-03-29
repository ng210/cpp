#ifndef __DEBUG_H
#define __DEBUG_H

#include "base/base_defs.h"

#ifdef _DEBUG

NS_FW_BASE_BEGIN

enum DEBUG_FLAGS : int {
	DEBUG_WINDOWS = 1,
	DEBUG_MEMORY = 2
};

typedef void (DEBUG_PRINT)(const char* format, const char* file, int line, ...);
typedef void (DEBUG_LASTERROR)(const char* szText, char *szFileName, int iLine);

// print: text => out("%s", text);
// debug: text, file, line => out(">%s(%d) : %s", file, line, text);
// lastError: => debug(getLastError);

class Tracer {
public:
	virtual void logW(const wchar_t* format, ...);
	virtual void logA(const char* format, ...);
	virtual const wchar_t* getLastErrorW();
	virtual const char* getLastErrorA();

	void debugW(const wchar_t* format, const wchar_t* file, int line, ...);
	void debugA(const char* format, const char* file, int line, ...);
};

class TracerWin : public Tracer {
public:
	void logW(const wchar_t* format, ...);
	void logA(const char* format, ...);
	const wchar_t* getLastErrorW();
	const char* getLastErrorA();
};

extern Tracer* tracer_;

NS_FW_BASE_END

#ifdef UNICODE
#define SYS(fn) fn; if (tracer_ != NULL) { const wchar_t* szError = tracer_->getLastErrorW(); if (szError != NULL && wcslen(szError) > 0) tracer_->debugW(szError, __TFILE__, __LINE__); LocalFree((LPVOID)szError); }
#define DEBUG(format, ...) if (tracer_ != NULL) tracer_->debugW(format, __TFILE__, __LINE__, __VA_ARGS__)
#define LOG(format, ...) if (tracer_ != NULL) tracer_->logW(format, __VA_ARGS__)

#else
#define SYS(fn) fn; if (tracer_ != NULL) { const char* szError = tracer_->getLastErrorA(); if (szError != NULL && strlen(szError) > 0) tracer_->debugA(szError, __TFILE__, __LINE__); LocalFree((LPVOID)szError); }
#define DEBUG(format, ...) if (tracer_ != NULL) tracer_->debugA(format, __TFILE__, __LINE__, __VA_ARGS__)
#define LOG(format, ...) if (tracer_ != NULL) tracer_->logA(format, __VA_ARGS__)
#endif

#else
#define SYS(fn) fn
#define DEBUG(format, ...) 
#endif

#endif