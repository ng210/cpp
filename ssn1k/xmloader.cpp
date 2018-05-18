#include <stdio.h>
#include <math.h>

#include "xmloader.h"
#include "base/memory.h"
#include "utils/file.h"

#include "player.h"
#include "playeradapter.h"

NS_PLAYER_USE
NS_SSN1K_USE

static char* NOTES[] = { "C-", "C#", "D-", "D#", "E-", "F-", "F#", "G-", "G#", "A-", "A#", "H-" };

static char dummy[16] = "";
class XmEffectMap : public Map {
public:
	XmEffectMap() {
		init(sizeof(int), 4*sizeof(char), Map::hashingItem, Collection::compareInt);
		int key; char* value;

		key = 0x00; value = "0"; put(&key, value); // 00 // Appregio
		key = 0x01; value = "1"; put(&key, value); // 01 // Porta up
		key = 0x02; value = "2"; put(&key, value); // 02 // Porta down
		key = 0x03; value = "3"; put(&key, value); // 03 // Tone porta
		key = 0x04; value = "4"; put(&key, value); // Vibrato
		key = 0x05; value = "5"; put(&key, value); // Tone porta + Volume slide
		key = 0x06; value = "6"; put(&key, value); // Vibrato + Volume slide
		key = 0x07; value = "7"; put(&key, value); // Tremolo
		key = 0x08; value = "8"; put(&key, value); // Set panning
		key = 0x09; value = "9"; put(&key, value); // Sample offset
		key = 0x0A; value = "A"; put(&key, value); // Volume slide
		key = 0x0B; value = "B"; put(&key, value); // Position jump
		key = 0x0C; value = "C"; put(&key, value); // Set volume
		key = 0x0D; value = "D"; put(&key, value); // Pattern break
		key = 0x0E; value = "E"; put(&key, value); // Fine porta up
		//key = 0xE1; value = "E1"; put(&key, value); // Fine porta up
		//key = 0xE2; value = "E2"; put(&key, value); // Fine porta down
		//key = 0xE3; value = "E3"; put(&key, value); // Set gliss control
		//key = 0xE4; value = "E4"; put(&key, value); // Set vibrato control
		//key = 0xE5; value = "E5"; put(&key, value); // Set finetune
		//key = 0xE6; value = "E6"; put(&key, value); // Set loop begin / loop
		//key = 0xE7; value = "E7"; put(&key, value); // Set tremolo control
		//key = 0xE9; value = "E9"; put(&key, value); // Retrig note
		//key = 0xEA; value = "EA"; put(&key, value); // Fine volume slide up
		//key = 0xEB; value = "EB"; put(&key, value); // Fine volume slide down
		//key = 0xEC; value = "EC"; put(&key, value); // Note cut
		//key = 0xED; value = "ED"; put(&key, value); // Note delay
		//key = 0xEE; value = "EE"; put(&key, value); // Pattern delay
		key = 0x0F; value = "F"; put(&key, value); // Set tempo / BPM
		key = 0x10; value = "G"; put(&key, value); // Set global volume
		key = 0x11; value = "H"; put(&key, value); // Global volume slide
		key = 0x14; value = "K"; put(&key, value); // Key off 
		key = 0x15; value = "L"; put(&key, value); // Set envelope position
		key = 0x19; value = "P"; put(&key, value); // Panning slide
		key = 0x1B; value = "R"; put(&key, value); // Multi retrig note
		key = 0x1D; value = "T"; put(&key, value); // Tremor
		key = 0x21; value = "X"; put(&key, value); // X1 - Extra fine porta up / X2 - Extra fine porta down
//		key = 0x21; value = "-X2"; put(&key, value); // X2 - Extra fine porta down
	};
	~XmEffectMap() {
	}
	char* get(int key) {
		void* value = Map::get(&key);
		if (value == NULL) {
			sprintf(dummy, "%02x", key);
			value = &dummy;
		}
		return (char*)value;
	}
};

static XmEffectMap* xmEffectMap_ = NULL;

XmLoader::XmLoader(PlayerAdapter* playerAdapter, SynthAdapter* synthAdapter) : patternOrder_(Array(sizeof(BYTE))) {
	playerAdapter_ = playerAdapter;
	synthAdapter_ = synthAdapter;
	if (xmEffectMap_ == NULL) {
		xmEffectMap_ = NEW_(XmEffectMap);
	}
}

XmLoader::~XmLoader(void) {
	ARRAY_FOREACH(&patterns_, FREE(value););
	DEL_(xmEffectMap_);
}

