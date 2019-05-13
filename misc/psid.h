/*
 * PSid.h
 *
 *  Created on: 2014.05.23.
 *      Author: Gabor
 */

#include "types.h"

#ifndef PSID_H_
#define PSID_H_

typedef struct _PSidHeader {
	char magicId[4];	// 0x00
	WORD version;		// 0x04
	WORD dataOffset;	// 0x06
	WORD loadAddr;		// 0x08
	WORD initAddr;		// 0x0a
	WORD playAddr;		// 0x0c
	WORD songs;			// 0x0e
	WORD startSong;		// 0x10
	BYTE speed[4];		// 0x12
	char str1[32];		// 0x16
	char str2[32];		// 0x36
	char str3[32];		// 0x56
	WORD flags;			// 0x76
	DWORD reserved;		// 0x78

	WORD dataLength;	// 0x7c
} PSidHeader;

#define PSIDHEADERSIZE ( offsetof(_PSidHeader, dataLength) )

#endif
