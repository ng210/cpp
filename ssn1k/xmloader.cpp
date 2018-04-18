#include "xmloader.h"
#include "base/memory.h"
#include "utils/file.h"
#include <stdio.h>
#include "player.h"

NS_PLAYER_USE

XmLoader::XmLoader() {
}

XmLoader::~XmLoader(void) {
}

void XmLoader::readRow(XMNOTE* note, BYTE*& data) {	// int iFrame, CHANNELINFO &chInfo, XMSTREAM *pStream) {
	BYTE code = *data++;
	bool isCompressed = code & 0x80;
	if (!isCompressed) {
		note->note = code;
		code = 0x9e;
	}
	code &= 0x1f;
	if (code & 0x01) {	// note
		note->note = *data++;
	}
	if (code & 0x02) {	// instrument
		note->instrument = *data++;
	}
	if (code & 0x04) {	// volume
		note->volume = *data++;

	}
	if (code & 0x08) {	// effect
		note->effect = *data++;
		//note->parameter = *data++;
	}
	if (code & 0x10) {	// parameter
		note->parameter = *data++;
	}

	//int note, velocity = 0x7F, fx, fxArg;
	//// get note byte
	//char c = *data++;
	//// default mask
	//int mask = 0x1E;
	//// if MSB set
	//if (c & 0x80) {
	//	// mask taken from note
	//	mask = c & 0x1F;
	//	if (mask & 1) {
	//		note = *data++;
	//	}
	//}
	//else
	//{
	//	note = c;
	//	mask |= 1;
	//}
	//// there is data
	//if (mask != 0)
	//{
	//	int iDelta = iFrame - chInfo.iFrame;
	//	chInfo.iFrame = iFrame;
	//	// there is note data
	//	if (mask & 1)
	//	{
	//		// note = 96 means note off
	//		if (note != 0x61)
	//		{
	//			note--;
	//			velocity = 0x7F;
	//		}
	//		else
	//		{
	//			note = chInfo.note;
	//			velocity = 0;
	//		}
	//	}
	//	// there is instrument data
	//	if (mask & 2)
	//	{
	//		int instrument = *data++;
	//		if (chInfo.instrument != instrument)
	//		{
	//			// write chg.prog event
	//			addEvent(pStream, iDelta, SYN_Player_Cmd_prgChange, instrument, 0, 0, 0);
	//			chInfo.instrument = instrument;
	//			iDelta = 0;
	//		}
	//	}
	//	if (mask & 4)
	//	{
	//		int volume = *data++;
	//		if (chInfo.ctrls[SYN_CI_Amp] != volume)
	//		{
	//			// write volume event
	//			int ctrlId, ctrlValue, cmd;
	//			XMPlayer::mapVol2Ctrl(volume, cmd, ctrlId, ctrlValue);
	//			if (cmd != -1 && (cmd != SYN_Player_Cmd_setCtrl || chInfo.ctrlId != ctrlId || chInfo.ctrls[ctrlId] != ctrlValue))
	//			{
	//				addEvent(pStream, iDelta, cmd, ctrlId, ctrlValue, 0, 0);
	//				chInfo.ctrlId = ctrlId;
	//				iDelta = 0;
	//			}
	//		}
	//	}
	//	if (mask & 8)
	//	{
	//		fx = *data++;
	//	}
	//	if (mask & 16)
	//	{
	//		fxArg = *data++;
	//	}
	//	if (mask & 24)
	//	{
	//		// write fx event
	//			int ctrlId, ctrlValue, cmd;
	//			XMPlayer::mapFx2Ctrl(fx, fxArg, cmd, ctrlId, ctrlValue);
	//			if (cmd != -1 && (cmd != SYN_Player_Cmd_setCtrl || chInfo.ctrlId != ctrlId || chInfo.ctrls[ctrlId] != ctrlValue))
	//			{
	//				addEvent(pStream, iDelta, cmd, ctrlId, ctrlValue, 0, 0);
	//				chInfo.ctrlId = ctrlId;
	//				iDelta = 0;
	//			}
	//	}

	//	if (mask & 1)
	//	{
	//		// note on possible
	//		if (1)
	//		{
	//			addEvent(pStream, iDelta, SYN_Player_Cmd_setNote, note, velocity, 0, 0);
	//			chInfo.note = note;
	//			chInfo.velocity = velocity;
	//			iDelta = 0;
	//		}
	//		else
	//		{
	//			addEvent(pStream, iDelta, SYN_Player_Cmd_setCtrl, note, 0, 0, 0);
	//			chInfo.ctrls[SYN_CI_Pitch] = note;
	//			chInfo.note = note;
	//			iDelta = 0;
	//		}
	//	}
	//}
}