// NOTE
XMNOTE* XmLoader::readNote(BYTE*& ptr, XMNOTE* xmNote) {
	BYTE code = *ptr++;
	xmNote->hasData = (code != 0x80);
	bool isCompressed = code & 0x80;
	int note = -1;
	if (!isCompressed) {
		note = code - 1;
		code = 0x9e;
	}
	code &= 0x1f;
	xmNote->note = (code & 0x01) ? *ptr++-1 : note;
	xmNote->instrument = (code & 0x02) ? *ptr++-1 : -1;
	xmNote->volume = (code & 0x04) ? *ptr++ : -1;
	xmNote->effect = (code & 0x08) ? *ptr++ : -1;
	xmNote->parameter = (code & 0x10) ? *ptr++ : -1;
	return xmNote;
}
PArray* XmLoader::convertNote(XMNOTE* xmNote, XMNOTE& state) {
	PLAYER_COMMAND cmd = NULL;
	PArray* commands = NEW_(PArray);
	if (xmNote->hasData) {
		// process instrument
		int instrument = xmNote->instrument;
		if (instrument != 0xFF && instrument != state.instrument) {
			state.instrument = instrument;
			cmd = synthAdapter_->createCommand(Synth_Cmd_prgChange, instrument);
			commands->add(cmd);
		}
		// process volume
		int volume = xmNote->volume;
		if (volume != 0xFF) {
			volume -= 0x10;
			if (volume < 0 || volume > 0x40) {
				volume = state.volume;
			}
			state.volume = volume;
		}
		// process note
		int note = xmNote->note;
		if (note >= 0x00 && note < 0x60) {
			note += 12;
			//if (instrument != 0xff) {
				// note on
				int v = volume == 0xFF ? 0x40 : volume;
				cmd = synthAdapter_->createCommand(Synth_Cmd_setControlF, SSN1K_CI_Env1Amp, 1.0);
				commands->add(cmd);
				cmd = synthAdapter_->createCommand(Synth_Cmd_setNoteOn, note, volume*255 >> 6);
				commands->add(cmd);
				volume = 0xFF;
			//} else {
			//	if (note != state.note) {
			//		// set note
			//		cmd = synthAdapter_->createCommand(Synth_Cmd_setControlF, SSN1K_CI_Tune, (double)note);
			//		commands->add(cmd);
			//	}
			//}
			state.note = note;
		}
		if (note == 0x60 && xmNote->note != state.note) {
			cmd = synthAdapter_->createCommand(Synth_Cmd_setNoteOff, state.note);
			commands->add(cmd);
			state.note = xmNote->note;
		}
		if (volume != 0xFF) {
			cmd = synthAdapter_->createCommand(Synth_Cmd_setControlF, SSN1K_CI_Env1Amp, volume / 255.0);
			commands->add(cmd);
		}
	}
	//v = xmNote.instrument;
	//(v != 0xff) ? printf("%02X ", v) : printf("-- ");
	//v = xmNote.volume;
	//(v != 0xff) ? printf("%02X ", v) : printf("-- ");
	//v = xmNote.effect;
	//(v != 0xff) ? printf("%s", xmEffectMap_->get(v)) : printf("--");
	//v = xmNote.parameter;
	//(v != 0xff) ? printf("%02X", v) : printf("--");
	if (commands->length() == 0) {
		DEL_(commands);
		commands = NULL;
	}
	return commands;
}
int XmLoader::printNote(XMNOTE* xmNote, char* buffer) {
	char* ptr = buffer;
	if (xmNote->hasData) {
		BYTE v = xmNote->note;
		ptr += (v != 0xff) ? (v != 96) ? sprintf(ptr, "%s%d ", NOTES[v % 12], v / 12) : sprintf(ptr, "off ") : sprintf(ptr, "--- ");
		v = xmNote->instrument;
		ptr += (v != 0xff) ? sprintf(ptr, "%02X ", v) : sprintf(ptr, "-- ");
		v = xmNote->volume;
		ptr += (v != 0xff) ? sprintf(ptr, "%02X ", v) : sprintf(ptr, "-- ");
		v = xmNote->effect;
		ptr += (v != 0xff) ? sprintf(ptr, "%s", xmEffectMap_->get(v)) : sprintf(ptr, "-");
		v = xmNote->parameter;
		ptr += (v != 0xff) ? sprintf(ptr, "%02X", v) : sprintf(ptr, "--");
	} else {
		ptr += sprintf(ptr, "... .. .. ...");
	}
	return (int)(ptr - buffer);
}

