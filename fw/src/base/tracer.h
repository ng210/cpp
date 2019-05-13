#ifndef __TRACER_H
#define __TRACER_H

#include "basedef.h"

NS_FW_BASE_BEGIN

// Tracer Base class
class Tracer {
public:
	virtual void log(const void* format, ...) = 0;
	virtual void logLastError(const void* file, int line) = 0;
	virtual void debug(const void* format, const void* file, int line, ...) = 0;
};

// ANSI char Tracer class 
class TracerA : public Tracer {
public:
	void log(const void* format, ...);
	void logLastError(const void* file, int line);
	void debug(const void* format, const void* file, int line, ...);
};

// Wide char Tracer class
class TracerW : public Tracer {
public:
	void log(const void* format, ...);
	void logLastError(const void* file, int line);
	void debug(const void* format, const void* file, int line, ...);
};

NS_FW_BASE_END

#endif