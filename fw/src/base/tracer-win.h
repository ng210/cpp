#ifndef __TRACERWIN_H
#define __TRACERWIN_H

#include "basedef.h"
#include "base/tracer.h"

NS_FW_BASE_BEGIN

// ANSI char TracerWin class 
class TracerWinA : public TracerA {
public:
	void log(const void* format, ...);
	void logLastError(const void* file, int line);
};

// Wide char TracerWin class
class TracerWinW : public TracerW {
public:
	void log(const void* format, ...);
	void logLastError(const void* file, int line);
};

NS_FW_BASE_END

#endif