int XmLoader::load(char *szPath) {
	int res = 1;
	BYTE* xm = File::read(szPath);
	if (xm != NULL) {
		res = 0;
		XMFILEHEADER *header = (XMFILEHEADER*)xm;
		tempo_ = header->defaultBPM / 60 * header->defaultTempo / 6;
		// channels: create targets
		int channelCount = header->channelCount;
		// convert patterns into sequences
		BYTE* ptr = xm + header->headerSize + offsetof(XMFILEHEADER, headerSize);
		//XMNOTE xmNotes[256];
		PArray* sequences[256];
		for (int p = 0; p < header->patternCount; p++) {
			PArray* pattern = NEW_(PArray);
			for (int ch = 0; ch < header->channelCount; ch++) {
				//xmNotes[ch].note = 96;
				//xmNotes[ch].instrument = 0;
				//xmNotes[ch].volume = 100;
				//xmNotes[ch].effect = 0;
				//xmNotes[ch].parameter = 0;
				sequences[ch] = NEW_(PArray);
				sequences_->add(sequences[ch]);
				PLAYER_COMMAND_U cmd;
				cmd.p = 0, Player_Cmd_assign, ch, sequences_->length(), Player_Flg_Active;
				pattern->add(&cmd);
			}

			XMFILEPATTERN* xmPattern = (XMFILEPATTERN*)ptr;
			ptr += offsetof(XMFILEPATTERN, packedData);
			//Array* pattern = NEW_(Array, sizeof(XMNOTE));

			for (int r = 0; r < xmPattern->rowCount; r++) {
				printf("%02x: ", r);
				for (int ch = 0; ch < header->channelCount; ch++) {
					BYTE* tmp = ptr;
					BYTE code = *ptr;
					//XMNOTE& xmNote = xmNotes[ch];
					//readRow(&xmNote, ptr);
					//addEvent(sequences[ch], ptr);
					//printf("[%02x %02x v%02x %02x:%02x (%2d)]", xmNote.note, xmNote.instrument, xmNote.volume, xmNote.effect, xmNote.parameter, (int)(ptr - tmp));
					//pattern->add(&xmNote);
				}
				//printf("\n");
			}
			//printf("--------\n");
			//addSequence(pattern);
		}
		res = 0;
		FREE(xm);
	}
	return res;
}

