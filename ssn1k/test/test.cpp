#include <stdio.h>
#include <math.h>
#include <conio.h>

#include "consoleapp.h"
#include "soundlib/soundplayer.h"
#include "soundlib/wavfmt.h"

#include "ssn1k/ssn1klib.h"
#include "syn/asu.h"

NS_FW_BASE_USE
NS_SSN1K_USE
NS_PLAYER_USE

#define TPS (14*4)	// ticks per second
#define TICKS 4*128

const int sampleRate = 48000;

size_t frameCounter = 0;
size_t frames = 0;
Player* player = NULL;
Synth* synths[16];
Mixer* mixer;
SynthAdapter* synthAdapter;
Sequence* sequences[256];

#define dd2db(dd) (BYTE)(dd & 0xff), (BYTE)((dd>>8) & 0xff), (BYTE)((dd>>16) & 0xff), (BYTE)((dd>>24) & 0xff)

void print(const char* const format, ...) {
	va_list args;
	va_start(args, format);
	console.vprintf(format, args);
	va_end(args);
}

COORD* gotoxy(int x, int y) {
	return console.gotoxy(x, y);
}

//*****************************************************************************

BYTE chords[] = {
	SSN1K_CI_SynthMix, SSN1K_MM_OVR,
	SSN1K_CI_SynthAmp, 0xFF,
	SSN1K_CI_SynthBal, 0x80,

	SSN1K_CI_Env1Mix, dd2db(0x3F600000),
	SSN1K_CI_Env1Amp, dd2db(0x3F800000),
	//SSN1K_CI_Env1DC, 0.0f,
	SSN1K_CI_Env1Atk, 0x10,
	SSN1K_CI_Env1Dec, 0x20,
	SSN1K_CI_Env1Sus, 0x80,
	SSN1K_CI_Env1Rel, 0x80,

	//SSN1K_CI_Env2Mix, 0.0f,
	//SSN1K_CI_Env2Amp, 0.1f,
	//SSN1K_CI_Env2DC, 0.0f,
	//SSN1K_CI_Env2Atk, 0.1f,
	//SSN1K_CI_Env2Dec, 0.2f,
	//SSN1K_CI_Env2Sus, 0.2f,
	//SSN1K_CI_Env2Rel, 0.4f,

	SSN1K_CI_Env3Mix, dd2db(0x3F400000),
	SSN1K_CI_Env3Amp, dd2db(0x43FA0000),
	SSN1K_CI_Env3DC, dd2db(0x43480000),
	//SSN1K_CI_Env3Amp, dd2db(0x447A0000),
	//SSN1K_CI_Env3DC, dd2db(0x41C80000),
	SSN1K_CI_Env3Atk, 0x0C,
	SSN1K_CI_Env3Dec, 0x18,
	SSN1K_CI_Env3Sus, 0x80,
	SSN1K_CI_Env3Rel, 0x60,

	//SSN1K_CI_Env4Mix, SSN1K_MM_BPS,
	//SSN1K_CI_Env4Amp, 0.0f,
	//SSN1K_CI_Env4DC, 0.0f,
	//SSN1K_CI_Env4Atk, 0.0f,
	//SSN1K_CI_Env4Dec, 0.0f,
	//SSN1K_CI_Env4Sus, 0.0f,
	//SSN1K_CI_Env4Rel, 0.0f,
	//SSN1K_CI_Env4Gate, 0.0f,

	//SSN1K_CI_Osc1Mix, CtrlValue(SSN1K_MM_OVR),
	SSN1K_CI_Osc1Amp, 0xA0,
	//SSN1K_CI_Osc1DC, 0.0f,
	//SSN1K_CI_Osc1Note, 0.0f,
	//SSN1K_CI_Osc1Tune, 0.0f,
	//SSN1K_CI_Osc1Fre, 0.0f,
	SSN1K_CI_Osc1Psw, 0x80,
	SSN1K_CI_Osc1Wav, SSN1K_WF_PSAW,

	SSN1K_CI_Osc2Mix, SSN1K_MM_ADD,
	SSN1K_CI_Osc2Amp, 0x80,
	//SSN1K_CI_Osc2DC, 0.0f,
	//SSN1K_CI_Osc2Note, 0.0f,
	SSN1K_CI_Osc2Tune, dd2db(0x4140A3D7),
	//SSN1K_CI_Osc2Fre, 0.0f,
	SSN1K_CI_Osc2Psw, 0x80,
	SSN1K_CI_Osc2Wav, SSN1K_WF_PSAW,

	//SSN1K_CI_Lfo1Mix, SSN1K_MM_MUL,
	SSN1K_CI_Lfo1Amp, dd2db(0x3E833333),
	//SSN1K_CI_Lfo1DC, 0.0f,
	//SSN1K_CI_Lfo1Note, 0.0f,
	//SSN1K_CI_Lfo1Tune,dd2db(0xBF800000),
	SSN1K_CI_Lfo1Fre, dd2db(0x40C33333),
	//SSN1K_CI_Lfo1Psw, 0.8f,
	SSN1K_CI_Lfo1Wav, SSN1K_WF_SIN,

	//SSN1K_CI_Lfo2Mix,SSN1K_MM_MUL,
	SSN1K_CI_Lfo2Amp, dd2db(0x3ACCCCCD),
	//SSN1K_CI_Lfo2DC, 0.0f,
	//SSN1K_CI_Lfo2Note, 0.0f,
	//SSN1K_CI_Lfo1Tune,dd2db(0xBF800000),
	SSN1K_CI_Lfo2Fre, dd2db(0x404CCCCD),
	//SSN1K_CI_Lfo2Psw, 0.0f,
	SSN1K_CI_Lfo2Wav, SSN1K_WF_SIN,

	//SSN1K_CI_FltMix, 0.0f,
	//SSN1K_CI_FltAmp, 0.0f,
	//SSN1K_CI_FltDC, 0.0f,
	SSN1K_CI_FltRes, 0x80,
	SSN1K_CI_FltMode, SSN1K_FM_LP,
	0xFF
};
BYTE solo[] = {
	SSN1K_CI_SynthMix, SSN1K_MM_OVR,
	SSN1K_CI_SynthAmp, 0xFF,
	SSN1K_CI_SynthBal, 0x80,
	//SSN1K_CI_Tune, dd2db(0xC1400000),

	SSN1K_CI_Env1Mix, dd2db(0x3F600000),
	SSN1K_CI_Env1Amp, dd2db(0x3F800000),
	//SSN1K_CI_Env1DC, 0.0f,
	SSN1K_CI_Env1Atk, 0x11,
	SSN1K_CI_Env1Dec, 0x10,
	SSN1K_CI_Env1Sus, 0xA0,
	SSN1K_CI_Env1Rel, 0x60,

	//SSN1K_CI_Env2Mix, 0.0f,
	//SSN1K_CI_Env2Amp, 0.1f,
	//SSN1K_CI_Env2DC, 0.0f,
	//SSN1K_CI_Env2Atk, 0.1f,
	//SSN1K_CI_Env2Dec, 0.2f,
	//SSN1K_CI_Env2Sus, 0.2f,
	//SSN1K_CI_Env2Rel, 0.4f,

	SSN1K_CI_Env3Mix, dd2db(0x3F400000),
	SSN1K_CI_Env3Amp, dd2db(0x44FA0000),
	SSN1K_CI_Env3DC, dd2db(0x42C80000),
	//SSN1K_CI_Env3Amp, dd2db(0x447A0000),
	//SSN1K_CI_Env3DC, dd2db(0x41C80000),
	//SSN1K_CI_Env3Amp, dd2db(0x46FA0000),
	//SSN1K_CI_Env3DC, dd2db(0x437A0000),
	SSN1K_CI_Env3Atk, 0x08,
	SSN1K_CI_Env3Dec, 0x18,
	SSN1K_CI_Env3Sus, 0xA0,
	SSN1K_CI_Env3Rel, 0x40,

	//SSN1K_CI_Env4Mix, SSN1K_MM_BPS,
	//SSN1K_CI_Env4Amp, 0.0f,
	//SSN1K_CI_Env4DC, 0.0f,
	//SSN1K_CI_Env4Atk, 0.0f,
	//SSN1K_CI_Env4Dec, 0.0f,
	//SSN1K_CI_Env4Sus, 0.0f,
	//SSN1K_CI_Env4Rel, 0.0f,
	//SSN1K_CI_Env4Gate, 0.0f,

	//SSN1K_CI_Osc1Mix, CtrlValue(SSN1K_MM_OVR),
	SSN1K_CI_Osc1Amp, 0xC0,
	//SSN1K_CI_Osc1DC, 0.0f,
	//SSN1K_CI_Osc1Note, 0.0f,
	//SSN1K_CI_Osc1Tune, dd2db(0xC1400000),
	//SSN1K_CI_Osc1Fre, 0.0f,
	SSN1K_CI_Osc1Psw, 0x80,
	SSN1K_CI_Osc1Wav, SSN1K_WF_TRI,

	SSN1K_CI_Osc2Mix, SSN1K_MM_ADD,
	SSN1K_CI_Osc2Amp, 0x80,
	//SSN1K_CI_Osc2DC, 0.0f,
	//SSN1K_CI_Osc2Note, 0.0f,
	SSN1K_CI_Osc2Tune, dd2db(0x3DCCCCCD),
	//SSN1K_CI_Osc2Fre, 0.0f,
	SSN1K_CI_Osc2Psw, 0x80,
	SSN1K_CI_Osc2Wav, SSN1K_WF_TRI,

	//SSN1K_CI_Lfo1Mix, SSN1K_MM_MUL,
	SSN1K_CI_Lfo1Amp, dd2db(0x3DCCCCCD),
	//SSN1K_CI_Lfo1DC, 0.0f,
	//SSN1K_CI_Lfo1Note, 0.0f,
	//SSN1K_CI_Lfo1Tune, 0.0f,
	SSN1K_CI_Lfo1Fre, dd2db(0x4059999A),
	//SSN1K_CI_Lfo1Psw, 0.8f,
	SSN1K_CI_Lfo1Wav, SSN1K_WF_SIN,

	//SSN1K_CI_Lfo2Mix,SSN1K_MM_MUL,
	SSN1K_CI_Lfo2Amp, dd2db(0x3C000000),
	//SSN1K_CI_Lfo2DC, 0.0f,
	//SSN1K_CI_Lfo2Note, 0.0f,
	//SSN1K_CI_Lfo2Tune, 0.0f,
	SSN1K_CI_Lfo2Fre, dd2db(0x3E99999A),
	//SSN1K_CI_Lfo2Psw, 0.0f,
	SSN1K_CI_Lfo2Wav, SSN1K_WF_SIN,

	//SSN1K_CI_FltMix, 0.0f,
	//SSN1K_CI_FltAmp, 0.0f,
	//SSN1K_CI_FltDC, 0.0f,
	SSN1K_CI_FltRes, 0x80,
	SSN1K_CI_FltMode, SSN1K_FM_LP,
	0xFF
};
BYTE bass[] = {
	SSN1K_CI_SynthMix, SSN1K_MM_OVR,
	SSN1K_CI_SynthAmp, 0xFF,
	SSN1K_CI_SynthBal, 0x80,
	//SSN1K_CI_Tune, C(5),

	SSN1K_CI_Env1Mix, dd2db(0x3F600000),
	SSN1K_CI_Env1Amp, dd2db(0x3F800000),
	//SSN1K_CI_Env1DC, 0.0f,
	SSN1K_CI_Env1Atk, 0x20,
	SSN1K_CI_Env1Dec, 0x40,
	SSN1K_CI_Env1Sus, 0x80,
	SSN1K_CI_Env1Rel, 0x80,

	//SSN1K_CI_Env2Mix, 0.0f,
	//SSN1K_CI_Env2Amp, 0.1f,
	//SSN1K_CI_Env2DC, 0.0f,
	//SSN1K_CI_Env2Atk, 0.1f,
	//SSN1K_CI_Env2Dec, 0.2f,
	//SSN1K_CI_Env2Sus, 0.2f,
	//SSN1K_CI_Env2Rel, 0.4f,

	SSN1K_CI_Env3Mix, dd2db(0x3F400000),
	SSN1K_CI_Env3Amp, dd2db(0x43FA0000),
	SSN1K_CI_Env3DC, dd2db(0x42C86666),
	SSN1K_CI_Env3Atk, 0xE0,
	SSN1K_CI_Env3Dec, 0x80,
	SSN1K_CI_Env3Sus, 0x20,
	SSN1K_CI_Env3Rel, 0x60,

	//SSN1K_CI_Env4Mix, SSN1K_MM_BPS,
	//SSN1K_CI_Env4Amp, 0.0f,
	//SSN1K_CI_Env4DC, 0.0f,
	//SSN1K_CI_Env4Atk, 0.0f,
	//SSN1K_CI_Env4Dec, 0.0f,
	//SSN1K_CI_Env4Sus, 0.0f,
	//SSN1K_CI_Env4Rel, 0.0f,
	//SSN1K_CI_Env4Gate, 0.0f,

	//SSN1K_CI_Osc1Mix, CtrlValue(SSN1K_MM_OVR),
	SSN1K_CI_Osc1Amp, 0xC0,
	//SSN1K_CI_Osc1DC, 0.0f,
	//SSN1K_CI_Osc1Note, 0.0f,
	//SSN1K_CI_Osc1Tune, 0.0f,
	//SSN1K_CI_Osc1Fre, 0.0f,
	SSN1K_CI_Osc1Psw, 0x80,
	SSN1K_CI_Osc1Wav, SSN1K_WF_PLS,

	SSN1K_CI_Osc2Mix, SSN1K_MM_ADD,
	SSN1K_CI_Osc2Amp, 0x60,
	//SSN1K_CI_Osc2DC, 0.0f,
	//SSN1K_CI_Osc2Note, 0.0f,
	SSN1K_CI_Osc2Tune, dd2db(0x41433333),
	//SSN1K_CI_Osc2Fre, 0.0f,
	SSN1K_CI_Osc2Psw, 0x80,
	SSN1K_CI_Osc2Wav, SSN1K_WF_PLS,

	//SSN1K_CI_Lfo1Mix, SSN1K_MM_MUL,
	SSN1K_CI_Lfo1Amp, dd2db(0x34000000),
	//SSN1K_CI_Lfo1DC, 0.0f,
	//SSN1K_CI_Lfo1Note, 0.0f,
	SSN1K_CI_Lfo1Tune, dd2db(0xBF800000),
	SSN1K_CI_Lfo1Fre, dd2db(0x40400000),
	//SSN1K_CI_Lfo1Psw, 0.8f,
	SSN1K_CI_Lfo1Wav, SSN1K_WF_PLS,

	//SSN1K_CI_Lfo2Mix,SSN1K_MM_MUL,
	SSN1K_CI_Lfo2Amp, dd2db(0x3E800000),
	//SSN1K_CI_Lfo2DC, 0.0f,
	//SSN1K_CI_Lfo2Note, 0.0f,
	//SSN1K_CI_Lfo2Tune, dd2db(0x3C333333),
	SSN1K_CI_Lfo2Fre, dd2db(0x3F000000),
	//SSN1K_CI_Lfo2Psw, 0.0f,
	SSN1K_CI_Lfo2Wav, SSN1K_WF_SIN,

	//SSN1K_CI_FltMix, 0.0f,
	//SSN1K_CI_FltAmp, 0.0f,
	//SSN1K_CI_FltDC, 0.0f,
	SSN1K_CI_FltRes, 0x80,
	SSN1K_CI_FltMode, SSN1K_FM_LP,
	0xFF
};
BYTE bassline[] = {
	SSN1K_CI_SynthMix, SSN1K_MM_OVR,
	SSN1K_CI_SynthAmp, 0xFF,
	SSN1K_CI_SynthBal, 0x80,
	//SSN1K_CI_Tune, C(5),

	SSN1K_CI_Env1Mix, dd2db(0x3F800000),
	SSN1K_CI_Env1Amp, dd2db(0x3F800000),
	//SSN1K_CI_Env1DC, 0.0f,
	SSN1K_CI_Env1Atk, 0x01,
	SSN1K_CI_Env1Dec, 0x28,
	SSN1K_CI_Env1Sus, 0x60,
	SSN1K_CI_Env1Rel, 0x10,

	SSN1K_CI_Env2Mix, dd2db(0x3F400000),
	SSN1K_CI_Env2Amp, dd2db(0x38800000),
	//SSN1K_CI_Env2DC, dd2db(0x3F800000),
	SSN1K_CI_Env2Atk, 0x00,
	SSN1K_CI_Env2Dec, 0x20,
	SSN1K_CI_Env2Sus, 0x40,
	SSN1K_CI_Env2Rel, 0x10,

	SSN1K_CI_Env3Mix, dd2db(0x3F400000),
	SSN1K_CI_Env3Amp, dd2db(0x42960000),
	SSN1K_CI_Env3DC, dd2db(0x42880000),
	SSN1K_CI_Env3Atk, 0x03,
	SSN1K_CI_Env3Dec, 0x10,
	SSN1K_CI_Env3Sus, 0x20,
	SSN1K_CI_Env3Rel, 0x10,

	//SSN1K_CI_Env4Mix, SSN1K_MM_BPS,
	//SSN1K_CI_Env4Amp, 0.0f,
	//SSN1K_CI_Env4DC, 0.0f,
	//SSN1K_CI_Env4Atk, 0.0f,
	//SSN1K_CI_Env4Dec, 0.0f,
	//SSN1K_CI_Env4Sus, 0.0f,
	//SSN1K_CI_Env4Rel, 0.0f,
	//SSN1K_CI_Env4Gate, 0.0f,

	//SSN1K_CI_Osc1Mix, CtrlValue(SSN1K_MM_OVR),
	SSN1K_CI_Osc1Amp, 0xC0,
	//SSN1K_CI_Osc1DC, 0.0f,
	//SSN1K_CI_Osc1Note, 0.0f,
	//SSN1K_CI_Osc1Tune, 0.0f,
	//SSN1K_CI_Osc1Fre, 0.0f,
	SSN1K_CI_Osc1Psw, 0x80,
	SSN1K_CI_Osc1Wav, SSN1K_WF_PSAW,

	SSN1K_CI_Osc2Mix, SSN1K_MM_ADD,
	SSN1K_CI_Osc2Amp, 0x80,
	//SSN1K_CI_Osc2DC, 0.0f,
	//SSN1K_CI_Osc2Note, 0.0f,
	SSN1K_CI_Osc2Tune, dd2db(0x414051EC),
	//SSN1K_CI_Osc2Fre, 0.0f,
	SSN1K_CI_Osc2Psw, 0x84,
	SSN1K_CI_Osc2Wav, SSN1K_WF_PLS,

	//SSN1K_CI_Lfo1Mix, SSN1K_MM_MUL,
	SSN1K_CI_Lfo1Amp, dd2db(0x3C000000),
	//SSN1K_CI_Lfo1DC, 0.0f,
	//SSN1K_CI_Lfo1Note, 0.0f,
	//SSN1K_CI_Lfo1Tune, 0.0f,
	SSN1K_CI_Lfo1Fre, dd2db(0x40400000),
	//SSN1K_CI_Lfo1Psw, 0.8f,
	SSN1K_CI_Lfo1Wav, SSN1K_WF_SIN,

	//SSN1K_CI_Lfo2Mix,SSN1K_MM_MUL,
	SSN1K_CI_Lfo2Amp, dd2db(0x3A800000),
	//SSN1K_CI_Lfo2DC, 0.0f,
	//SSN1K_CI_Lfo2Note, 0.0f,
	//SSN1K_CI_Lfo2Tune, 0.0f,
	SSN1K_CI_Lfo2Fre, dd2db(0x3E99999A),
	//SSN1K_CI_Lfo2Psw, 0.0f,
	SSN1K_CI_Lfo2Wav, SSN1K_WF_SIN,

	//SSN1K_CI_FltMix, 0.0f,
	//SSN1K_CI_FltAmp, 0.0f,
	//SSN1K_CI_FltDC, 0.0f,
	SSN1K_CI_FltRes, 0x80,
	SSN1K_CI_FltMode, SSN1K_FM_LP,
	0xFF
};
BYTE kick[] = {
	SSN1K_CI_SynthMix, SSN1K_MM_OVR,
	SSN1K_CI_SynthAmp, 0xFF,
	SSN1K_CI_SynthBal, 0x80,
	SSN1K_CI_Tune, dd2db(0xC2200000),

	SSN1K_CI_Env1Mix, dd2db(0x3DCCCCCD),
	SSN1K_CI_Env1Amp, dd2db(0x3F800000),
	//SSN1K_CI_Env1DC, 0.0f,
	SSN1K_CI_Env1Atk, 0x00,
	SSN1K_CI_Env1Dec, 0x32,
	SSN1K_CI_Env1Sus, 0x40,
	SSN1K_CI_Env1Rel, 0x10,

	//SSN1K_CI_Env2Mix, 0.0f,
	//SSN1K_CI_Env2Amp, 0.1f,
	//SSN1K_CI_Env2DC, 0.0f,
	//SSN1K_CI_Env2Atk, 0.1f,
	//SSN1K_CI_Env2Dec, 0.2f,
	//SSN1K_CI_Env2Sus, 0.2f,
	//SSN1K_CI_Env2Rel, 0.4f,

	//SSN1K_CI_Env3Mix, dd2db(0x00000000),
	SSN1K_CI_Env3Amp, dd2db(0x457A0000),
	SSN1K_CI_Env3DC, dd2db(0x447A0000),
	//SSN1K_CI_Env3Amp, dd2db(0x447A0000),
	//SSN1K_CI_Env3DC, dd2db(0x42C80000),
	SSN1K_CI_Env3Atk, 0x01,
	SSN1K_CI_Env3Dec, 0x24,
	SSN1K_CI_Env3Sus, 0x10,
	SSN1K_CI_Env3Rel, 0x10,

	//SSN1K_CI_Env4Mix, SSN1K_MM_BPS,
	SSN1K_CI_Env4Amp, dd2db(0x43260000),
	//SSN1K_CI_Env4DC, dd2db(0x42C80000),
	SSN1K_CI_Env4Atk, 0x00,
	SSN1K_CI_Env4Dec, 0x04,
	SSN1K_CI_Env4Sus, 0x08,
	SSN1K_CI_Env4Rel, 0x10,
	//SSN1K_CI_Env4Gate, 0.0f,

	//SSN1K_CI_Osc1Mix, CtrlValue(SSN1K_MM_OVR),
	SSN1K_CI_Osc1Amp, 0xC0,
	//SSN1K_CI_Osc1DC, 0.0f,
	//SSN1K_CI_Osc1Note, 0.0f,
	//SSN1K_CI_Osc1Tune, 0.0f,
	//SSN1K_CI_Osc1Fre, 0.0f,
	//SSN1K_CI_Osc1Psw, 0x80,
	SSN1K_CI_Osc1Wav, SSN1K_WF_SIN,

	SSN1K_CI_Osc2Mix, SSN1K_MM_ADD,
	SSN1K_CI_Osc2Amp, 0x40,
	//SSN1K_CI_Osc2DC, 0.0f,
	//SSN1K_CI_Osc2Note, 0.0f,
	SSN1K_CI_Osc2Tune, dd2db(0x4140A3D7),
	//SSN1K_CI_Osc2Fre, 0.0f,
	//SSN1K_CI_Osc2Psw, 0x48,
	SSN1K_CI_Osc2Wav, SSN1K_WF_SIN,

	//SSN1K_CI_Lfo1Mix, SSN1K_MM_MUL,
	SSN1K_CI_Lfo1Amp, dd2db(0x31000000),
	//SSN1K_CI_Lfo1DC, 0.0f,
	//SSN1K_CI_Lfo1Note, 0.0f,
	//SSN1K_CI_Lfo1Tune, 0.0f,
	SSN1K_CI_Lfo1Fre, dd2db(0x40400000),
	//SSN1K_CI_Lfo1Psw, 0.8f,
	SSN1K_CI_Lfo1Wav, SSN1K_WF_SIN,

	//SSN1K_CI_Lfo2Mix,SSN1K_MM_MUL,
	SSN1K_CI_Lfo2Amp, dd2db(0x00000000),
	//SSN1K_CI_Lfo2DC, 0.0f,
	//SSN1K_CI_Lfo2Note, 0.0f,
	//SSN1K_CI_Lfo2Tune, 0.0f,
	SSN1K_CI_Lfo2Fre, dd2db(0x3E99999A),
	//SSN1K_CI_Lfo2Psw, 0.0f,
	SSN1K_CI_Lfo2Wav, SSN1K_WF_SIN,

	//SSN1K_CI_FltMix, 0.0f,
	//SSN1K_CI_FltAmp, 0.0f,
	//SSN1K_CI_FltDC, 0.0f,
	SSN1K_CI_FltRes, 0x30,
	SSN1K_CI_FltMode, SSN1K_FM_LP,
	0xFF
};
BYTE snare[] = {
	SSN1K_CI_SynthMix, SSN1K_MM_OVR,
	SSN1K_CI_SynthAmp, 0xFF,
	SSN1K_CI_SynthBal, 0x80,
	SSN1K_CI_Tune, dd2db(0xC1600000),

	SSN1K_CI_Env1Mix, dd2db(0x3E800000),
	SSN1K_CI_Env1Amp, dd2db(0x3F000000),
	//SSN1K_CI_Env1DC, 0.0f,
	SSN1K_CI_Env1Atk, 0x01,
	SSN1K_CI_Env1Dec, 0x10,
	SSN1K_CI_Env1Sus, 0x30,
	SSN1K_CI_Env1Rel, 0x10,

	//SSN1K_CI_Env2Mix, 0.0f,
	//SSN1K_CI_Env2Amp, 0.1f,
	//SSN1K_CI_Env2DC, 0.0f,
	//SSN1K_CI_Env2Atk, 0.1f,
	//SSN1K_CI_Env2Dec, 0.2f,
	//SSN1K_CI_Env2Sus, 0.2f,
	//SSN1K_CI_Env2Rel, 0.4f,

	SSN1K_CI_Env3Mix, dd2db(0x3F600000),
	SSN1K_CI_Env3Amp, dd2db(0x437A0000),
	SSN1K_CI_Env3DC, dd2db(0x427A0000),
	SSN1K_CI_Env3Atk, 0x08,
	SSN1K_CI_Env3Dec, 0x14,
	SSN1K_CI_Env3Sus, 0x40,
	SSN1K_CI_Env3Rel, 0x10,

	//SSN1K_CI_Env4Mix, SSN1K_MM_BPS,
	SSN1K_CI_Env4Amp, dd2db(0x42800000),
	//SSN1K_CI_Env4DC, 0.0f,
	SSN1K_CI_Env4Atk, 0x01,
	SSN1K_CI_Env4Dec, 0x03,
	SSN1K_CI_Env4Sus, 0x40,
	SSN1K_CI_Env4Rel, 0x40,

	//SSN1K_CI_Osc1Mix, CtrlValue(SSN1K_MM_OVR),
	SSN1K_CI_Osc1Amp, 0x80,
	//SSN1K_CI_Osc1DC, 0.0f,
	//SSN1K_CI_Osc1Note, 0.0f,
	//SSN1K_CI_Osc1Tune, dd2db(0xC1D00000),
	//SSN1K_CI_Osc1Fre, 0.0f,
	SSN1K_CI_Osc1Psw, 0xA0,
	SSN1K_CI_Osc1Wav, SSN1K_WF_SIN,

	SSN1K_CI_Osc2Mix, SSN1K_MM_MUL ,
	SSN1K_CI_Osc2Amp, 0xE0,
	//SSN1K_CI_Osc2DC, 0.0f,
	//SSN1K_CI_Osc2Note, 0.0f,
	SSN1K_CI_Osc2Tune, dd2db(0x42800000),
	//SSN1K_CI_Osc2Fre, 0.0f,
	//SSN1K_CI_Osc2Psw, 0x78,
	SSN1K_CI_Osc2Wav, SSN1K_WF_RND,

	//SSN1K_CI_Lfo1Mix, SSN1K_MM_MUL,
	//SSN1K_CI_Lfo1Amp, dd2db(0x31000000),
	//SSN1K_CI_Lfo1DC, 0.0f,
	//SSN1K_CI_Lfo1Note, 0.0f,
	//SSN1K_CI_Lfo1Tune, 0.0f,
	//SSN1K_CI_Lfo1Fre, dd2db(0x40400000),
	//SSN1K_CI_Lfo1Psw, 0.8f,
	//SSN1K_CI_Lfo1Wav, SSN1K_WF_SIN,

	//SSN1K_CI_Lfo2Mix,SSN1K_MM_MUL,
	//SSN1K_CI_Lfo2Amp, dd2db(0x31000000),
	//SSN1K_CI_Lfo2DC, 0.0f,
	//SSN1K_CI_Lfo2Note, 0.0f,
	//SSN1K_CI_Lfo2Tune, 0.0f,
	//SSN1K_CI_Lfo2Fre, dd2db(0x3E99999A),
	//SSN1K_CI_Lfo2Psw, 0.0f,
	//SSN1K_CI_Lfo2Wav, SSN1K_WF_SIN,

	//SSN1K_CI_FltMix, 0.0f,
	//SSN1K_CI_FltAmp, 0.0f,
	//SSN1K_CI_FltDC, 0.0f,
	SSN1K_CI_FltRes, 0x88,
	SSN1K_CI_FltMode, SSN1K_FM_LP,
	0xFF
};

