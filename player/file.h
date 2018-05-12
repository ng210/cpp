#ifndef __PLAYER_FILE_H
#define __PLAYER_FILE_H

//#include "collection/array.h"
#include "playerdef.h"

//NS_FW_BASE_USE
NS_PLAYER_BEGIN

// ASU: adapter/sequence/user data file format
// OFFSET		SIZE	VALUE
// 0000			byte	count of adapter IDs
// +1			byte	1st adapter ID
// +n			byte	nth adapter ID
// n+1			word	count of sequences
// +2			word	length of the 1st sequence
// +m*2			word	length of the mth sequence
// n+1+m*2+2	word	count of user data blocks
// +2			dword	length of the 1st user data block
// +p*2			dword	length of the pth user data block

NS_PLAYER_END

#endif