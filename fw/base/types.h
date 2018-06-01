typedef signed char         INT8, *PINT8;
typedef signed short        INT16, *PINT16;
typedef signed int          INT32, *PINT32;
typedef signed __int64      INT64, *PINT64;
typedef unsigned char       UINT8, *PUINT8;
typedef unsigned short      UINT16, *PUINT16;
typedef unsigned int        UINT32, *PUINT32;
typedef unsigned __int64    UINT64, *PUINT64;
//#define UINT8 UINT8
//#define byte UINT8
//#define WORD UINT16
//#define word WORD
//#define DWORD UINT32
//#define dword DWORD
//#define QWORD UINT64
//#define qword QWORD

#ifdef UNICODE
#include <wchar.h>
#define TCHAR wchar_t
#else
#define TCHAR char
#endif
