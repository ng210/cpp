//*****************************************************************************
//
//	STRING Functions
//	- strdup: string duplication (cloning)
//	- strncpy: safe copy of n characters
//	- strsplit: split string by delimiter
// define snprintf as _snprintf
//*****************************************************************************

#include "str.h"
#include "Collection/EArray.h"

//#pragma warning(disable:4273)


//*********************************************************
char* strdup_(const char *szStr)
{
	return strdup(szStr, strlen(szStr));
}
//*********************************************************
char* strdup(const char *szStr, size_t iLength)
{
	char* szRes = 0;
	if (iLength > 0)
	{
		NEWBUF(szRes, iLength+1);
		memcpy(szRes, (void*)szStr, iLength);
		szRes[iLength] = '\0';
	}
	return szRes;
}
//*********************************************************
char* strncpy(char *szDst, size_t iSize, const char *szSrc, size_t iLength)
{
	size_t iLen = iSize-1 > iLength ? iSize-1 : iLength;
	memcpy((void*)szDst, (void*)szSrc, iLen);
	szDst[iLen] = '\0';
	return szDst;
}
//*********************************************************
size_t strrspn(const char *szStr, const char *szControl)
{
	int iCount = strlen(szStr);
	while (iCount > 0 && strchr(szControl, szStr[--iCount]) != NULL);
	return iCount;
}
//*********************************************************
size_t strrcspn(const char *szStr, const char *szControl)
{
	int iCount = strlen(szStr);
	while (iCount > 0 && strchr(szControl, szStr[--iCount]) == NULL);
	return iCount;
}
//*********************************************************
IndexedCollection* strsplit(const char *szInput, const char *szDelimiter, const char *szOpen, const char *szClose)
{
	EArray* NEW(arr, EArray, 16);
	if (szInput != NULL)
	{
		char **pszArg;
		if (szDelimiter != NULL)
		{
			const char *p, *szStart;
			int iLength;
			int iMark = -1;
			int iOpenMod = (szOpen != NULL) ? strlen(szOpen) : 0;
			int iCloseMod = 0;
			if (szClose == NULL)
			{
				szClose = szOpen;
				iCloseMod = iOpenMod;
			}
			else
			{
				iCloseMod = strlen(szClose);
			}
			p = szStart = szInput;
			while (*p != '\0')
			{
				// skip delimiters
				szStart = p += strspn(p, szDelimiter);
				if (szOpen != NULL && (iMark = strchr(szOpen, *p) - szOpen) >= 0)
				{
					iLength = 0;
					while (*p != '\0')
					{
						// skip to the closing character
						const char *szEnd = strchr(p, szClose[iMark % iCloseMod]);
						if (szEnd == NULL)
						{
							iLength = strlen(p);
							p += iLength;
							break;
						}
						p = szEnd + 1;
						iLength = p - szStart;
						// if the next char is a delimiter, we found a token
						if (strchr(szDelimiter, *p) != 0)
						{
							break;
						}
					}
				}
				else
				{
					// skip to first delimiter
					p += iLength = strcspn(p, szDelimiter);
				}
				if (iLength > 0)
				{
					// add token to array
					pszArg = (char**)arr->add().ref;
					*pszArg = strdup(szStart, iLength);
				}
			}
		}
		else
		{
			pszArg = (char**)arr->add().ref;
			*pszArg = strdup(szInput);
		}
	}
	return (IndexedCollection*)arr;
}
//*********************************************************
//*********************************************************
int atoi(char *szStr, int iBase, int *piValue)
{
	unsigned int iRet = 0;
	*piValue = 0;
	if (szStr != NULL)
	{
		int bSign = 0;
		if (szStr[iRet] == '-')
		{
			bSign = 1;
			iRet++;
		}
		for (; iRet<strlen(szStr); iRet++)
		{
			int d = szStr[iRet];
			d -= (d >= '0' && d <= '9') ? '0' : 'A'-10;
			if (d >= 0 && d < iBase)
			{
				*piValue *= iBase;
				*piValue += d;
			}
			else
			{
				break;
			}
		}
		if (bSign)
		{
			*piValue = -*piValue;
		}
	}
	return iRet;
}

//#pragma warning(default:4273)