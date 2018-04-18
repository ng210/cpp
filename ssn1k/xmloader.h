#ifndef __XMLOADER_H
#define __XMLOADER_H

#include "basedef.h"
#include "collection/parray.h"

NS_FW_BASE_USE

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
	WORD rstartPosition;
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

typedef struct XMPATTERN_ {
	int rows;
	int channels[MAX_NUMBER_OF_CHANNELS];
} XMPATTERN;

typedef struct XMNOTE_ {
	int note;
	int instrument;
	int volume;
	int effect;
	int parameter;
} XMNOTE;

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
protected:
	//static void mapVol2Ctrl(int volume, int& cmd, int& ctrlId, int& ctrlValue);
	//static void mapFx2Ctrl(int fx, int fxArg, int& cmd, int& ctrlId, int& ctrlValue);
	//static void addEvent(XMSTREAM *pStream, int iDelta, int cmd, int arg1, int arg2, int arg3, int arg4);
protected:	PROP_R(size_t, tempo);
protected:	PROP_R(PArray*, sequences);
protected:	PROP_R(PArray*, instruments);
public:
	XmLoader();
	virtual ~XmLoader(void);

	void readRow(XMNOTE* note, BYTE*& data);
	//void readInstrument();
	int load(char *szPath);
};

#endif