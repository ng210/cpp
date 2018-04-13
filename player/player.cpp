#include "base/memory.h"
#include "Channel.h"
#include "Player.h"

NS_PLAYER_BEGIN

Player::Player() : targets_(sizeof(Target)), channels_(sizeof(Channel)) {
	Target* target = NEW_(Target, this, this);
	targets_.add(target);
	masterChannel_ = NULL;
	framesPerSecond_ = 25;
	ticksPerFrame_ = 1;
}
Player::~Player() {
	ARRAY_FOREACH(&targets_, DEL_((Target*)value));
	ARRAY_FOREACH(&channels_, DEL_((Channel*)value));
	ARRAY_FOREACH(&sequences_, FREE(value));
}

void Player::addTarget(void* object, AbstractAdapter* adapter) {
	targets_.add(NEW_(Target, object, adapter));
	adapter->prepareObject(object);
}
//void Player::addSequence(Array* sequence) {
//	sequences_->push(sequence);
//	if (sequences_->length() == 1) {
//		// the very first sequence is assigned to the master channel
//		Target* target = NEW_(Target, this, this);
//		targets_->push(target);
//		masterChannel_ = NEW_(Channel, this, target, sequence);
//		masterChannel_->id_ = "master";
//		masterChannel_->setActive();
//		masterChannel_->setLooping();
//		channels_->push(masterChannel_);
//	}
//
//}
void Player::addSequence(unsigned char* stream) {
	// create sequence as Array
	PArray* sequence = NEW_(PArray);
	unsigned short delta = 0;
	int cmd = 0;
	unsigned char* args = NULL;
	size_t i = 0;
	PLAYER_COMMAND_U ptr;
	ptr.p = stream;
	while (true) {
		sequence->add(ptr.p);
		cmd = ptr.s->code;
		if (cmd == (unsigned char)Player_Cmd_end) {
			break;
		}
		ptr.p += offsetof(PLAYER_COMMAND, args) + ptr.s->argc;
	}
	sequences_.add(sequence);

	if (sequences_.length() == 1) {
		// the very first sequence is assigned to the master channel
		Target* target = (Target*)targets_.getAt(0);
		masterChannel_ = NEW_(Channel, this, 0, target, sequence);
		masterChannel_->setActive();
		masterChannel_->setLooping();
		channels_.add(masterChannel_);
	}
}
void Player::run(size_t ticks) {
	// run master channel
	if (masterChannel_->isActive()) {
		masterChannel_->run(ticks);
	}
	if (masterChannel_->isActive()) {
		// if master is still active run every other channel
		for (int i = 1; i < (int)channels_.length(); i++) {
			Channel* chn = (Channel*)channels_.getAt(i);
			if (chn->isActive()) {
				chn->run(ticks);
			}
		}
	}
}

// Adapter

int Player::prepareObject(void* object) {
	return 0;
}

int Player::processCommand(void* object, PLAYER_COMMAND* command) {
	Player* player = (Player*)object;
	unsigned char* args = &command->args;
	switch (command->code) {
	case Player_Cmd_setTempo:
		player->framesPerSecond_ = args[0];
		player->ticksPerFrame_ = args[1];
		player->refreshRate_ = player->framesPerSecond_ * player->ticksPerFrame_;
		break;
	case Player_Cmd_assign:
		Target* target = (Target*)player->targets_.getAt(args[0]);
		PArray* sequence = (PArray*)player->sequences_.getAt(args[1]);
		size_t status = args[2];
		Channel* chn = NULL;
		// get an inactive channel
		size_t ix = -1;
		for (int i = 0; i < (int)player->channels_.length(); i++) {
			chn = (Channel*)player->channels_.getAt(i);
			if (!chn->isActive()) {
				ix = i;
				break;
			}
		}
		if (ix == -1) {
			// create new channel
			ix = player->channels_.length();
			chn = NEW_(Channel, this, ix, target, sequence);
			//chn->id_ = NEW_(String, ix);
			player->channels_.add(chn);
		}
		// assign channel
		chn->set(player, ix, target, sequence, status & 0x80);
		chn->setLooping(status & 0x7f);
		break;
	}
	return 0;
}