// PATTERN
XMPATTERN* XmLoader::readPattern(XMFILEPATTERN* ptr) {
	XMPATTERN* pattern = (XMPATTERN*)MALLOC(BYTE, sizeof(XMPATTERN) + sizeof(XMNOTE)*ptr->rowCount*channelCount_);
	pattern->rowCount = ptr->rowCount;
	pattern->data = (XMNOTE*)((BYTE*)pattern + sizeof(XMPATTERN));
	BYTE* packedData = &ptr->packedData;
	int i = 0;
	for (int r = 0; r < ptr->rowCount; r++) {
		for (int ch = 0; ch < channelCount_; ch++) {
			readNote(packedData, &pattern->data[i++]);
		}
	}
	return pattern;
}
void XmLoader::convertPattern(XMPATTERN* pattern, PArray* sequences) {
	for (int ch = 0; ch < channelCount_; ch++) {
		bool hasFrames = false;
		Sequence* sequence = NEW_(Sequence);
		for (int ri = 0; ri < pattern->rowCount; ri++) {
			XMNOTE* xmNote = &pattern->data[ch + ri*channelCount_];
			if (xmNote->hasData) {
				PArray* commands = convertNote(xmNote, states_[ch]);
				if (commands != NULL) {
					Frame* frame = sequence->addFrame(ri);
					hasFrames = true;
					ARRAY_FOREACH(commands, frame->addCommand((PLAYER_COMMAND)value));
					DEL_(commands);
				}
			}
		}
		int id = -1;
		if (hasFrames) {
			id = sequences->length();
			sequence->addFrame(pattern->rowCount)->addCommand(playerAdapter_->createEndCommand());
			sequences->add(sequence);
		} else {
			DEL_(sequence);
		}
		pattern->sequences[ch] = id;
	}
}
int XmLoader::printPattern(XMPATTERN* pattern, char* buffer) {
	int n = 0;
	char* ptr = buffer;
	for (int r = 0; r < pattern->rowCount; r++) {
		ptr += sprintf(ptr, "%02x: ", r);
		for (int ch = 0; ch < channelCount_; ch++) {
			XMNOTE* xmNote = &pattern->data[n++];
			ptr += printNote(xmNote, ptr);
			if (ch < channelCount_ - 1) {
				ptr += sprintf(ptr, "|");
			}
		}
		ptr += sprintf(ptr, "\n");
	}
	return (int)(ptr - buffer);
}