BYTE saw[] = {
	SSN1K_CI_SynthMix, SSN1K_MM_OVR,
	SSN1K_CI_SynthAmp, 0xFF,
	SSN1K_CI_SynthBal, 0x80,
	SSN1K_CI_Tune, dd2db(0x31C00000),

	SSN1K_CI_Env1Amp, dd2db(0x3F800000),
	SSN1K_CI_Env1Atk, 0x00,
	SSN1K_CI_Env1Dec, 0x10,
	SSN1K_CI_Env1Sus, 0x80,
	SSN1K_CI_Env1Rel, 0x20,

	SSN1K_CI_Env2Mix, dd2db(0x3F400000),
	SSN1K_CI_Env2Amp, dd2db(0x38800000),
	//SSN1K_CI_Env2DC, dd2db(0x3F800000),
	SSN1K_CI_Env2Atk, 0x00,
	SSN1K_CI_Env2Dec, 0x20,
	SSN1K_CI_Env2Sus, 0x40,
	SSN1K_CI_Env2Rel, 0x10,

	SSN1K_CI_Env3Mix, dd2db(0x3F600000),
	SSN1K_CI_Env3Amp, dd2db(0x447A0000),
	SSN1K_CI_Env3DC, dd2db(0x427A0000),
	SSN1K_CI_Env3Atk, 0x08,
	SSN1K_CI_Env3Dec, 0x14,
	SSN1K_CI_Env3Sus, 0x80,
	SSN1K_CI_Env3Rel, 0x10,


	SSN1K_CI_Osc1Amp, 0xE0,
	SSN1K_CI_Osc1Psw, 0x80,
	SSN1K_CI_Osc1Wav, SSN1K_WF_PSAW,

	SSN1K_CI_Osc2Mix, SSN1K_MM_ADD,
	SSN1K_CI_Osc2Amp, 0x30,
	SSN1K_CI_Osc2Psw, 0x60,
	SSN1K_CI_Osc2Tune, dd2db(0x414051EC),

	//SSN1K_CI_FltMix, SSN1K_MM_BPS,
	SSN1K_CI_FltRes, 0x60,
	SSN1K_CI_FltMode, SSN1K_FM_LP,

	0xFF
};
BYTE pls[] = {
	SSN1K_CI_SynthMix, SSN1K_MM_OVR,
	SSN1K_CI_SynthAmp, 0x80,
	SSN1K_CI_SynthBal, 0x80,
	SSN1K_CI_Tune, dd2db(0x31C00000),

	SSN1K_CI_Env1Amp, dd2db(0x3F800000),
	SSN1K_CI_Env1Atk, 0x08,
	SSN1K_CI_Env1Dec, 0x10,
	SSN1K_CI_Env1Sus, 0x40,
	SSN1K_CI_Env1Rel, 0x20,

	//SSN1K_CI_Env2Mix, dd2db(0),	//dd2db(0x3F400000),
	SSN1K_CI_Env2Amp, dd2db(0x38800000),
	//SSN1K_CI_Env2DC, dd2db(0x3F800000),
	SSN1K_CI_Env2Atk, 0x01,
	SSN1K_CI_Env2Dec, 0x10,
	SSN1K_CI_Env2Sus, 0x40,
	SSN1K_CI_Env2Rel, 0x20,

	SSN1K_CI_Env3Mix, dd2db(0x3F400000),
	SSN1K_CI_Env3Amp, dd2db(0x447A0000),
	SSN1K_CI_Env3DC, dd2db(0x447A0000),
	SSN1K_CI_Env3Atk, 0x08,
	SSN1K_CI_Env3Dec, 0x10,
	SSN1K_CI_Env3Sus, 0xC0,
	SSN1K_CI_Env3Rel, 0x02,


	SSN1K_CI_Osc1Amp, 0x60,
	SSN1K_CI_Osc1Psw, 0x80,
	SSN1K_CI_Osc1Wav, SSN1K_WF_PLS,

	SSN1K_CI_Osc2Mix, SSN1K_MM_BPS,
	SSN1K_CI_Osc2Amp, 0x40,
	SSN1K_CI_Osc2Psw, 0x80,
	SSN1K_CI_Osc2Tune, dd2db(0x414051EC),

	//SSN1K_CI_FltMix, SSN1K_MM_BPS,
	SSN1K_CI_FltRes, 0x10,
	SSN1K_CI_FltMode, SSN1K_FM_LP,
	0xFF
};

