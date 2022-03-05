#ifndef __XMLOADER_H
#define __XMLOADER_H

//#include <windows.h>

#include "basedef.h"
#include "collection/map.h"
#include "sequence.h"
#include "collection/parray.h"
#include "playeradapter.h"
#include "ssn1k/synthadapter.h"

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
	UINT16 version;
	UINT32 headerSize;
	UINT16 songLength;
	UINT16 restartPosition;
	UINT16 channelCount;
	UINT16 patternCount;
	UINT16 instrumentCount;
	UINT16 flags;
	UINT16 defaultTempo;
	UINT16 defaultBPM;
	UINT8 patternOrder;
} XMFILEHEADER;

typedef struct XMFILEPATTERN_ {
	UINT32 headerSize;
	UINT8 packingType;
	UINT16 rowCount;
	UINT16 packedDataSize;
	UINT8 packedData;
} XMFILEPATTERN;
#pragma pack(pop)

//typedef Array XMSTREAM;

typedef struct XMNOTE_ {
	UINT8 hasData;
	UINT8 note;
	UINT8 instrument;
	UINT8 volume;
	UINT8 effect;
	UINT8 parameter;
} XMNOTE;

typedef struct XMPATTERN_ {
	int rowCount;
	Array* channels[64];
	XMNOTE* data;
} XMPATTERN;

class XmLoader {
protected:	PROP_R(float, bpm);
protected:	PROP_R(float, ticks);
protected:	PROP_REF(PArray, patterns);
protected:	PROP_REF(Array, patternOrder);
protected:	PROP_R(PlayerAdapter*, playerAdapter);
protected:	PROP_R(SynthAdapter*, synthAdapter);
protected:	XMNOTE states_[64];
protected:	PROP_R(int, channelCount);

protected:
	static Map effects;

	XMPATTERN* readPattern(XMFILEPATTERN* ptr);
	void convertPattern(XMPATTERN* pattern, PArray* sequences);
	XMNOTE* readNote(UINT8*& ptr, XMNOTE* xmNote);
	PArray* convertNote(XMNOTE* xmNote, XMNOTE& state);
#ifdef _DEBUG
	int printPattern(XMPATTERN* pattern, char* buffer);
	int printNote(XMNOTE* xmNote, char* buffer);
#endif
public:
	XmLoader(PlayerAdapter* playerAdapter, SynthAdapter* synthAdapter);
	virtual ~XmLoader(void);

	int load(const char *szPath);
	PArray* convert();
};

#endif