#ifndef __DEBUG_H
#define __DEBUG_H

#include "basedef.h"

#ifdef _DEBUG

#include "base/tracer.h"
#include "base/tracer-win.h"

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

#define SYS(fn) fn; if (Debug::tracer_ != NULL) Debug::tracer_->logLastError( __FILE__, __LINE__)
#define DEBUG(format, ...) if (Debug::tracer_ != NULL) Debug::tracer_->debug(format, __FILE__, __LINE__, __VA_ARGS__)
#define LOG(format, ...) if (Debug::tracer_ != NULL) Debug::tracer_->log(format, __VA_ARGS__)

#else
#define SYS(fn) fn
#define DEBUG(format, ...) 
#endif

#endif