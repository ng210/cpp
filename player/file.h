#ifndef __PLAYER_FILE_H
#define __PLAYER_FILE_H

//#include "collection/array.h"
#include "playerdef.h"

//NS_FW_BASE_USE
NS_PLAYER_BEGIN

// ASU: adapter/sequence/user data file format
// OFFSET		SIZE	VALUE
// 0000			UINT8	count of adapter IDs
// +1			UINT8	1st adapter ID
// +n			UINT8	nth adapter ID
// n+1			UINT16	count of sequences
// +2			UINT16	length of the 1st sequence
// +m*2			UINT16	length of the mth sequence
// n+1+m*2+2	UINT16	count of user data blocks
// +2			UINT32	length of the 1st user data block
// +p*2			UINT32	length of the pth user data block

NS_PLAYER_END

#endif