// test.xm
//BYTE* instruments[] = {
//	chords, bassline, solo, kick, snare, bass
//};

// 1asu.xm
BYTE* instruments[] = {
	saw, pls, /*chords, solo,*/ bass, bassline, kick, snare,
	pls, saw
};

Ctrl* bank01 = NULL;

int startPattern = 0;
int endPattern = 0;
int voiceCount[] = {
	2, 2, 2, 2,  1, 1, 1, 2,
	2, 2, 2, 2,  2, 2, 2, 2
};
BYTE mixerSettings[] = {
	SSN1K_CI_MixVolume1,	0x80,	SSN1K_CI_MixBalance1,	0x60,
	SSN1K_CI_MixVolume2,	0x40,	SSN1K_CI_MixBalance2,	0x40,
	SSN1K_CI_MixVolume3,	0x40,	SSN1K_CI_MixBalance3,	0x80,
	SSN1K_CI_MixVolume4,	0x20,	SSN1K_CI_MixBalance4,	0xC0,
	SSN1K_CI_MixVolume5,	0x20,	SSN1K_CI_MixBalance5,	0x20,
	SSN1K_CI_MixVolume6,	0x20,	SSN1K_CI_MixBalance6,	0xE0,
	SSN1K_CI_MixVolume7,	0x00,	SSN1K_CI_MixBalance7,	0x20,
	SSN1K_CI_MixVolume8,	0x00,	SSN1K_CI_MixBalance8,	0x80,
	0xFF
};