size_t Player::getCurrentFrame() {
	return masterChannel_->tick();
}
NS_PLAYER_END

//#include "Base/mem.h"
//#include "Player.h"
//
//Player::Player() {
//	// create array of target references
//	NEW(m_aTargets, EArray, 16);
//	// add this player object as the target at index 0
//	setTarget(0, this);
//	// create array of channel structures
//	NEW(m_aChannels, EArray, sizeof(PLAYERCHANNEL), 16);
//	// add master channel at index 0
//	COLLECTIONITEM item = m_aChannels->add();
//	m_pMasterChannel = (PLAYERCHANNEL*)item.p;
//	//memset(m_pMasterChannel, 0, sizeof(PLAYERCHANNEL));
//	// master channel is interpreted by this player
//	m_pMasterChannel->m_pAdapter = this;
//	//m_pMasterChannel->m_id = 0;
//}
//
//Player::~Player() {
//	_VS_debug("Player::~Player\n");
//	DEL(m_aChannels);
//	DEL(m_aTargets);
//	DEL(m_paInputs);
//}
//
//
//void Player::setTarget(PlayerAdapter* pAdapter) {
//	COLLECTIONITEM item = m_aTargets->add();
//	// set target and adapter
//	*item.ref = pAdapter;
//}
//
//void Player::setTarget(size_t iTarget, PlayerAdapter* pAdapter) {
//	// add targets if necessary
//	while (m_aTargets->Count() < iTarget) {
//		m_aTargets->add();
//	}
//	// set target and adapter
//	m_aTargets->setAt(iTarget, pAdapter);
//
///*
//	if (iTarget < m_aTargets->Count())
//	{
//		// set target and adapter
//		// m_aTargets[iTarget] = pAdapter;
//		m_aTargets->setAt(iTarget, pAdapter);
//	}
//	else
//	{
//		// add targets
//		for (size_t i=m_aTargets->Count(); i<iTarget; i++)
//		{
//			m_aTargets->add();
//		}
//		// set adapter
//		m_aTargets->addObj(pAdapter, sizeof(PlayerAdapter*));
//	}
//*/
//}
//void Player::setChannel(size_t iTarget, size_t iStream, char cStatus) {
//	//int id = m_aChannels->Count();
//	COLLECTIONITEM item = m_aChannels->add();
//	PLAYERCHANNEL *pCh = (PLAYERCHANNEL*)item.p;
//	//pCh->m_id = id;
//	pCh->m_iTarget = iTarget;
//	pCh->m_pAdapter = (PlayerAdapter*)m_aTargets->getAt(iTarget);
//	pCh->m_iCurrentFrame = 0;
//	pCh->m_pStart.stream.dd = pCh->m_pCursor.stream.dd = (unsigned int*)m_paInputs->getAt(iStream);
//	pCh->m_status = cStatus;
//	pCh->m_status |= Player_Flg_Active;
//}
//
//int Player::run(float fCycles) {
//	// result = 0 indicates no processing
//	int iResult = 0;
//	// if master channel is active, player is active
//	if (m_pMasterChannel->m_status & Player_Flg_Active)	{
//		// advance cycle counter
//		m_fCycles += fCycles;
//		// next frame reached?
//		if (m_fCycles < m_fCyclesPerFrame) {
//_End:
//			return iResult;
//		}
//		// reset counter
//		m_fCycles -= m_fCyclesPerFrame;
//		// run master channel
//		runChannel(m_pMasterChannel);
//		// is the master channel still active?
//		if (m_pMasterChannel->m_status & Player_Flg_Active) {
//			// run the other channels
//			Iterator *it = m_aChannels->begin(1);
//			while (m_aChannels->next(it)) {
//				PLAYERCHANNEL *pChn = (PLAYERCHANNEL*)it->pItem;
//				// include active channels only
//				if (pChn->m_status & Player_Flg_Active) {
//					runChannel(pChn);
//				}
//			}
//			m_aChannels->end(it);
//			//for (int i=1; i<m_iChannels; i++)
//			//{
//			//	PLAYERCHANNEL *pChn = &m_aChannels[i];
//			//	// ignore inactive channels
//			//	if (pChn->m_status & Player_Flg_Active)
//			//	{
//			//		runChannel(pChn);
//			//	}
//			//}
//		}
//		// result = 1 indicates a processed frame
//		iResult = 1;
//	}
//	goto _End;
//}
//
//
//// Derived Player classes
//StreamPlayer::StreamPlayer(int iCyclesPerSecond, LPPLAYERSTREAM pInput, int status) {
//	// store cycles per second value
//	m_iCyclesPerSecond = iCyclesPerSecond;
//	// clear attributes
//	m_fCyclesPerFrame = m_fCycles = 0;
//
//	// process input stream
//	LPPLAYERSTREAM pStream = pInput;
//	if (pInput.dd != 0) {
//		// 00 02 number of targets
//		int iTargets = *pStream.dw++;
//		while (iTargets-- > 0) {
//			m_aTargets->add();
//		}
//		// 02 02 number of streams
//		int iInputs = *pStream.dw++;
//		// calculate absolute addresses from stream offsets
//		// skip array of pointers: m_iInputs * sizeof(dw)
//		int iOffset = (int)&pStream.dw[iInputs];
//		
//		while (iInputs-- > 0) {
//			iOffset += *pStream.dw;
//			m_paInputs->addObj((void*)iOffset, sizeof(void*));
//			pStream.dw++;
//		}
//		//for (int i=0; i<iInputs; i++)
//		//{
//		//	iOffset += *pStream.dw;
//		//	m_paInputs[i].stream.dd = (unsigned int*)iOffset;
//		//	pStream.dw++;
//		//}
//
//		// assign the first sequence to the master channel
//		m_pMasterChannel->m_pStart.stream.dd = m_pMasterChannel->m_pCursor.stream.dd = (unsigned int*)m_paInputs->getAt(0);
//		m_pMasterChannel->m_status = status;
//	}
//}
//StreamPlayer::~StreamPlayer() {
//	_VS_debug("StreamPlayer::~StreamPlayer\n");
//}
//int StreamPlayer::runChannel(PLAYERCHANNEL *pCh) {
//_Start:
//	int iResult = 0;
//	// check frame counter
//	if (pCh->m_iCurrentFrame < *pCh->m_pCursor.stream.dw)
//	{
//_End:
//		// advance frame
//		pCh->m_iCurrentFrame++;
//		return iResult;
//	}
//	// process commands
//	iResult = 1;
//	pCh->m_pCursor.stream.dw++;
//	int iLastCmd;
//	do
//	{
//		// get command
//		char iCmd = *pCh->m_pCursor.stream.db;
//		// check end of sequence
//		if (iCmd != Player_Cmd_End)
//		{
//			// MSB of command marks last command in the frame
//			iLastCmd = iCmd & 0x80;
//			//iCmd &= 0x7f;
//			//pCh->m_pCursor.stream.db++;
//			//iResult = pCh->m_pAdapter->processCommand(pCh->m_iCurrentFrame, pCh, pCh->m_pCursor);
//			iResult = pCh->m_pAdapter->processCommand(pCh);
//		}
//		else
//		{
//#ifdef _DEBUG
//	printf("Fr:$%x, Ch:$%x, Cmd: end(%d)\n", pCh->m_iCurrentFrame, pCh, pCh->m_status & Player_Flg_Looping);
//#endif
//			// end of sequence reached
//			if (pCh->m_status & Player_Flg_Looping)
//			{
//				if (pCh == m_pMasterChannel)
//				{
//					// remove all other channels
//					while (m_aChannels->Count() > 1)
//					{
//						m_aChannels->removeAt(1);
//					}
//				}
//				// looping was enabled, the sequence restarts
//				pCh->m_pCursor = pCh->m_pStart;
//				// process frame #0 immediately
//				pCh->m_iCurrentFrame = 0;	//-1;
//				goto _Start;
//			}
//			else
//			{
//				// set channel inactive
//				m_aChannels->remove(pCh);
//				//pCh->m_status &= ~Player_Flg_Active;
//			}
//			goto _End;
//		}
//	}
//	while (!iLastCmd);
//	// restart frame counting
//	pCh->m_iCurrentFrame = 0;
//	goto _End;
//}
////int StreamPlayer::processCommand(int iFrame, PLAYERCHANNEL *pCh, PLAYERINPUT& pInput)
//int StreamPlayer::processCommand(PLAYERCHANNEL *pCh)
//{
//	PLAYERINPUT& pInput = pCh->m_pCursor;
//	LPPLAYERSTREAM& pCursor = pInput.stream;
//	int iResult = 0;
//	int iTempo, iTarget, iStream;
//	char cStatus;
//
//#ifdef _DEBUG
//	printf("Fr:$%x, Ch:$%x, ", pCh->m_iCurrentFrame, pCh);
//#endif
//
//	int iCmd = *pCursor.db++;
//	iCmd &= 0x7f;
//	switch (iCmd)
//	{
//		case Player_Cmd_setTempo:
//			// 00 02 frames per second
//			// cycles per frame = cycles per sec / frame per sec
//			iTempo = *pCursor.dw++;
//#ifdef _DEBUG
//	printf("Cmd: setTempo($%x)\n", iTempo);
//#endif
//			m_fCyclesPerFrame = (float)m_iCyclesPerSecond / iTempo;
//			break;
//
//		case Player_Cmd_assign:
//			// 00 02 channel id
//			// 02 02 sequence id
//			// 04 01 status
//			iTarget = *pCursor.dw++;
//			iStream = *pCursor.dw++;
//			cStatus = *pCursor.db++;
//#ifdef _DEBUG
//	printf("Cmd: assign($%x,$%x,$%x)\n", iTarget, iStream, cStatus);
//#endif
//			setChannel(iTarget, iStream, cStatus);
//			break;
//		default:
//#ifdef _DEBUG
//			printf("Cmd: $%x?\n", iCmd);
//#endif
//			break;
//	}
//	return iResult;
//}
//
//CommandPlayer::CommandPlayer(int iCyclesPerSecond, int iNumberOfTargets, int iInput, PLAYERCOMMAND *pInput, int status)
//{
//	// store cycles per second value
//	m_iCyclesPerSecond = iCyclesPerSecond;
//	// clear attributes
//	m_fCyclesPerFrame = m_fCycles = 0;
//	// set main input
//	//m_pInput.cmd = paCmd;
//	// default channel count is 1: master channel
//	int iTargets = iNumberOfTargets;
//
//	// add targets
//	while (iTargets-- > 0)
//	{
//		m_aTargets->add();
//	}
//	//NEWTBUF(m_aChannels, PLAYERCHANNEL, m_iChannels);
//	//// #0 is the master channel, its adapter is this Player instance
//	//setChannelAdapter(0, this);
//
//	// read input command arrays
//	NEW(m_paInputs, EArray, iInput);
//	for (int i=0; i<iInput; i++)
//	{
//		m_paInputs->addObj(pInput, sizeof(PLAYERCOMMAND*));
//		while (pInput->cmd != -1) pInput++;
//		pInput++;
//	}
//
//	// assign the first sequence to the master channel
//	m_pMasterChannel->m_pStart.cmd = m_pMasterChannel->m_pCursor.cmd = (PLAYERCOMMAND*)m_paInputs->getAt(0);
//	m_pMasterChannel->m_status = status;
//}
//
//CommandPlayer::~CommandPlayer() {
//	_VS_debug("CommandPlayer::~CommandPlayer\n");
//}
//int CommandPlayer::runChannel(PLAYERCHANNEL *pCh)
//{
//_Start:
//	int iResult = 0;
//	// check frame counter
//	if (pCh->m_iCurrentFrame < pCh->m_pCursor.cmd->delta)
//	{
//_End:
//		// advance frame
//		pCh->m_iCurrentFrame++;
//		return iResult;
//	}
//	// process commands
//	iResult = 1;
//	//int iLastCmd;
//	do
//	{
//		// get command
//		PLAYERCOMMAND *pCmd = pCh->m_pCursor.cmd;
//		char iCmd = pCmd->cmd;
//		// check end of sequence
//		if (iCmd != Player_Cmd_End)
//		{
//			//iResult = pCh->m_pAdapter->processCommand(pCh->m_iCurrentFrame, pCh, (PLAYERINPUT&)pCmd);
//			iResult = pCh->m_pAdapter->processCommand(pCh);
//		}
//		else
//		{
//#ifdef _DEBUG
//	printf("Fr:$%x, Ch:$%x, Cmd: end(%d)\n", pCh->m_iCurrentFrame, pCh, pCh->m_status & Player_Flg_Looping);
//#endif
//			// end of sequence reached
//			if (pCh->m_status & Player_Flg_Looping)
//			{
//				if (pCh == m_pMasterChannel)
//				{
//					// remove all other channels
//					while (m_aChannels->Count() > 1)
//					{
//						m_aChannels->removeAt(1);
//					}
//				}
//				// restart the channel
//				pCh->m_pCursor = pCh->m_pStart;
//				// process frame #0 immediately
//				pCh->m_iCurrentFrame = 0;	//-1;
//				goto _Start;
//			}
//			else
//			{
//				if (pCh != m_pMasterChannel) {
//					// remove channel
//					m_aChannels->remove(pCh);
//				} else {
//					// set channel inactive
//					pCh->m_status &= ~Player_Flg_Active;
//				}
//			}
//			goto _End;
//		}
//		pCh->m_pCursor.cmd++;
//	}
//	while (pCh->m_pCursor.cmd->delta == 0);
//	//while (!iLastCmd);
//	// restart frame counting
//	pCh->m_iCurrentFrame = 0;
//	goto _End;
//}
//
////int CommandPlayer::processCommand(int iFrame, PLAYERCHANNEL *pCh, PLAYERINPUT& pInput)
//int CommandPlayer::processCommand(PLAYERCHANNEL *pCh)
//{
//	PLAYERINPUT& pInput = pCh->m_pCursor;
//	PLAYERCOMMAND*& pCursor = pInput.cmd;
//	int iResult = 0;
//	int iTempo, iTick, iTarget, iStream;
//	char cStatus;
//
//#ifdef _DEBUG
//	printf("Fr:$%x, Ch:$%x, ", pCh->m_iCurrentFrame, pCh);
//#endif
//
//	int iCmd = pCursor->cmd;
//	iCmd &= 0x7f;
//	switch (iCmd)
//	{
//		case Player_Cmd_setTempo:
//			// 00 02 frames per second
//			// cycles per frame = cycles per sec / frame per sec
//			iTempo = pCursor->arg1;
//			iTick = pCursor->arg2;
//#ifdef _DEBUG
//	printf("Cmd: setTempo($%x bpm)\n", iTempo);
//#endif
//			m_fCyclesPerFrame = (float)m_iCyclesPerSecond * 60 / iTempo;
//			if (iTick > 0) m_fCyclesPerFrame /= iTick;
//			break;
//
//		case Player_Cmd_assign:
//			// 00 02 channel id
//			// 02 02 sequence id
//			// 04 01 status
//			iTarget = pCursor->arg1;
//			iStream = pCursor->arg2;
//			cStatus = pCursor->arg3;
//#ifdef _DEBUG
//	printf("Cmd: assign($%x,$%x,$%x)\n", iTarget, iStream, cStatus);
//#endif
//			setChannel(iTarget, iStream, cStatus);
//			break;
//		default:
//#ifdef _DEBUG
//			printf("Cmd: $%x?\n", iCmd);
//#endif
//			break;
//	}
//	return iResult;
//}
//