//void XMPlayer::reset()
//{
//	m_fCycles = 0;
//	m_status = Player_Flg_Active;
//}
//int XMPlayer::run(float fCycles)
//{
//	int iResult = 0;
//	if (m_status & Player_Flg_Active)
//	{
//		if (m_fCycles > 0)
//		{
//			m_fCycles -= fCycles;
//_End:
//			return iResult;
//		}
//		m_fCycles += m_fCyclesPerFrame;
//		// read next line
//		while (m_iCurrentStep < (int)m_pCurrentStream->Count())
//		{
//			SYNCOMMAND* pCmd = (SYNCOMMAND*)m_pCurrentStream->get(m_iCurrentStep);
//			if (pCmd->delta == m_iDelta)
//			{
//				m_iCurrentStep++;
//				processCommand(m_iFrame, 0, 0, (void*&)pCmd);
//				iResult = 1;
//				m_iDelta = 0;
//			}
//			else
//			{
//				m_iFrame++;
//				m_iDelta++;
//				goto _End;
//			}
//		}
//		m_status &= ~Player_Flg_Active;
//	}
//	goto _End;
//}
//
//int XMPlayer::render(int iCount)
//{
//	return SynPlayer::run();
//}
//
//int XMPlayer::processCommand(int iFrame, int iCh, int iCmd, void*& pData)
//{
//	SYNCOMMAND*& pCmd = (SYNCOMMAND*&)pData;
//	int iResult = 0;
//#ifdef _DEBUG
//	printf("Fr:$%x, Ch:$%x, ", iFrame, iCh);
//#endif
//	char cNote, cGate, cCtrlId, cCtrlValue;
//	Synth& synth = aSynths[iCh];
//	switch (pCmd->cmd)
//	{
//		// set note
//		case SYN_Player_Cmd_setNote:
//			cNote = pCmd->arg1;
//			cGate = pCmd->arg2;
//#ifdef _DEBUG
//	printf("Cmd: setNote($%x,$%x)\n", cNote, cGate);
//#endif
//			synth.setNote(cNote);
//			synth.setGate(cGate);
//			break;
//		// set controller
//		case SYN_Player_Cmd_setCtrl:
//			cCtrlId = pCmd->arg1;
//			cCtrlValue = pCmd->arg2;
//#ifdef _DEBUG
//	printf("Cmd: setCtrl($%x,$%x)\n", cCtrlId, cCtrlValue);
//#endif
//			//((char*)&aSynthCtrls[iCh])[cCtrlId] = cCtrlValue;
//			((char*)aSynths[iCh].m_paCtrl)[cCtrlId] = cCtrlValue;
//			//synth.m_ctrls.a[cId] = *pData.db++;
//			//((Ctrl*)&aSynthCtrls[iCh])[*pData.db++].set(*pData.db++);
//			break;
//		// set controller sweep
//		case SYN_Player_Cmd_sweepCtrl:
////#ifdef _DEBUG
////	printf("Cmd: setCtrlSweep(%x,%x,%x)\n", cCtrlId, cCtrlValue);
////#endif
//			//aSynths[iCh]->setCtrlSweep(*pData.db++, (float)*pData.db++/127, (float)*pData.db++/127, *pData.db++);
//			break;
//		case SYN_Player_Cmd_prgChange:
//#ifdef _DEBUG
//	printf("Cmd: prgCh($%x)\n", pCmd->arg1);
//#endif
//			break;
//	}
//	return iResult;
//}
//int XMPlayer::getStatus()
//{
//	return m_status;
//}
//int XMPlayer::setPlayPos(int iStream, int iStep)
//{
//	int iRet = 0;
//	if (m_paStreams != NULL)
//	{
//		EArray *pStream = (EArray*)m_paStreams->get(iStream);
//		if (pStream != NULL)
//		{
//			m_iCurrentStep = 0;
//			if (m_paStreams->Count() > (size_t)iStep)
//			{
//				m_pCurrentStream = pStream;
//				m_iCurrentStep = iStep;
//			}
//			m_iDelta = 0;
//		}
//	}
//	return iRet;
//}


// CONVERT PATTERNS
//	// get first pattern from file
//	XMFILEPATTERN *pFilePattern = (XMFILEPATTERN*)&m_pBuffer[pHeader->iHeaderSize + 60];
//	// create master stream
//	XMSTREAM* NEW(pStream, XMSTREAM, sizeof(PLAYERCOMMAND), 64);
//	XMSTREAM *pMasterStream = pStream;
//	m_paStreams->add(pStream);
//	// running index of created streams
//	int iStream = 1;
//	for (int pi=0; pi<pHeader->iPatternCount; pi++)
//	{
//		// add new pattern (array of stream ids)
//		XMPATTERN *pPattern = (XMPATTERN*)m_paPatterns->add();
//		if (pFilePattern->iPackedDataSize != 0)
//		{
//			int iRows = pFilePattern->iRowCount;
//			pPattern->iRows = iRows;
//			// skip pattern header
//			pFilePattern = (XMFILEPATTERN*)&pFilePattern->aPackedData;
//			// read pattern data
//			XMSTREAM* apStreams[MAX_NUMBER_OF_CHANNELS];
//			// array used to keep track of each channel's state
//			CHANNELINFO aChannels[MAX_NUMBER_OF_CHANNELS];
//			memset(&aChannels, 0, sizeof(CHANNELINFO) * MAX_NUMBER_OF_CHANNELS);
//			for (int ri=0; ri<iRows; ri++)
//			{
//				for (int ci=0; ci<pHeader->iChannelCount; ci++)
//				{
//					if (ri == 0)
//					{
//						NEW(apStreams[ci], XMSTREAM, sizeof(PLAYERCOMMAND), 64);
//						m_paStreams->add(apStreams[ci]);
//					}
//					readCommands((char*&)pFilePattern, ri, aChannels[ci], apStreams[ci]);
//				}
//			}
//			for (int ci=0; ci<pHeader->iChannelCount; ci++)
//			{
//				pPattern->channels[ci] = iStream;
//				iStream++;
//			}
//		}
//		else
//		{
//			// assign default pattern
//			for (int i=0; i<pHeader->iChannelCount; i++)
//			{
//				pPattern->channels[i] = -1;	// -1 means empty stream
//			}
//		}
//	}
//	int iMasterFrame = 0;
//	for (int i=0; i<pHeader->iSongLength; i++)
//	{
//		int iFrame = iMasterFrame;
//		int pi = (&pHeader->aPatternOrder)[i];
//		XMPATTERN *pPattern = (XMPATTERN*)m_paPatterns->get(pi);
//		for (int ci=0; ci<pHeader->iChannelCount; ci++)
//		{
//			addEvent(pMasterStream, iFrame, Player_Cmd_assign, ci, pPattern->channels[ci], 0, 0);
//			iFrame = 0;
//		}
//		iMasterFrame += pPattern->iRows;
//	}
//}	}

