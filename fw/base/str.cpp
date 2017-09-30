#include "base/MemoryMgr.h"
#include <string.h>

NAMESPACE_FRMWRK_USE

//*********************************************************
char* strdup_(const char *szStr, size_t iLength) {
	char* szRes = 0;
	if (iLength > 0) {
		NEWARR(char, szRes, iLength + 1);
		memcpy(szRes, (void*)szStr, iLength);
		szRes[iLength] = '\0';
	}
	return szRes;
}
//*********************************************************
char* strdup_(const char *szStr) {
	return strdup_(szStr, strlen(szStr));
}