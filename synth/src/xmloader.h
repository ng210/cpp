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

	//typedef void (XmEffectHandler)(byte code, XmNote* xmNote);

	class XmEffect {
		PROP(byte, code);
		PROP(int, parameter);
		//PROP_R(char, sign);
		//PROP_R(XmEffectHandler*, handler);
	public:
		XmEffect();
		XmEffect(int code, int parameter);
	};

	typedef struct XmNote {
		bool hasData;
		byte note;
		byte instrument;
		int volume;
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

	typedef enum XmNoteBits {
		XmNoteBitsCompressed	= 0x80,
		XmNoteBitsFxParameter	= 0x10,
		XmNoteBitsFxType		= 0x08,
		XmNoteBitsVolume		= 0x04,
		XmNoteBitsInstrument	= 0x02,
		XmNoteBitsNote			= 0x01,

		XmNoteBitsAll			= 0x1f,
	} XmNoteBits;

	typedef enum XmFxType {
		XmFxTypeAppregio,				// 0 Appregio
		XmFxTypePortaUp,				// 1 Porta up
		XmFxTypePortaDown,				// 2 Porta down
		XmFxTypeTonePorta,				// 3 Tone porta
		XmFxTypeVibrato,				// 4 Vibrato
		XmFxTypeTonePortaVolumeSlide,	// 5 Tone porta + Volume slide
		XmFxTypeVibratoVolumeSlide,		// 6 Vibrato + Volume slide
		XmFxTypeTremolo,				// 7 Tremolo
		XmFxTypeSetPanning,				// 8 Set panning
		XmFxTypeSampleOffset,			// 9 Sample offset
		XmFxTypeVolumeSlide,			// A Volume slide
		XmFxTypePositionJump,			// B Position jump
		XmFxTypeSetVolume,				// C Set volume
		XmFxTypePatternBreak,			// D Pattern break
		XmFxTypeFinePortaUp,			// E1 Fine porta up
		XmFxTypeFinePortaDown,			// E2 Fine porta down
		XmFxTypeSetGlissControl,		// E3 Set gliss control
		XmFxTypeSetVibratoControl,		// E4 Set vibrato control
		XmFxTypeSetFinetune,			// E5 Set finetune
		XmFxTypeSetLoop,				// E6 Set loop begin / loop
		XmFxTypeSetTremoloControl,		// E7 Set tremolo control
		XmFxTypeRetrigNote,				// E9 Retrig note
		XmFxTypeFineVolumeSlideUp,		// EA Fine volume slide up
		XmFxTypeFineVolumeSlideDown,	// EB Fine volume slide down
		XmFxTypeNoteCut,				// EC Note cut
		XmFxTypeNoteDelay,				// ED Note delay
		XmFxTypePatternDelay,			// EE Pattern delay
		XmFxTypeSetTempo,				// F Set tempo / BPM
		XmFxTypeSetGlobalVolume,		// G  (010h) Set global volume
		XmFxTypeGlobalVolumeSlide,		// H  (011h) Global volume slide
										// I  (012h) Unused
										// J  (013h) Unused
										// K  (014h) Unused
		XmFxTypeSetEnvelopePosition,	// L  (015h) Set envelope position
										// M  (016h) Unused
										// N  (017h) Unused
										// O  (018h) Unused
		XmFxTypePanningSlide,			// P  (019h) Panning slide
										// Q  (01ah) Unused
		XmFxTypeMultiRetrigNote,		// R  (01bh) Multi retrig note
										// S  (01ch) Unused
		XmFxTypeTremor,					// T  (01dh) Tremor
										// U  (01eh) Unused
										// V  (01fh) Unused
										// W  (020h) Unused
		XmFxTypeExtraFinePortaUp,		// X1 (021h) Extra fine porta up
		XmFxTypeExtraFinePortaDown,		// X2 (021h) Extra fine porta down

		XmFxTypeNone
	} XmFxType;

	class XmLoader {
		int bpm_;
		int ticks_;
		int channelCount_;
		Array patternOrder_;
		PROP_R(Array, instrumentInfos);
		SynthAdapter* synthAdapter_;
		Player* player_;
		PlayerDevice* playerDevice_;

		void processVolumeEffect(byte code, byte*& ptr, XmNote* xmNote);
		void processEffect(byte code, byte*& ptr, XmNote* xmNote);
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