void soundCallback(short* buffer, int sampleCount) {
	size_t start = 0;
	size_t end = 0;
	size_t remains = sampleCount;
	while (remains) {
		if (frameCounter == 0) {
			player->run(1);
			frameCounter = (size_t)(48000.0f / player->refreshRate());
			frames++;
			print("Frame: %05d\n", frames);
			gotoxy(0, -1);
		}
		size_t len = frameCounter < remains ? frameCounter : remains;
		end = start + len;
		frameCounter -= len;
		mixer->run(buffer, start, end);
		start = end;
		remains -= len;
	}
}
void saveSamples(const char* path) {
	Buffer buffer;
	WORD tmp[4096];
	while (player->masterChannel()->isActive()) {
		soundCallback((short*)tmp, 4096/2);
		buffer.append(tmp, 2 * 4096);
	}
	printf("\n\nWriting %d bytes to %s...\n", buffer.length(), path);
	BYTE* data = buffer.getByteBuffer();
	WaveFmt::write(path, sampleRate, 2, 16, data, buffer.length());
	FREE(data);
}

void setupSequences(Player* player) {
	Sequence* sequence;
	Frame* frame;
	// create main sequence
	sequences[0] = sequence = NEW_(Sequence);
	frame = sequence->addFrame(0);
	IAdapter* adapter = (IAdapter*)player->adapters()->getAt(0);
	frame->addCommand(adapter->createCommand(Player_Cmd_setTempo, 120.0f, 8.0f));
	frame->addCommand(adapter->createCommand(Player_Cmd_assign, 2, 1, Player_Flg_Active | 0x0f));
	frame->addCommand(adapter->createCommand(Player_Cmd_assign, 2, 2, Player_Flg_Active | 0x03));
	//frame->addCommand(adapter->createCommand(Player_Cmd_assign, 1, 3, Player_Flg_Active | 0x03));
	frame = sequence->addFrame(16);
	frame->addCommand(adapter->createEndCommand());
	player->addSequence(sequence);
	AbstractChannel* chn = Player::createChannel();
	chn->init(/*player,*/ 0, (Target*)player->targets()->getAt(0), sequence, Player_Flg_Active);
	player->addChannel(chn);

	// create bass sequence
	sequences[1] = sequence = NEW_(Sequence);
	//frame = sequence->addFrame(0);
	//frame->addCommand(MAKE_COMMAND1(Synth_Cmd_prgChange, 3));
	//frame->addCommand(MAKE_COMMAND2(Synth_Cmd_setNoteOn, C(1), 0xC0));
	//frame = sequence->addFrame(1);
	//frame->addCommand(MAKE_COMMAND1(Synth_Cmd_setNoteOff, C(1)));
	//frame = sequence->addFrame(2);
	//frame->addCommand(MAKE_COMMAND2(Synth_Cmd_setNoteOn, C(2), 0xA0));
	//frame = sequence->addFrame(3);
	//frame->addCommand(MAKE_COMMAND1(Synth_Cmd_setNoteOff, C(2)));
	//frame = sequence->addFrame(4);
	//frame->addCommand(MAKE_COMMAND2(Synth_Cmd_setNoteOn, C(1), 0x80));
	//frame = sequence->addFrame(5);
	//frame->addCommand(MAKE_COMMAND1(Synth_Cmd_setNoteOff, C(1)));
	//frame = sequence->addFrame(6);
	//frame->addCommand(MAKE_COMMAND2(Synth_Cmd_setNoteOn, C(1), 0xA0));
	//frame = sequence->addFrame(7);
	//frame->addCommand(MAKE_COMMAND1(Synth_Cmd_setNoteOff, C(1)));
	//frame = sequence->addFrame(8);
	//frame->addCommand(MAKE_COMMAND2(Synth_Cmd_setNoteOn, C(2), 0xC0));
	//frame = sequence->addFrame(9);
	//frame->addCommand(MAKE_COMMAND1(Synth_Cmd_setNoteOff, C(2)));
	//frame = sequence->addFrame(10);
	//frame->addCommand(MAKE_COMMAND2(Synth_Cmd_setNoteOn, C(1), 0xA0));
	//frame = sequence->addFrame(11);
	//frame->addCommand(MAKE_COMMAND1(Synth_Cmd_setNoteOff, C(1)));
	//frame = sequence->addFrame(12);
	//frame->addCommand(MAKE_COMMAND2(Synth_Cmd_setNoteOn, C(1), 0x80));
	//frame = sequence->addFrame(13);
	//frame->addCommand(MAKE_COMMAND1(Synth_Cmd_setNoteOff, C(1)));
	//frame = sequence->addFrame(14);
	//frame->addCommand(MAKE_COMMAND2(Synth_Cmd_setNoteOn, C(2), 0xA0));
	//frame = sequence->addFrame(15);
	//frame->addCommand(MAKE_COMMAND1(Synth_Cmd_setNoteOff, C(2)));
	//frame = sequence->addFrame(16);
	//frame->addCommand(MAKE_COMMAND0(Player_Cmd_end));

	frame = sequence->addFrame(0);
	frame->addCommand(MAKE_COMMAND1(Synth_Cmd_prgChange, 1));
	frame->addCommand(MAKE_COMMAND2(Synth_Cmd_setNoteOn, C(2), 0xFF));
	frame = sequence->addFrame(2);
	frame->addCommand(MAKE_COMMAND1(Synth_Cmd_setNoteOff, C(2)));
	frame = sequence->addFrame(4);
	frame->addCommand(MAKE_COMMAND2(Synth_Cmd_setNoteOn, C(3), 0xA0));
	frame = sequence->addFrame(6);
	frame->addCommand(MAKE_COMMAND1(Synth_Cmd_setNoteOff, C(3)));
	frame = sequence->addFrame(8);
	frame->addCommand(MAKE_COMMAND0(Player_Cmd_end));

	player->addSequence(sequence);

	// create bass transpose
	sequences[2] = sequence = NEW_(Sequence);
	frame = sequence->addFrame(0);
	frame->addCommand(synthAdapter->createCommand(Synth_Cmd_setControlF, SSN1K_CI_Tune, 0.0));
	frame = sequence->addFrame(16);
	frame->addCommand(synthAdapter->createCommand(Synth_Cmd_setControlF, SSN1K_CI_Tune, 4.0));
	frame = sequence->addFrame(32);
	frame->addCommand(synthAdapter->createCommand(Synth_Cmd_setControlF, SSN1K_CI_Tune, 9.0));
	frame = sequence->addFrame(48);
	frame->addCommand(synthAdapter->createCommand(Synth_Cmd_setControlF, SSN1K_CI_Tune, 7.0));
	frame = sequence->addFrame(64);
	frame->addCommand(MAKE_COMMAND0(Player_Cmd_end));
	player->addSequence(sequence);

	// create chords
	sequences[3] = sequence = NEW_(Sequence);
	frame = sequence->addFrame(0);
	frame->addCommand(MAKE_COMMAND1(Synth_Cmd_prgChange, 0));
	frame->addCommand(MAKE_COMMAND2(Synth_Cmd_setNoteOn, C  (4), 0x80));
	frame->addCommand(MAKE_COMMAND2(Synth_Cmd_setNoteOn, E  (4), 0x80));
	frame->addCommand(MAKE_COMMAND2(Synth_Cmd_setNoteOn, G  (4), 0x80));
	frame = sequence->addFrame(16);
	frame->addCommand(MAKE_COMMAND2(Synth_Cmd_setNoteOn, E  (4), 0x80));
	frame->addCommand(MAKE_COMMAND2(Synth_Cmd_setNoteOn, Gis(4), 0x80));
	frame->addCommand(MAKE_COMMAND2(Synth_Cmd_setNoteOn, H  (4), 0x80));
	frame = sequence->addFrame(32);
	frame->addCommand(MAKE_COMMAND2(Synth_Cmd_setNoteOn, A(4), 0x80));
	frame->addCommand(MAKE_COMMAND2(Synth_Cmd_setNoteOn, C(4), 0x80));
	frame->addCommand(MAKE_COMMAND2(Synth_Cmd_setNoteOn, E(4), 0x80));
	frame = sequence->addFrame(48);
	frame->addCommand(MAKE_COMMAND1(Synth_Cmd_prgChange, 1));
	frame->addCommand(MAKE_COMMAND2(Synth_Cmd_setNoteOn, G(4), 0x80));
	frame->addCommand(MAKE_COMMAND2(Synth_Cmd_setNoteOn, H(4), 0x80));
	frame->addCommand(MAKE_COMMAND2(Synth_Cmd_setNoteOn, D(4), 0x80));
	frame = sequence->addFrame(64);
	frame->addCommand(MAKE_COMMAND0(Player_Cmd_end));
	player->addSequence(sequence);
}
//void createSoundBank() {
//	// create instrument bank
//	int count = arraysize(instruments);
//	bank01 = MALLOC(Ctrl*, count);
//	for (int i = 0; i < count; i++) {
//		// add an instrument
//		bank01[i] = Ctrl::createControls(SSN1K_CI_COUNT);
//		// setup instrument from byte data
//		Synth::setControls(bank01[i], instruments[i]);
//	}
//}
void loadXm(const char* file, PlayerAdapter* playerAdapter, SynthAdapter* synthAdapter) {
	PArray* sequences = NULL;
	XmLoader xmLoader(playerAdapter, synthAdapter);
	if (!xmLoader.load(file)) {
		char* buffer = MALLOC(char, 64*1024);
		LOG("Pattern count: %d\n", xmLoader.patterns().length());
		ARRAY_FOREACH(&xmLoader.patterns(), xmLoader.printPattern((XMPATTERN*)value, buffer); LOG(buffer));
		FREE(buffer);
		sequences = xmLoader.convert();
		// create master channel, sequence
		Sequence* sequence = (Sequence*)sequences->getAt(0);
		player->addSequence(sequence);
		AbstractChannel* chn = Player::createChannel();
		chn->init(/*player,*/ 0, (Target*)player->targets()->getAt(0), sequences->getAt(0), Player_Flg_Active);
		player->addChannel(chn);

		for (UINT32 i = 1; i < sequences->length(); i++) {
			sequence = (Sequence*)sequences->getAt(i);
			player->addSequence(sequence);
		}
		DEL_(sequences);
	} else {
		LOG("File not found!\n");
	}
}
AbstractChannel* createChannel(void) {
	return NEW_(Channel);
}

