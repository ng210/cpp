#ifndef __TRACER_H
#define __TRACER_H

#include "base/base_defs.h"

#ifdef _DEBUG

NS_FW_BASE_BEGIN

class Tracer {
public:
	void log(const TCHAR* format, ...);
	void debug(const TCHAR* format, const TCHAR* file, int line, ...);
	const TCHAR* getLastError();	
};

NS_FW_BASE_END

#endif

#endif