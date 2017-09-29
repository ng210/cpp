#ifndef __BASE64_H
#define __BASE64_H

#include "base_defs.h"

NAMESPACE_FRMWRK_BEGIN

class Base64 {
	static const char *m_szCodeTable;
public:
	static char* create(size_t iLength);
	static void create(char* szCode, size_t iLength);
	static char* encode(char* str);
	static char* encode(long long iCode);
	static char* decode(char* szCode);
	static long long toInt64(char* szCode);
};

NAMESPACE_FRMWRK_END

#endif
