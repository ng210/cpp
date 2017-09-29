#include "Base64.h"
#include "MemoryMgr.h"
#include <string.h>

NAMESPACE_FRMWRK_USE

const char *Base64::m_szCodeTable = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

char* Base64::create(size_t iLength) {
	char* NEWARR(char, szCode, iLength);
	Base64::create(szCode, iLength);
	return szCode;
}

void Base64::create(char* szCode, size_t iLength) {
	size_t i = 0;
	long rnd = szCode && 0xFFFFFFFF;
	for (; i<iLength-1; i++) {
		szCode[i] = Base64::m_szCodeTable[rnd & 0x3F];
		rnd += 0x7FFFFFED;
	}
	szCode[i] = '\0';
}

char* Base64::encode(char* str) {
	// get length
	int iLength = 0;
	while (str[iLength] != '\0') iLength++;
	// allocate buffer for base64 representation
	char* szCode = NULL;
	const int bitWidth = 6;
	if (iLength > 0) {
		//szCode = (char*)MemoryMgr::alloc(sizeof(char) * 2 * iLength + 1);
		NEWARR(char, szCode, 2*iLength + 1);
		//szCode = new char[sizeof(char) * 2 * iLength + 1];
		int bits = 0, i = 0, j = 0;
		DWORD acc = 0;
		while (str[i] != 0) {
			if (bits < 8) {
				acc |= ((BYTE)str[i++] << bits);
				bits += 8;
			}
			if (bits >= bitWidth) {
				// write data
				BYTE b = acc & ((1<<bitWidth) - 1);
				szCode[j++] = Base64::m_szCodeTable[b];
				acc >>= bitWidth;
				bits -= bitWidth;
			}
		}
		while (bits > 0 && acc != 0) {
			BYTE b = acc & ((1<<bitWidth) - 1);
			szCode[j++] = Base64::m_szCodeTable[b];
			acc >>= bitWidth;
			bits -= bitWidth;
		}
		szCode[j] = '\0';
	} else {
		//NEWBUF(szCode, 2);
		//szCode = (char*)MemoryMgr::alloc(sizeof(char) * 2);
		//szCode = new char[sizeof(char) * 2];
		NEWARR(char, szCode, 2);
		szCode[0] = 'A';
		szCode[1] = '\0';
	}
	return szCode;
}
char* Base64::encode(long long iCode) {
	// create base64 string from 64bit integer
	char szTmp[9];
	*(long long*)&szTmp = iCode;
	szTmp[8] = '\0';
	return Base64::encode((char*)&szTmp);
}

char* Base64::decode(char* szCode) {
	int iLength = 0;
	while (szCode[iLength] != '\0') iLength++;

	//char* NEWBUF(str, iLength);
	//char *szStr = (char*)MemoryMgr::alloc(iLength);
	//char *szStr = new char[iLength];
	char*NEWARR(char, szStr, iLength);
	int j = 0, i = 0, bits = 0;
	DWORD acc = 0;
	const int bitWidth = 6;
	while (szCode[i] != 0) {
		acc |= ((BYTE)(strchr(Base64::m_szCodeTable, szCode[i]) - Base64::m_szCodeTable))<<bits;
		bits += bitWidth;
		if (bits >= 8) {
			szStr[j++] = acc & 0x0ff;
			bits -= 8;
			acc >>= 8;
		}
		i++;
	}
	while (bits > 0) {
		szStr[j++] = acc & 0x0ff;
		bits -= 8;
		acc >>= 8;
	}
	szStr[j] = '\0';
//	char *pRet = strdup(str);
//	DEL(str);
	return szStr;
}

long long Base64::toInt64(char szCode[12])
{
	long long iCode = 0;
	char a = 0;
	int rb = 0;
	int i = 0, j = 0;
	while (szCode[i] != 0 && j < 8)
	{
		BYTE b = (BYTE)(strchr(Base64::m_szCodeTable, szCode[i]) - Base64::m_szCodeTable);
		a |= b<<rb;
		rb += 6;
		if (rb >= 8)
		{
			((byte*)&iCode)[j] += a;
			j++;
			rb -= 8;
			a = b>>(6 - rb);
		}
		i++;
	}
	((byte*)&iCode)[j] += a;
	return iCode;
}
