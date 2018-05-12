#ifndef __XMLOADER_H
#define __XMLOADER_H

#include "basedef.h"
#include "collection/map.h"
#include "sequence.h"
#include "collection/parray.h"
#include "playeradapter.h"
#include "synthadapter.h"

NS_FW_BASE_USE
NS_PLAYER_USE
NS_SSN1K_USE

#define MAX_NUMBER_OF_CHANNELS 32

#pragma pack(push,1)
typedef struct XMFILEHEADER_ {
	char idText[17];
	char moduleName[20];
	char ch01A;
	char trackerName[20];
	WORD version;
	DWORD headerSize;
	WORD songLength;
	WORD restartPosition;
	WORD channelCount;
	WORD patternCount;
	WORD instrumentCount;
	WORD flags;
	WORD defaultTempo;
	WORD defaultBPM;
	BYTE patternOrder;
} XMFILEHEADER;

typedef struct XMFILEPATTERN_ {
	DWORD headerSize;
	BYTE packingType;
	WORD rowCount;
	WORD packedDataSize;
	BYTE packedData;
} XMFILEPATTERN;
#pragma pack(pop)

//typedef Array XMSTREAM;

typedef struct XMNOTE_ {
	BYTE hasData;
	BYTE note;
	BYTE instrument;
	BYTE volume;
	BYTE effect;
	BYTE parameter;
} XMNOTE;

typedef struct XMPATTERN_ {
	int rowCount;
	XMNOTE* data;
	int sequences[64];
} XMPATTERN;

//typedef struct CHANNELINFO {
//	// id of currently assigned sequence
//	int iSequence;
//	int iFrame;
//	// currently assigned note, velocity, instrument values
//	int note, velocity, instrument, ctrlId;
//	// currently assigned controller values
//	int ctrls[256];
//} CHANNELINFO;

//typedef struct XMFILEINSTRUMENT
//{
//	int id;
//} XMFILEINSTRUMENT;

class XmLoader {
//protected:
	//static void mapVol2Ctrl(int volume, int& cmd, int& ctrlId, int& ctrlValue);
	//static void mapFx2Ctrl(int fx, int fxArg, int& cmd, int& ctrlId, int& ctrlValue);
	//static void addEvent(XMSTREAM *pStream, int iDelta, int cmd, int arg1, int arg2, int arg3, int arg4);
protected:	PROP_R(float, bpm);
protected:	PROP_R(float, ticks);
protected:	PROP_REF(PArray, patterns);
protected:	PROP_REF(Array, patternOrder);
protected:	PROP_R(PlayerAdapter*, playerAdapter);
protected:	PROP_R(SynthAdapter*, synthAdapter);
protected:	XMNOTE states_[64];

//protected:	PROP_R(PArray*, sequences);
//protected:	PROP_R(PArray*, instruments);
//protected:	PROP_R(Sequence*, mainSequence);
protected:	PROP_R(int, channelCount);
public:
	XmLoader(PlayerAdapter* playerAdapter, SynthAdapter* synthAdapter);
	virtual ~XmLoader(void);

	XMPATTERN* readPattern(XMFILEPATTERN* ptr);
	void convertPattern(XMPATTERN* pattern, PArray* sequences);
	int printPattern(XMPATTERN* pattern, char* buffer);

	XMNOTE* readNote(BYTE*& ptr, XMNOTE* xmNote);
	PArray* convertNote(XMNOTE* xmNote, XMNOTE& state);
	int printNote(XMNOTE* xmNote, char* buffer);

	//void addEvent(Frame* frame, BYTE cmd, int arg1 = 0, int arg2 = 0, int arg3 = 0, int arg4 = 0);
	//void convertNote(XMNOTE& note, Sequence* track, int delta, BYTE*& data);
	//void readInstrument();
	int load(const char *szPath);
	PArray* convert();

	static Map effects;
};

#endif