//void XMPlayer::addEvent(XMSTREAM *pStream, int iDelta, int cmd, int arg1, int arg2, int arg3, int arg4)
//{
//	SYNCOMMAND syncmd;
//	syncmd.delta = iDelta;
//	syncmd.cmd = cmd;
//	syncmd.arg1 = arg1;
//	syncmd.arg2 = arg2;
//	syncmd.arg3 = arg3;
//	syncmd.arg4 = arg4;
//	pStream->add(&syncmd);
//}
//void XMPlayer::mapVol2Ctrl(int volume, int& cmd, int& ctrlId, int& ctrlValue) {
//	int c = volume & 0xF0;
//	cmd = SYN_Player_Cmd_setCtrl;
//	if (c <= 0x50)
//	{
//		// set volume
//		ctrlId = SYN_CI_Amp;
//		ctrlValue = volume<<1;
//	}
//	else
//	{
//		// invalid command
//		cmd = -1;
//	}
//}
//void XMPlayer::mapFx2Ctrl(int fx, int fxArg, int& cmd, int& ctrlId, int& ctrlValue) {
//	cmd = SYN_Player_Cmd_setCtrl;
//	switch (fx)
//	{
//		case 0x0c:
//			// set volume
//			ctrlId = SYN_CI_Amp;
//			ctrlValue = fxArg>>1;
//			break;
//		default:
//			cmd = -1;
//			break;
//	}
//}
//void XMPlayer::showinfo() {
//	// print general info
//	XMFILEHEADER *pHeader = (XMFILEHEADER*)m_pBuffer;
//	char* NEWBUF(szText, 1024);
//	if (strncmp(pHeader->szIdText, "Extended Module: ", 17) != 0)
//	{
//		strncpy(szText, pHeader->szIdText, 17);
//		printf("Not standard XM file! Id text is: %s\n", szText);
//	}
//	strncpy(szText, pHeader->szModuleName, 20);
//	int iEnd = strrspn(szText, " ");
//	szText[iEnd] = '\0';
//	printf("Module name: %s\n", szText);
//	strncpy(szText, pHeader->szTrackerName, 20);
//	iEnd = strrspn(szText, " ");
//	szText[iEnd] = '\0';
//	printf("Tracker used: %s v%d\n", szText, pHeader->iVersion);
//	printf("Song length: %d\n", pHeader->iSongLength);
//	printf("Restarts at: %d\n", pHeader->iRestartPosition);
//	printf("Channel count: %d\n", pHeader->iChannelCount);
//	printf("Pattern count: %d\n", pHeader->iPatternCount);
//	printf("Instrument count: %d\n", pHeader->iInstrumentCount);
//	printf("Flags: %x\n", pHeader->iFlags);
//	printf("Tempo: %d, BPM: %d\n", pHeader->iDefaultTempo, pHeader->iDefaultBPM);
//
//	printf("Sequences found: %d\n", m_paStreams->Count());
//	Iterator *it = m_paStreams->begin(0);
//	while (m_paStreams->next(it))
//	{
//		printf("Seq #%d", it->m_iItem);
//		EArray *pStream = (EArray*)it->m_pItem;
//		showStream(pStream);
//		printf("\n\n");
//	}
//	DEL(it);
//	printf("\n");
//
//	printf("Pattern order:");
//	for (int i=0; i<pHeader->iSongLength; i++)
//	{
//		char c = (&pHeader->aPatternOrder)[i];
//		printf("%d ", c);
//	}
//	printf("\n");
//	for (size_t i=0; i<m_paPatterns->Count(); i++)
//	{
//		XMPATTERN *pattern = (XMPATTERN*)(m_paPatterns->get(i));
//		printf("Pattern %x: ", i);
//		for (size_t ci=0; pattern->channels[ci] != NULL; ci++)
//		{
//			printf("%x ", pattern->channels[ci]);
//		}
//		printf("\n");
//	}
//	DEL(szText);
//}
//
//void XMPlayer::showStream(XMSTREAM *pStream) {
//	char* NEWBUF(szText, 1024);
//	Iterator *it = pStream->begin(0);
//	int iDelta = -1;
//	int iNote = 0;
//	int iOctave = 1;
//	char cMod = '-';
//	const char szNotes[] = "CcDdEFfGfAaH";
//	while (pStream->next(it))
//	{
//		SYNCOMMAND *pCmd = (SYNCOMMAND*)it->m_pItem;
//		switch (pCmd->cmd)
//		{
//			case SYN_Player_Cmd_setNote:
//				iNote = pCmd->arg1 % 12;
//				iOctave = pCmd->arg1 / 12 + 1;
//				if (szNotes[iNote] & 0x20)
//				{
//					cMod = '#';
//					iNote--;
//				}
//				else
//				{
//					cMod = '-';
//				}
//				_snprintf(szText, 1024, "Note(%c%c%d,%d)", szNotes[iNote], cMod, iOctave, pCmd->arg2);
//				break;
//			case SYN_Player_Cmd_setCtrl:
//				_snprintf(szText, 1024, "Ctrl(%d,%d)", pCmd->arg1, pCmd->arg2);
//				break;
//			case SYN_Player_Cmd_prgChange:
//				_snprintf(szText, 1024, "Prg.Chg(%d)", pCmd->arg1);
//				break;
//		}
//		if (pCmd->delta || it->m_iItem == 0)
//		{
//			printf("\n%x: %s", pCmd->delta, szText);
//		}
//		else
//		{
//			printf(", %s", szText);
//		}
//		
//	}
//	DEL(it);
//	DEL(szText);	
//}
/*
int XMPlayer::load(char *szPath)
{
	int iRet = 0;
	m_file = File::open(szPath, "rb");
	if (m_file != NULL)
	{
		NEWBUF(m_pBuffer, (size_t)m_file->m_iSize);
		int iCount = m_file->read(m_pBuffer, (int)m_file->m_iSize);
		DEL(m_file);
		XMFILEHEADER *pHeader = (XMFILEHEADER*)m_pBuffer;
		if (iCount > 0)
		{
			// calculate cycles per frame, beat = row = 1/4
			m_fCyclesPerFrame = (float)m_iCyclesPerSecond / pHeader->iDefaultBPM*4;	// / pHeader->iDefaultTempo;
			// process patterns
			iRet = iCount;
			XMFILEPATTERN *pFilePattern = (XMFILEPATTERN*)&m_pBuffer[pHeader->iHeaderSize + 60];
			unsigned char *pData =&pFilePattern->aPackedData;
			XMPATTERN *pMasterPattern = (XMPATTERN*)m_paPatterns->add();
			for (int pi=0; pi<pHeader->iPatternCount; pi++)
			{
				XMPATTERN *pPattern = (XMPATTERN*)m_paPatterns->add();
				new(&pPattern->aChannels)SArray(16);
				pPattern->iRowCount = pFilePattern->iRowCount;
				if (pFilePattern->iPackedDataSize != 0)
				{
					// read pattern data
					char note = 0;
					char inst = 0;
					char vol = 0;
					char fx = 0;
					char fxArg = 0;
					EArray *apSequences[32];
					int iDelta = 0;
					for (int ri=0; ri<pPattern->iRowCount; ri++)
					{
						for (int ci=0; ci<pHeader->iChannelCount; ci++)
						{
							if (ri == 0)
							{
								NEW(apSequences[ci], EArray, sizeof(SYNCOMMAND), 16);
								m_paStreams->add(apSequences[ci]);
							}
							EArray *pSequence = apSequences[ci];

							char c = *pData++;
							SYNCOMMAND volCmd;
							SYNCOMMAND fxCmd;
							int mask = c & 0x80 ? c & 0x1F : 0x1F;
							if (mask != 0)
							{
								if (mask & 1)
								{
									note = *pData++;
								}
								if (mask & 2)
								{
									int 
									if (inst != *pData)
									{
										inst = *pData++;
										SYNCOMMAND* pCmd = (SYNCOMMAND*)pSequence->add();
										pCmd->delta = iDelta;
										pCmd->cmd = SYN_Player_Cmd_prgChange;
										pCmd->arg1 = inst;
									}
								}
								if (mask & 4)
								{
									vol = *pData++;
									XMPlayer::mapVol2Ctrl(&volCmd, vol);
								}
								if (mask & 8)
								{
									fx = *pData++;
								}
								if (mask & 16)
								{
									fxArg = *pData++;
								}

								if (mask & 24)
								{
									mapFx2Ctrl(&fxCmd, fx, fxArg);
								}
								if (mask & 1)
								{
									// note on possible
									if (1)
									{
										SYNCOMMAND* pCmd = (SYNCOMMAND*)pSequence->add();
										pCmd->delta = iDelta;
										pCmd->cmd = SYN_Player_Cmd_setNote;
										pCmd->arg1 = note;
										pCmd->arg2 = 0x7F;
									}
									else
									{
										SYNCOMMAND* pCmd = (SYNCOMMAND*)pSequence->add();
										pCmd->delta = iDelta;
										pCmd->cmd = SYN_Player_Cmd_setCtrl;
										pCmd->arg1 = SYN_CI_Pitch;
										pCmd->arg2 = note;
									}
								}
								if (mask & 4)
								{
									SYNCOMMAND* pCmd = (SYNCOMMAND*)pSequence->add();
									memcpy(pCmd, &volCmd, sizeof(SYNCOMMAND));
									pCmd->delta = iDelta;
								}
								if (mask & 24)
								{
									SYNCOMMAND* pCmd = (SYNCOMMAND*)pSequence->add();
									memcpy(pCmd, &fxCmd, sizeof(SYNCOMMAND));
									pCmd->delta = iDelta;
								}
							}
						}
						iDelta++;
					}
					for (int ci=0; ci<pHeader->iChannelCount; ci++)
					{
						pPattern->aChannels.add(apSequences[ci]);
					}
				}
				else
				{
					// create default pattern
					EArray* NEW(pSequence, EArray, sizeof(SYNCOMMAND), 16);
					pPattern->aChannels.add(pSequence);
					SYNCOMMAND* pCmd = (SYNCOMMAND*)pSequence->add();
					pCmd->delta = 0;
					pCmd->cmd = (SYN_PlayerCommands)Player_Cmd_End;
				}
			}
		}
	}
	return iRet;
}
void XMPlayer::showinfo()
{
	// print general info
	XMFILEHEADER *pHeader = (XMFILEHEADER*)m_pBuffer;
	char* NEWBUF(szText, 1024);
	if (strncmp(pHeader->szIdText, "Extended Module: ", 17) != 0)
	{
		strncpy(szText, pHeader->szIdText, 17);
		printf("Not standard XM file! Id text is: %s\n", szText);
	}
	strncpy(szText, pHeader->szModuleName, 20);
	int iEnd = strrspn(szText, " ");
	szText[iEnd] = '\0';
	printf("Module name: %s\n", szText);
	strncpy(szText, pHeader->szTrackerName, 20);
	iEnd = strrspn(szText, " ");
	szText[iEnd] = '\0';
	printf("Tracker used: %s v%d\n", szText, pHeader->iVersion);
	printf("Song length: %d\n", pHeader->iSongLength);
	printf("Restarts at: %d\n", pHeader->iRestartPosition);
	printf("Channel count: %d\n", pHeader->iChannelCount);
	printf("Pattern count: %d\n", pHeader->iPatternCount);
	printf("Instrument count: %d\n", pHeader->iInstrumentCount);
	printf("Flags: %x\n", pHeader->iFlags);
	printf("Tempo: %d, BPM: %d\n", pHeader->iDefaultTempo, pHeader->iDefaultBPM);
	printf("Pattern order:");
	for (int i=0; i<pHeader->iSongLength; i++)
	{
		char c = (&pHeader->aPatternOrder)[i];
		printf("%d ", c);
	}
	printf("\n");
	for (size_t i=0; i<m_paPatterns->Count(); i++)
	{
		XMPATTERN *pattern = (XMPATTERN*)(m_paPatterns->get(i));
		printf("Pattern %x\n", i);
		printf("Length: %d\n", pattern->iRowCount);
	}
	DEL(szText);
}
void XMPlayer::mapVol2Ctrl(SYNCOMMAND *pCmd, int vol)
{
	int iCtrlId = SYN_CI_Amp;
	int iValue = vol;
	pCmd->cmd = SYN_Player_Cmd_setCtrl;
	pCmd->arg1 = iCtrlId;
	pCmd->arg2 = iValue;
}

void XMPlayer::mapFx2Ctrl(SYNCOMMAND *pCmd, int fx, int fxArg)
{
	int iCtrlId = fx;
	int iValue = fxArg;
	pCmd->cmd = SYN_Player_Cmd_setCtrl;
	pCmd->arg1 = iCtrlId;
	pCmd->arg2 = iValue;
}

*/