// LOADER
int XmLoader::load(const char *szPath) {
	int res = 1;
	BYTE* xm = File::read(szPath);
	if (xm != NULL) {
		res = 0;
		// read header
		XMFILEHEADER *header = (XMFILEHEADER*)xm;
		bpm_ = (float)header->defaultBPM;
		ticks_ = 24.0f / header->defaultTempo;
		channelCount_ = header->channelCount;
		for (int i = 0; i < header->songLength; i++) {
			patternOrder_.add(&header->patternOrder + i);
		}
		// read pattern
		BYTE* ptr = xm + offsetof(XMFILEHEADER, headerSize) + header->headerSize;
		for (int p = 0; p < header->patternCount; p++) {
			XMFILEPATTERN* xmPattern = (XMFILEPATTERN*)ptr;
			patterns_.add(readPattern(xmPattern));
			ptr += offsetof(XMFILEPATTERN, packedData) + xmPattern->packedDataSize;
		}
		res = 0;
		FREE(xm);
	}
	return res;
}
PArray* XmLoader::convert() {
	// create master sequence
	PArray* sequences = NEW_(PArray);
	Sequence* masterSequence = NEW_(Sequence);
	sequences->add(masterSequence);
	Frame* frame = masterSequence->addFrame(0);
	frame->addCommand(playerAdapter_->createCommand(Player_Cmd_setTempo, bpm_, ticks_));

	// reset channel states
	for (int i = 0; i < channelCount_; i++) {
		memset(states_, -1, sizeof(XMNOTE));
		states_[i].hasData = 0;
	}	

	// convert patterns
	for (UINT32 pi = 0; pi < patterns_.length(); pi++) {
		XMPATTERN* pattern = (XMPATTERN*)patterns_.getAt(pi);
		convertPattern(pattern, sequences);
	}

	// add sequences to master sequence
	int rowCounter = 0;
	for (UINT32 i = 0; i < patternOrder_.length(); i++) {
		BYTE pi = *(BYTE*)patternOrder_.getAt(i);
		if (i > 0) frame = NULL;
		XMPATTERN* pattern = (XMPATTERN*)patterns_.getAt(pi);
		for (int ch = 0; ch < channelCount_; ch++) {
			int sequenceId = pattern->sequences[ch];
			if (sequenceId != -1) {
				if (frame == NULL) {
					frame = masterSequence->addFrame(rowCounter);
				}
				frame->addCommand(playerAdapter_->createCommand(Player_Cmd_assign, ch+1, sequenceId, Player_Flg_Active));
			}
		}
		rowCounter += pattern->rowCount;
	}

	masterSequence->addFrame(rowCounter)->addCommand(playerAdapter_->createEndCommand());


	//	int n = 0;
	//	memset(xmNotes, 0, sizeof(XMNOTE) * 64);
	//	for (int r = 0; r < pattern->rowCount; r++) {
	//		printf("%02x: ", r);
	//		for (int ch = 0; ch < channelCount_; ch++) {
	//			XMNOTE* xmNote = &pattern->data[n++];
	//			if (xmNote->hasData) {
	//				if (channels[ch] == NULL) {
	//					sequences->add(channels[ch] = NEW_(Sequence));
	//				}
	//				convertNote(channels[ch], rowCounter, xmNote, synthAdapter);
	//			//	SYNTH_CMD_SET_NOTE* command = convertNote(channels[ch], rowCounter, xmNote, synthAdapter);
	//			//	if (lastSetNoteCommand[ch] == NULL && xmNote->note < 96) {
	//			//		lastSetNoteCommand[ch] = command;
	//			//	} else {
	//			//		//if (lastNoteOn[ch] == NULL && xmNote->note == 96);
	//			//		if (lastSetNoteCommand[ch] != NULL && xmNote->note < 96) {
	//			//			lastSetNoteCommand[ch]->duration = rowCounter - lastSetNoteCommand[ch]->duration;
	//			//			lastSetNoteCommand[ch] = command;
	//			//		} else {
	//			//			if (lastSetNoteCommand[ch] != NULL && xmNote->note == 96) {
	//			//				lastSetNoteCommand[ch]->duration = rowCounter - lastSetNoteCommand[ch]->duration;
	//			//				lastSetNoteCommand[ch] = NULL;
	//			//			}
	//			//		}
	//			//	}
	//			}
	//		}
	//		rowCounter++;
	//	}

	//for (int ch = 0; ch < channelCount_; ch++) {
	//	if (lastSetNoteCommand[ch] != NULL) {
	//		lastSetNoteCommand[ch]->duration = rowCounter - lastSetNoteCommand[ch]->duration;
	//	}
	//}
	return sequences;
}
//void XmLoader::addEvent(Frame* frame, BYTE cmd, int arg1, int arg2, int arg3, int arg4) {
//
//}
//void XmLoader::convertNote(XMNOTE& xmNote, Sequence* track, int time, BYTE*& data) {	// int iFrame, CHANNELINFO &chInfo, XMSTREAM *pStream) {
//	BYTE code = *data++;
//	bool isCompressed = code & 0x80;
//	if (code != 0x80) {
//		Frame* frame = track->addFrame(time);
//		if (!isCompressed) {
//			xmNote.note = code;
//			code = 0x9e;
//		}
//		code &= 0x1f;
//		if (code & 0x02) {	// instrument
//							// add program change
//			xmNote.instrument = *data++;
//			addEvent(frame, Synth_Cmd_prgChange, xmNote.instrument);
//		}
//
//		// add note on event
//		if (code & 0x05 == 0x05) {	// note+velocity
//			xmNote.note = *data++;
//		}
//		if (code & 0x04) {	// volume
//							// add set volume
//			xmNote.volume = *data++;
//		}
//		BYTE velocity = 100;
//		if (code & 0x04) {
//			velocity =
//		}
//		addEvent(frame, Synth_Cmd_setNote, -1, velocity);
//
//		if (code & 0x08) {	// effect
//			xmNote.effect = *data++;
//			//note->parameter = *data++;
//		}
//		if (code & 0x10) {	// parameter
//			xmNote.parameter = *data++;
//		}
//	}
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


		//// create main sequence
		//if (sequences_ != NULL) {
		//	ARRAY_FOREACH(sequences_, DEL_((Sequence*)value););
		//	DEL_(sequences_);
		//}
		//sequences_ = NEW_(PArray);

		//mainSequence_ = NEW_(Sequence);
		//sequences_->add(mainSequence_);

		//// convert patterns into sequences
		//BYTE* ptr = xm + offsetof(XMFILEHEADER, headerSize) + header->headerSize;

		//XMNOTE xmNotes[64];
		//for (int p = 0; p < header->patternCount; p++) {
		//	XMFILEPATTERN* xmPattern = (XMFILEPATTERN*)ptr;
		//	ptr += offsetof(XMFILEPATTERN, packedData);
		//	int delta = 0;
		//	for (int ch = 0; ch < header->channelCount; ch++) {
		//		Sequence* track = NEW_(Sequence);
		//		for (int r = 0; r < xmPattern->rowCount; r++) {
		//			convertNote(xmNotes[ch], track, delta, ptr);
		//			delta += header->defaultTempo;
		//		}
		//	}
		//}

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
