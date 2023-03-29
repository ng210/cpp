#ifndef __DEBUG_H
#define __DEBUG_H

#include "basedef.h"

#ifdef _DEBUG

#include "console/tracer.h"
#include "win/tracer-win.h"

NS_FW_BASE_BEGIN

enum DEBUG_FLAGS : int {
	DEBUG_WINDOWS	= 1,
	DEBUG_MEMORY	= 2,
	DEBUG_UNICODE	= 4
};

class Debug {
public:
	static Tracer* tracer_;
	static void initialize(int options = 0);
	static void destroy();
};

NS_FW_BASE_END

//#define SYS(fn) fn; if (var == 0 && Debug::tracer_ != NULL) Debug::tracer_->logLastError( __FILE__, __LINE__)
#define SYSPR(fn) if (fn == 0 && Debug::tracer_ != NULL) Debug::tracer_->logLastError( __FILE__, __LINE__)
#define SYSPR2(fn, err) if (fn == err && Debug::tracer_ != NULL) Debug::tracer_->logLastError( __FILE__, __LINE__)
#define SYSFN(var, fn) var = fn; if (var == 0 && Debug::tracer_ != NULL) Debug::tracer_->logLastError( __FILE__, __LINE__)
#define SYSFN2(var, fn, err) var = fn; if (var == err && Debug::tracer_ != NULL) Debug::tracer_->logLastError( __FILE__, __LINE__)
#define DEBUG(format, ...) if (Debug::tracer_ != NULL) Debug::tracer_->debug(format, __FILE__, __LINE__, __VA_ARGS__)
#define LOG(format, ...) if (Debug::tracer_ != NULL) Debug::tracer_->log(format, __VA_ARGS__)
#define INFO(format, ...) if (Debug::tracer_ != NULL) Debug::tracer_->log(format, __VA_ARGS__)

#else
#define SYSPR(fn) fn
#define SYSPR2(fn, err) fn
#define SYSFN(var, fn) var = fn
#define SYSFN2(var, fn, err) var = fn
#define DEBUG(format, ...) 
#define INFO(format, ...)
#define LOG(format, ...) printf(format, __VA_ARGS__)
#endif

#endif