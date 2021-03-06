#ifndef __PLAYER_H
#define __PLAYER_H

#include "basedef.h"
#include "collection/array.h"

NS_FW_BASE_USE

#include "player.h"
#include "channel.h"

#define NS_PLAYER ply
#define NS_PLAYER_BEGIN namespace NS_PLAYER {
#define NS_PLAYER_END }
#define NS_PLAYER_USE using namespace NS_PLAYER;

NS_PLAYER_BEGIN

class Channel;
/******************************************************************************
* Base player class
*****************************************************************************/
class Player : public AbstractPlayer {
//protected:	AbstractAdapter* adapter_;

	//// number of cycles per second
	//int m_iCyclesPerSecond;
	//// number of cycles per frame
	//float m_fCyclesPerFrame;
	//// internal counter of cycles
	//float m_fCycles;
	//// array of targets (accessed via adapters)
	//EArray *m_aTargets;
	//// array of channels
	//EArray *m_aChannels;
	//// array of input data
	//PROP_R(protected, EArray*, Inputs, pa);
	//// master channel
	//PLAYERCHANNEL *m_pMasterChannel;
public:
	Player();
	virtual ~Player();

	//void addSequence(Array* sequence);
	void addSequence(void* stream);
	void run(size_t ticks);

	// AbstractAdapter interface
	int prepareObject(void* target);
	int processCommand(void* target, PLAYER_COMMAND* cmd);
	size_t getCurrentFrame();

	//// return status of player
	//inline int getStatus() { return m_pMasterChannel->m_status; }
	//// add target
	//void setTarget(PlayerAdapter* pAdapter);
	//void setTarget(size_t iTarget, PlayerAdapter* pAdapter);
	//// set channel
	//void setChannel(size_t iTarget, size_t iStream, char cStatus);
	//// seek along timeline (cycle count)
	//inline void seek(float fCycles) { m_fCycles += fCycles; }
	//// return actual temp (cycles per frame value)
	//inline float getCyclesPerFrame() { return m_fCyclesPerFrame; }
	//// return master frame count
	//inline int getMasterCounter() { return m_pMasterChannel->m_iCurrentFrame; }
	//// run the playback
	//virtual int runChannel(PLAYERCHANNEL *pCh) = 0;
	//virtual int run(float fCycles);
};

NS_PLAYER_END

///******************************************************************************
// * Stream input
// *****************************************************************************/
//typedef union LPPLAYERSTREAM {
//	unsigned char*		db;
//	unsigned short int*	dw;
//	unsigned int*		dd;
//} LPPLAYERSTREAM;
//
///******************************************************************************
// * Array of commands input
// *****************************************************************************/
//typedef struct PLAYERCOMMAND {
//	int delta;
//	int cmd;
//	int arg1, arg2, arg3, arg4;
//} PLAYERCOMMAND;
//
///******************************************************************************
// * Unified player input
// *****************************************************************************/
//typedef union PLAYERINPUT {
//	LPPLAYERSTREAM stream;
//	PLAYERCOMMAND *cmd;
//} PLAYERINPUT;
//
//class PlayerAdapter;
///******************************************************************************
// * Channel of playback
// *****************************************************************************/
//struct PLAYERCHANNEL {
//	// channel id
//	//int m_id;
//	// target id
//	size_t m_iTarget;
//	// start of assigned stream
//	PLAYERINPUT m_pStart;
//	// current position in the stream
//	PLAYERINPUT m_pCursor;
//	// status of channel
//	int m_status;
//	// number of current frame
//	int m_iCurrentFrame;
//	// assigned player adapter object
//	PlayerAdapter *m_pAdapter;
//};
///******************************************************************************
// * Player Adapter "interface"
// *****************************************************************************/
//class PlayerAdapter {
//public:
//	//virtual int processCommand(int iFrame, PLAYERCHANNEL *pChn, PLAYERINPUT& pInput) = 0;
//	virtual int processCommand(PLAYERCHANNEL *pChn) = 0;
//};

//
///******************************************************************************
// * Player class for stream inputs
// *****************************************************************************/
//class StreamPlayer : public Player {
//public:
//	StreamPlayer(int iCyclesPerSecond, LPPLAYERSTREAM pStream, int status);
//	~StreamPlayer();
//	//int processCommand(int iFrame, PLAYERCHANNEL *pChn, PLAYERINPUT& pInput);
//	int processCommand(PLAYERCHANNEL *pChn);
//	int runChannel(PLAYERCHANNEL *pCh);
//};
//
///******************************************************************************
// * Player class for array of commands inputs
// *****************************************************************************/
//class CommandPlayer : public Player {
//public:
//	CommandPlayer(int iCyclesPerSecond, int iNumberOfChannels, int iInputs, PLAYERCOMMAND* pInput, int status);
//	~CommandPlayer();
//	//int processCommand(int iFrame, PLAYERCHANNEL *pChn, PLAYERINPUT& pInput);
//	int processCommand(PLAYERCHANNEL *pChn);
//	int runChannel(PLAYERCHANNEL *pCh);
//};

#endif