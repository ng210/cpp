#ifndef INT8
typedef signed char         INT8, *PINT8;
#endif
#ifndef INT16
typedef signed short        INT16, *PINT16;
#endif
#ifndef INT32
typedef signed int          INT32, *PINT32;
#endif
#ifndef INT64
typedef signed __int64      INT64, *PINT64;
#endif
#ifndef UINT8
typedef unsigned char       UINT8, *PUINT8;
#endif
#ifndef UINT16
typedef unsigned short      UINT16, *PUINT16;
#endif
#ifndef UINT32
typedef unsigned int        UINT32, *PUINT32;
#endif
#ifndef UINT64
typedef unsigned __int64    UINT64, *PUINT64;
#endif
#ifndef HANDLE
typedef void* HANDLE;
#endif

#ifndef UINT8
#define UINT8 UINT8
#endif
#ifndef BYTE
#define BYTE UINT8
#endif
#ifndef byte
#define byte BYTE
#endif
#ifndef WORD
#define WORD UINT16
#endif
#ifndef word
#define word WORD
#endif
#ifndef DWORD
#define DWORD unsigned long
#endif
#ifndef dword
#define dword DWORD
#endif
#ifndef QWORD
#define QWORD UINT64
#endif
#ifndef qword
#define qword QWORD
#endif



#ifdef UNICODE
#include <wchar.h>
#define TCHAR wchar_t
#else
#define TCHAR char
#endif
