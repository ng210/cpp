#ifndef __XMLOADER_H
#define __XMLOADER_H

#include "player/src/player.h"
#include "synth/src/device/synth-adapter.h"
#include "synth/src/device/module-device.h"

NS_FW_BASE_USE
using namespace PLAYER;
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

	typedef struct XmInstrument {
		dword instrumentHeaderSize;
		char name[22];
		byte type;				// (always 0)
		word sampleCount;
		dword sampleHeaderSize;
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
		byte volumeType;		// bit 0 : On; bit 1: Sustain; bit 2: Loop;
		byte panningType;		// bit 0 : On; bit 1: Sustain; bit 2: Loop;
		byte vibratoType;
		byte vibratoSweep;
		byte vibratoDepth;
		byte vibratoRate;
		word VolumeFadeout;
		word Reserved;
	} XmInstrument;

	typedef struct XmSample {
		dword  length;
		dword  loopStart;
		dword  loopLength;
		byte   volume;
		byte   fineTune;
		byte   type;
		byte   panning;
		byte   note;
		byte   reserved;
		char   name[22];
	} XmSample;

	typedef struct XmPattern {
		int rowCount;
		XmNote* notes;
	} XmPattern;
#pragma pack(pop)

	typedef struct InstrumentInfo {
		char name[24];
		int channelId;
		int deviceId;
		int drumId;
		ModuleDevice* device;
		int voiceCount;
		int programId;
		Soundbank* soundbank;
		int dataBlockId;
		int sequenceId;
		Sequence* sequence;
	} InstrumentInfo;

	typedef Device* (ADD_DEVICE_HANDLER)(SynthAdapter*, InstrumentInfo*);

	typedef Device* (ADD_EFFECT_HANDLER)(Player*/*, MixerChannel*/);

	class XmLoader {
		static Map effects_;
		//static ADD_DEVICE_HANDLER addDefaultDevice;
		static ADD_EFFECT_HANDLER addNoEffect;

		int bpm_;
		int ticks_;
		int channelCount_;
		Array patternOrder_;
		PROP_R(Array, instrumentInfos);
		SynthAdapter* synthAdapter_;
		Player* player_;
		PlayerDevice* playerDevice_;

		void getVolumeEffect(byte code, byte*& ptr, XmNote* xmNote);
		void getEffect(byte code, byte*& ptr, XmNote* xmNote);
		XmNote* readNote(byte*& ptr, XmNote* xmNote);
		XmPattern* readPattern(XmFilePattern* ptr);		
		//Sequence* optimizeSequence(Sequence* sequence);
		void convertPatterns();
	public:
		PROP_R(PArray, patterns);
		XmLoader(PlayerDevice* playerDevice);
		virtual ~XmLoader(void);

		int load(const char* xmPath, Map* soundbanks);

		char* printPattern(int id);
		int printNote(XmNote* xmNote, char* buffer);
	};
}
#endif