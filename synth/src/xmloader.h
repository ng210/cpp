#ifndef __XMLOADER_H
#define __XMLOADER_H

#include "player/src/player.h"
#include "synth-adapter.h"

NS_FW_BASE_USE
namespace SYNTH {

#define MAX_NUMBER_OF_CHANNELS 32

#pragma pack(push,1)
	typedef struct XmFileHeader {
		char idText[17];
		char moduleName[20];
		char ch01A;
		char trackerName[20];
		word version;
		dword headerSize;
		word songLength;
		word restartPosition;
		word channelCount;
		word patternCount;
		word instrumentCount;
		word flags;
		word defaultTempo;
		word defaultBPM;
		byte patternOrder;
	} XmFileHeader;

	typedef struct XmFilePattern {
		dword headerSize;
		byte packingType;
		word rowCount;
		word packedDataSize;
		byte packedData;
	} XmFilePattern;
#pragma pack(pop)

	typedef struct XmEffect {
		byte code;
		byte parameter;
	} XmEffect;

	typedef struct XmNote {
		bool hasData;
		byte note;
		byte instrument;
		byte volume;
		XmEffect effects[2];
	} XmNote;

	typedef struct XmInstrumentHeader {
		dword size;
		char name[22];
		byte type;		// (always 0)
		word sampleCount;
	} XmInstrumentHeader;

	typedef struct XmInstrument {
		XmInstrumentHeader header;
		dword size;
		byte sampleForNotes[96];
		byte volumneEnvelope[48];
		byte panningEnvelope[48];
		byte volumePoints;
		byte panningPoints;
		byte volumeSustainPoint;
		byte volumeLoopStart;
		byte volumeLoopEnd;
		byte panningSustainPoint;
		byte panningLoopStart;
		byte panningLoopEnd;
		byte volumeType;	// bit 0 : On; bit 1: Sustain; bit 2: Loop;
		byte panningType;	// bit 0 : On; bit 1: Sustain; bit 2: Loop;
		byte vibratoType;
		byte vibratoSweep;
		byte vibratoDepth;
		byte vibratoRate;
		word VolumeFadeout;
		word Reserved;
	} XmInstrument;

	typedef struct XmPattern {
		int rowCount;
		XmNote* notes;
	} XmPattern;

	class XmLoader {
		static Map effects_;

		int bpm_;
		int ticks_;
		int channelCount_;
		Array patternOrder_;
		Array instruments_;
		Adapter* synthAdapter_;
		Player* player_;
		Stream* soundBank_;

		void getVolumeEffect(byte code, byte*& ptr, XmNote* xmNote);
		void getEffect(byte code, byte*& ptr, XmNote* xmNote);
		XmNote* readNote(byte*& ptr, XmNote* xmNote);
		XmPattern* readPattern(XmFilePattern* ptr);		
		//Sequence* optimizeSequence(Sequence* sequence);
		void process();
	public:
		PROP_R(PArray, patterns);
		XmLoader(Player* player, Stream* soundBank = NULL);
		virtual ~XmLoader(void);

		int load(const char* path);

#ifdef _DEBUG
		char* printPattern(int id);
		int printNote(XmNote* xmNote, char* buffer);
#endif
	};
}
#endif