int _main(NS_FW_BASE::Map* args) {

	console.showCursor(false);
	print("SSN1K test\n");
	//MemoryMgr::isDebugOn = true;

	const char* saveFile = (const char*)args->get("save");
	const char* exportFile = (const char*)args->get("export");
	//const char* startFrame = parseInt((const char*)args->get("start"));
	//const char* endFrame = parseInt((const char*)args->get("end"));
	//int ticks = Integer::parse(args->get(&String("ticks")));

	// setup player
	Player::createChannel = createChannel;
	player = NEW_(Player);
	PlayerAdapter* playerAdapter = (PlayerAdapter*)player->adapters()->getAt(0);
	synthAdapter = NEW_(SynthAdapter);
	player->adapters()->add(synthAdapter);
	player->addUserData(sizeof(instruments), instruments);
	// additional setup
	//player->addUserData(synthBank01Size, synthBank01);
	mixer = NEW_(Mixer);
	int instrumentCount = arraysize(instruments);
	bank01 = Synth::createBank(instrumentCount, instruments);
	//createSoundBank();
	mixer->setControls(mixerSettings);
	for (int i = 0; i < 16; i++) {
		synths[i] = NEW_(Synth, voiceCount[i]);
		synths[i]->bank(bank01);
		player->addTarget(synths[i], synthAdapter);
		mixer->addInput(synths[i]);
	}

	SSN1K::setSampleRate((float)sampleRate);
	//SSN1K::interpolate = SSN1K::smoothstep;
	//SSN1K::interpolate = SSN1K::sinusoid;
	SSN1K::interpolate = SSN1K::flat;

	const char* file = (const char*)args->get("in");
	if (file != NULL) {
		loadXm(file, playerAdapter, synthAdapter);
	} else {
		// setup sequences
		setupSequences(player);
	}

	if (player->masterChannel() != NULL) {

#ifdef _DEBUG
		LOG("#00 Sequence\n");
		PArray* sequences = player->sequences();
		((SynSequence*)sequences->getAt(0))->print((IAdapter*)player->adapters()->getAt(0));
		for (UINT32 i = 1; i < sequences->length(); i++) {
			SynSequence* sequence = (SynSequence*)sequences->getAt(i);
			LOG("#%02d Sequence\n", i);
			sequence->print(synthAdapter);
		}
#endif

		bool play = true;
		if (exportFile && NS_FW_BASE::strlen(exportFile) > 0) {
			Buffer* asuData = NEW_(Buffer);
			Buffer* body = NEW_(Buffer);
			// create header
			// - count of adapter IDs
			// - adapter IDs
			// - count of sequences
			// - sequence lengths
			// - count of user data blocks
			// - lengths of user data blocks
			BYTE buffer[4096];
			BYTE* ptr = buffer;
			DWORD totalLength = 0;
			// write adapter IDs
			*ptr++ = player->adapters()->length();
			ARRAY_FOREACH(player->adapters(), *ptr++ = ((IAdapter*)value)->getId(););
			// write sequence lengths
			*(WORD*)ptr = player->sequences()->length(); ptr += sizeof(WORD);
			// length of master sequence
			totalLength += *(WORD*)ptr = ((SynSequence*)player->sequences()->getAt(0))->toStream((IAdapter*)player->adapters()->getAt(0), body); ptr += sizeof(WORD);
			for (UINT32 i = 1; i < player->sequences()->length(); i++) {
				// lengths of the other sequences (adapter dependant)
				int adapterId = 1;
				totalLength += *(WORD*)ptr = ((SynSequence*)player->sequences()->getAt(i))->toStream((IAdapter*)player->adapters()->getAt(adapterId), body);
				ptr += sizeof(WORD);
			}
			// write user data lengths
			*(WORD*)ptr = player->userData()->length(); ptr += sizeof(WORD);
			for (UINT32 i = 0; i < player->userData()->length(); i++) {
				PLAYER_USER_DATA* userData = (PLAYER_USER_DATA*)player->userData()->getAt(i);
				totalLength += *(DWORD*)ptr = userData->length;
				body->append(userData->data, userData->length);
				ptr += sizeof(DWORD);
			}
			int headerSize = (UINT32)(ptr - buffer);
			totalLength += headerSize;
			asuData->append(buffer, headerSize);
			asuData->append(body);
			DEL_(body);

			LOG("%d total bytes collected\n", totalLength);
			File::write(exportFile, asuData);
			play = false;
			DEL_(asuData);
		}
		if (saveFile && NS_FW_BASE::strlen(saveFile) > 0) {
			saveSamples(saveFile);
			play = false;
		}

		if (play && SoundPlayer::start(sampleRate, 1, soundCallback) == 0) {
			// fast forward to start
			//while (!startPattern) {

			//}

			while (player->masterChannel()->isActive()) {
				Sleep(10);
			}
			printf("\n");
			printf("Stopping audio playback\n");
			SoundPlayer::stop();
		}
		printf("\n\nExiting\n");
	}

	//int count = arraysize(instruments);
	//for (int i = 0; i < count; i++) {
	//	FREE(bank01[i]);
	//}
	FREE(bank01);

	DEL_(mixer);

	DEL_(synthAdapter);
	for (int i = 0; i < 16; i++) {
		DEL_(synths[i]);
	}
	ARRAY_FOREACH(player->sequences(), DEL_((Sequence*)value));
	DEL_(player);

	console.showCursor(true);

	return 0;
}
