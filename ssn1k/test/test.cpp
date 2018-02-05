#include "fw/base/consoleapp.h"
#include "soundlib/SoundPlayer.h"
#include "soundlib/WavFmt.h"
#include "fw/base/utils/File.h"

#include <stdio.h>
#include <math.h>
#include "ssn1k/ssn1klib.h"
#include "player/Player.h"

NS_FW_BASE_USE
NS_SSN1K_USE
NS_PLAYER_USE

//DWtoDB( 0), Synth_Cmd_setControl, 2, SSN1K_CI_Tune, E(1)-E(0),		// 0#define SAVE_MODE

//#define SAVE_MODE

#define TPS (14*4)	// ticks per second
#define TICKS 1*128

const int sampleRate = 48000;

size_t frameCounter = 0;
size_t frames = 0;
Player* player = NULL;
Synth* synths[16];
Mixer* mixer;
SynthAdapter* synthAdapter;

unsigned char mainSequence[] = {
	DWtoDB(0),  Player_Cmd_setTempo, 2, TPS, 1,			// set fps to 4, tpf to 4 (makes 5 tps)
	//DWtoDB(0),  Player_Cmd_assign,	 3, 1, 1, 0x83,		// connect target #1 with sequence #1 with status active
	DWtoDB(0),  Player_Cmd_assign,	 3, 2, 5, 0x83,		// connect target #2 with sequence #1 with status active
	DWtoDB(0),  Player_Cmd_assign,	 3, 2, 6, 0x80,		// connect target #2 with sequence #3 with status active
	DWtoDB(0),  Player_Cmd_assign,	 3, 3, 7, 0x83,		// connect target #3 with sequence #4 with status active

	DWtoDB(128),  Player_Cmd_assign, 3, 1, 2, 0x83,		// connect target #1 with sequence #1 with status active
	DWtoDB(128),  Player_Cmd_assign, 3, 1, 3, 0x83,		// connect target #1 with sequence #1 with status active
	DWtoDB(128),  Player_Cmd_assign, 3, 1, 4, 0x83,		// connect target #1 with sequence #1 with status active
														////DWtoDB(0),  Player_Cmd_assign,	 3, 4, 5, 3,		// connect target #4 with sequence #5 with status active
	//////DWtoDB(0),  Player_Cmd_assign,	 3, 4, 3, 3,		// connect target #2 with sequence #3 with status active
	//////DWtoDB(32),  Player_Cmd_assign,	 3, 4, 6, 1,		// connect target #4 with sequence #6 with status active
	//////DWtoDB(32),  Player_Cmd_assign,	 3, 4, 7, 1,		// connect target #4 with sequence #7 with status active
	//////DWtoDB(32),  Player_Cmd_assign,	 3, 4, 8, 1,		// connect target #4 with sequence #8 with status active
	//DWtoDB(512), Player_Cmd_assign,	 3, 1, 1, 1,		// connect target #1 with sequence #1 with status active
	//DWtoDB(32), Player_Cmd_assign,	 3, 1, 1, 1,		// connect target #1 with sequence #1 with status active
	//DWtoDB(32), Player_Cmd_assign,	 3, 1, 1, 1,		// connect target #1 with sequence #1 with status active
	DWtoDB(128), Player_Cmd_end							// end
};
unsigned char sequence11[] = {
	DWtoDB(16), Synth_Cmd_setNote, 3, E(3), 8, 0x50,
	DWtoDB(0), Synth_Cmd_setNote, 3, G(3), 8, 0x60,
	DWtoDB(0), Synth_Cmd_setNote, 3, H(3), 4, 0x80,
	DWtoDB(16), Player_Cmd_end
};
unsigned char sequence12[] = {
	DWtoDB(16), Synth_Cmd_setNote, 3, H(3), 4, 0x50,
	DWtoDB(0), Synth_Cmd_setNote, 3, D(2), 4, 0x60,
	DWtoDB(0), Synth_Cmd_setNote, 3, Fis(3), 4, 0x80,

	DWtoDB(16), Player_Cmd_end
};
unsigned char sequence13[] = {
	DWtoDB(16), Synth_Cmd_setNote, 3, C(3), 4, 0x50,
	DWtoDB(0), Synth_Cmd_setNote, 3, E(3), 4, 0x60,
	DWtoDB(0), Synth_Cmd_setNote, 3, G(3), 4, 0x80,
	DWtoDB(16), Player_Cmd_end
};
unsigned char sequence14[] = {
	DWtoDB(16), Synth_Cmd_setNote, 3, G(3), 4, 0x80,
	DWtoDB(0), Synth_Cmd_setNote, 3, H(3), 4, 0x60,
	DWtoDB(0), Synth_Cmd_setNote, 3, D(3), 4, 0x40,
	DWtoDB(16), Player_Cmd_end
};

unsigned char sequence5[] = {
	DWtoDB( 0),	Synth_Cmd_setNote, 3, E(1), 8, 0x50,	// 0
	DWtoDB(16), Synth_Cmd_setNote, 3, E(2), 6, 0x70,
	DWtoDB( 8), Synth_Cmd_setNote, 3, E(1), 6, 0x60,
	DWtoDB( 8), Synth_Cmd_setNote, 3, E(0), 8, 0x7F,	// 2
	DWtoDB(16), Synth_Cmd_setNote, 3, E(1), 6, 0x60,
	DWtoDB( 8), Synth_Cmd_setNote, 3, E(0), 6, 0x7F,
	DWtoDB( 8), Synth_Cmd_setNote, 3, E(2), 8, 0x40,	// 4
	DWtoDB(16), Synth_Cmd_setNote, 3, E(1), 6, 0x60,
	DWtoDB(16), Synth_Cmd_setNote, 3, E(0), 6, 0x7F,	// 6
	DWtoDB( 8), Synth_Cmd_setNote, 3, E(1), 6, 0x70,
	DWtoDB( 8), Synth_Cmd_setNote, 3, E(2), 6, 0x60,
	DWtoDB( 8), Synth_Cmd_setNote, 3, E(1), 6, 0x50,
	DWtoDB( 8), Player_Cmd_end						// 0
};
unsigned char sequence6[] = {
	DWtoDB( 0), Synth_Cmd_setControl, 2, SSN1K_CI_Tune, E(1)-E(0),		// 0
	DWtoDB(128), Synth_Cmd_setControl, 2, SSN1K_CI_Tune, H(0)-E(0),		// 16
	DWtoDB(128), Synth_Cmd_setControl, 2, SSN1K_CI_Tune, C(0)-E(0),		// 16
	DWtoDB(128), Synth_Cmd_setControl, 2, SSN1K_CI_Tune, G(1)-E(0),		// 24
	DWtoDB(64), Synth_Cmd_setControl, 2, SSN1K_CI_Tune, Fis(1)-E(0),	// 24
	DWtoDB(64), Player_Cmd_end								// end
};
unsigned char sequence7[] = {
	DWtoDB(0), Synth_Cmd_setNote, 3, E(0), 2, 0x7F,
	DWtoDB(0), Synth_Cmd_setControl, 2, SSN1K_CI_Env1Rel, 0x20,
	DWtoDB(0), Synth_Cmd_setControl, 2, SSN1K_CI_Osc1Amp, 0x7F,
	DWtoDB(0), Synth_Cmd_setControl, 2, SSN1K_CI_Osc2Amp, 0x00,
	DWtoDB(0), Synth_Cmd_setControl, 2, SSN1K_CI_SynthBal, 0x40,

	DWtoDB(16), Synth_Cmd_setNote, 3, E(0), 2, 0x60,
	DWtoDB(0), Synth_Cmd_setControl, 2, SSN1K_CI_Env1Rel, 0x12,

	DWtoDB(16), Synth_Cmd_setNote, 3, E(3), 1, 0x60,
	DWtoDB(0), Synth_Cmd_setControl, 2, SSN1K_CI_Env1Rel, 0x12,
	DWtoDB(0), Synth_Cmd_setControl, 2, SSN1K_CI_Osc1Amp, 0x40,
	DWtoDB(0), Synth_Cmd_setControl, 2, SSN1K_CI_Osc2Amp, 0x20,
	DWtoDB(0), Synth_Cmd_setControl, 2, SSN1K_CI_SynthBal, 0x20,


	DWtoDB(40),Synth_Cmd_setNote, 3, E(0), 2, 0x80,
	DWtoDB(0), Synth_Cmd_setControl, 2, SSN1K_CI_Env1Rel, 0x12,
	DWtoDB(0), Synth_Cmd_setControl, 2, SSN1K_CI_Osc1Amp, 0x7F,
	DWtoDB(0), Synth_Cmd_setControl, 2, SSN1K_CI_Osc2Amp, 0x00,
	DWtoDB(0), Synth_Cmd_setControl, 2, SSN1K_CI_SynthBal, 0x40,

	DWtoDB(8), Synth_Cmd_setNote, 3, E(0), 2, 0x7F,
	DWtoDB(0), Synth_Cmd_setControl, 2, SSN1K_CI_Env1Rel, 0x18,

	DWtoDB(16), Synth_Cmd_setNote, 3, E(3), 2, 0x60,
	DWtoDB(0), Synth_Cmd_setControl, 2, SSN1K_CI_Env1Rel, 0x12,
	DWtoDB(0), Synth_Cmd_setControl, 2, SSN1K_CI_Osc1Amp, 0x40,
	DWtoDB(0), Synth_Cmd_setControl, 2, SSN1K_CI_Osc2Amp, 0x20,
	DWtoDB(0), Synth_Cmd_setControl, 2, SSN1K_CI_SynthBal, 0x60,

	DWtoDB(24), Synth_Cmd_setNote, 3, E(0), 2, 0x60,
	DWtoDB(0), Synth_Cmd_setControl, 2, SSN1K_CI_Env1Rel, 0x12,
	DWtoDB(0), Synth_Cmd_setControl, 2, SSN1K_CI_Osc1Amp, 0x7F,
	DWtoDB(0), Synth_Cmd_setControl, 2, SSN1K_CI_Osc2Amp, 0x00,
	DWtoDB(0), Synth_Cmd_setControl, 2, SSN1K_CI_SynthBal, 0x40,

	DWtoDB(8), Player_Cmd_end
};

CtrlSetting synth1Settings[] = {
	//{ SSN1K_CI_SynthMix, 0.0f },
	{ SSN1K_CI_SynthAmp, 1.0f },
	{ SSN1K_CI_SynthBal, 0.9f },		// = dc

	//{ SSN1K_CI_Env1Mix, 0.0f },
	{ SSN1K_CI_Env1Amp, 0.8f },
	//{ SSN1K_CI_Env1DC, 0.0f },
	{ SSN1K_CI_Env1Atk, 0.02f },
	{ SSN1K_CI_Env1Dec, 0.2f },
	{ SSN1K_CI_Env1Sus, 0.4f },
	{ SSN1K_CI_Env1Rel, 0.6f },

	//{ SSN1K_CI_Env2Mix, 0.0f },
	{ SSN1K_CI_Env2Amp, 0.1f },
	//{ SSN1K_CI_Env2DC, 0.0f },
	{ SSN1K_CI_Env2Atk, 0.1f },
	{ SSN1K_CI_Env2Dec, 0.2f },
	{ SSN1K_CI_Env2Sus, 0.2f },
	{ SSN1K_CI_Env2Rel, 0.4f },

	//{ SSN1K_CI_Env3Mix, 0.0f },
	{ SSN1K_CI_Env3Amp, 5000.0f },
	{ SSN1K_CI_Env3DC, 2500.0f },
	{ SSN1K_CI_Env3Atk, 0.05f },
	{ SSN1K_CI_Env3Dec, 0.4f },
	{ SSN1K_CI_Env3Sus, 0.8f },
	{ SSN1K_CI_Env3Rel, 0.8f },

	{ SSN1K_CI_Env4Mix, CtrlValue(SSN1K_MM_BPS) },
	//{ SSN1K_CI_Env4Amp, 0.0f },
	//{ SSN1K_CI_Env4DC, 0.0f },
	//{ SSN1K_CI_Env4Atk, 0.0f },
	//{ SSN1K_CI_Env4Dec, 0.0f },
	//{ SSN1K_CI_Env4Sus, 0.0f },
	//{ SSN1K_CI_Env4Rel, 0.0f },
	//{ SSN1K_CI_Env4Gate, 0.0f },

	{ SSN1K_CI_Osc1Mix, CtrlValue(SSN1K_MM_OVR) },
	{ SSN1K_CI_Osc1Amp, 1.0f },
	//{ SSN1K_CI_Osc1DC, 0.0f },
	//{ SSN1K_CI_Osc1Note, 0.0f },
	{ SSN1K_CI_Osc1Tune, 12.0f },
	{ SSN1K_CI_Osc1Fre, 0.0f },
	{ SSN1K_CI_Osc1Psw, 0.5f },
	{ SSN1K_CI_Osc1Wav, CtrlValue(SSN1K_WF_PSAW) },

	//{ SSN1K_CI_Osc2Mix, CtrlValue(SSN1K_MM_MUL) },
	{ SSN1K_CI_Osc2Amp, 0.8f },
	//{ SSN1K_CI_Osc2DC, 0.0f },
	//{ SSN1K_CI_Osc2Note, 0.0f },
	{ SSN1K_CI_Osc2Tune, 12.02f },
	{ SSN1K_CI_Osc2Fre, 0.0f },
	{ SSN1K_CI_Osc2Psw, 0.8f },
	{ SSN1K_CI_Osc2Wav, CtrlValue(SSN1K_WF_PSAW) },

	//{ SSN1K_CI_Lfo1Mix, CtrlValue(SSN1K_MM_MUL) },
	{ SSN1K_CI_Lfo1Amp, 6.0f },
	//{ SSN1K_CI_Lfo1DC, 0.0f },
	//{ SSN1K_CI_Lfo1Note, 0.0f },
	//{ SSN1K_CI_Lfo1Tune, 0.0f },
	{ SSN1K_CI_Lfo1Fre, 4.0f },
	//{ SSN1K_CI_Lfo1Psw, 0.8f },
	{ SSN1K_CI_Lfo1Wav, CtrlValue(SSN1K_WF_SIN) },

	//{ SSN1K_CI_Lfo2Mix,CtrlValue(SSN1K_MM_MUL) },
	{ SSN1K_CI_Lfo2Amp, 0.1f },
	//{ SSN1K_CI_Lfo2DC, 0.0f },
	//{ SSN1K_CI_Lfo2Note, 0.0f },
	//{ SSN1K_CI_Lfo2Tune, 0.0f },
	{ SSN1K_CI_Lfo2Fre, 2.5f },
	//{ SSN1K_CI_Lfo2Psw, 0.0f },
	{ SSN1K_CI_Lfo2Wav, CtrlValue(SSN1K_WF_SIN) },

	//{ SSN1K_CI_FltMix, 0.0f },
	//{ SSN1K_CI_FltAmp, 0.0f },
	//{ SSN1K_CI_FltDC, 0.0f },
	{ SSN1K_CI_FltRes, 0.1f },
	{ SSN1K_CI_FltMode, CtrlValue(SSN1K_FM_LP) },
	{ -1, -1 }
};
CtrlSetting synth2Settings[] = {
	//{ SSN1K_CI_SynthMix, 0.0f },
	{ SSN1K_CI_SynthAmp, 1.0f },
	{ SSN1K_CI_SynthBal, 0.5f },		// = dc

	{ SSN1K_CI_Env1Mix, 0.9f },
	{ SSN1K_CI_Env1Amp, 1.0f },
	//{ SSN1K_CI_Env1DC, 0.0f },
	{ SSN1K_CI_Env1Atk, 0.01f },
	{ SSN1K_CI_Env1Dec, 0.1f },
	{ SSN1K_CI_Env1Sus, 0.5f },
	{ SSN1K_CI_Env1Rel, 0.2f },

	//{ SSN1K_CI_Env2Mix, 0.0f },
	{ SSN1K_CI_Env2Amp, 0.5f },
	//{ SSN1K_CI_Env2DC, 0.0f },
	{ SSN1K_CI_Env2Atk, 0.01f },
	{ SSN1K_CI_Env2Dec, 0.2f },
	{ SSN1K_CI_Env2Sus, 0.3f },
	{ SSN1K_CI_Env2Rel, 0.2f },

	{ SSN1K_CI_Env3Mix, 0.8f },
	{ SSN1K_CI_Env3Amp, 200.0f },
	{ SSN1K_CI_Env3DC, 20.0f },
	{ SSN1K_CI_Env3Atk, 0.003f },
	{ SSN1K_CI_Env3Dec, 0.1f },
	{ SSN1K_CI_Env3Sus, 0.4f },
	{ SSN1K_CI_Env3Rel, 0.2f },

	//{ SSN1K_CI_Env4Mix, 0.0f },
	//{ SSN1K_CI_Env4Amp, 800.0f },
	//{ SSN1K_CI_Env4DC, 0.0f },
	//{ SSN1K_CI_Env4Atk, 0.0f },
	//{ SSN1K_CI_Env4Dec, 0.1f },
	//{ SSN1K_CI_Env4Sus, 0.5f },
	//{ SSN1K_CI_Env4Rel, 0.2f },

	//{ SSN1K_CI_Osc1Mix, CtrlValue(SSN1K_MM_MUL) },
	{ SSN1K_CI_Osc1Amp, 1.0f },
	//{ SSN1K_CI_Osc1DC, 0.0f },
	//{ SSN1K_CI_Osc1Note, 0.0f },
	{ SSN1K_CI_Osc1Tune, 0.0f },
	{ SSN1K_CI_Osc1Fre, 0.0f },
	{ SSN1K_CI_Osc1Psw, 0.2f },
	{ SSN1K_CI_Osc1Wav, CtrlValue(SSN1K_WF_PSAW) },

	{ SSN1K_CI_Osc2Mix, CtrlValue(SSN1K_MM_ADD) },
	{ SSN1K_CI_Osc2Amp, 0.6f },
	//{ SSN1K_CI_Osc2DC, 0.0f },
	//{ SSN1K_CI_Osc2Note, 0.0f },
	{ SSN1K_CI_Osc2Tune, 0.1f },
	{ SSN1K_CI_Osc2Fre, 0.0f },
	{ SSN1K_CI_Osc2Psw, 0.3f },
	{ SSN1K_CI_Osc2Wav, CtrlValue(SSN1K_WF_PLS) },

	//{ SSN1K_CI_Lfo1Mix, CtrlValue(SSN1K_MM_MUL) },
	//{ SSN1K_CI_Lfo1Amp, 8.0f },
	//{ SSN1K_CI_Lfo1DC, 0.0f },
	//{ SSN1K_CI_Lfo1Note, 0.0f },
	//{ SSN1K_CI_Lfo1Tune, 0.0f },
	//{ SSN1K_CI_Lfo1Fre, 6.0f },
	//{ SSN1K_CI_Lfo1Psw, 0.8f },
	//{ SSN1K_CI_Lfo1Wav, CtrlValue(SSN1K_WF_SIN) },

	//{ SSN1K_CI_Lfo2Mix,CtrlValue(SSN1K_MM_MUL) },
	//{ SSN1K_CI_Lfo2Amp, 0.0f },
	//{ SSN1K_CI_Lfo2DC, 0.0f },
	//{ SSN1K_CI_Lfo2Note, 0.0f },
	//{ SSN1K_CI_Lfo2Tune, 0.0f },
	//{ SSN1K_CI_Lfo2Fre, 0.0f },
	//{ SSN1K_CI_Lfo2Psw, 0.0f },
	//{ SSN1K_CI_Lfo2Wav, CtrlValue(SSN1K_WF_SIN) },

	//{ SSN1K_CI_FltMix, 0.0f },
	//{ SSN1K_CI_FltAmp, 0.0f },
	//{ SSN1K_CI_FltDC, 0.0f },
	{ SSN1K_CI_FltRes, 0.6f },
	{ SSN1K_CI_FltMode, CtrlValue(SSN1K_FM_LP) },
	{ -1, -1 }
};
CtrlSetting synth3Settings[] = {
	//{ SSN1K_CI_SynthMix, 0.0f },
	{ SSN1K_CI_SynthAmp, 1.6f },
	{ SSN1K_CI_SynthBal, 0.5f },		// = dc

	{ SSN1K_CI_Env1Mix, 0.5f },
	{ SSN1K_CI_Env1Amp, 0.8f },
	//{ SSN1K_CI_Env1DC, 0.0f },
	{ SSN1K_CI_Env1Atk, 0.0002f },
	{ SSN1K_CI_Env1Dec, 0.01f },
	{ SSN1K_CI_Env1Sus, 0.6f },
	{ SSN1K_CI_Env1Rel, 0.14f },

	//{ SSN1K_CI_Env2Mix, 0.0f },
	//{ SSN1K_CI_Env2Amp, 0.3f },
	//{ SSN1K_CI_Env2DC, 0.0f },
	//{ SSN1K_CI_Env2Atk, 0.01f },
	//{ SSN1K_CI_Env2Dec, 0.1f },
	//{ SSN1K_CI_Env2Sus, 0.3f },
	//{ SSN1K_CI_Env2Rel, 0.2f },

	//{ SSN1K_CI_Env3Mix, 0.0f },
	{ SSN1K_CI_Env3Amp, 2000.0f },
	{ SSN1K_CI_Env3DC, 100.0f },
	{ SSN1K_CI_Env3Atk, 0.01f },
	{ SSN1K_CI_Env3Dec, 0.1f },
	{ SSN1K_CI_Env3Sus, 0.8f },
	{ SSN1K_CI_Env3Rel, 0.4f },

	//{ SSN1K_CI_Env4Mix, CtrlValue(SSN1K_MM_ADD) },
	{ SSN1K_CI_Env4Amp, 200.0f },
	{ SSN1K_CI_Env4DC, 20.0f },
	{ SSN1K_CI_Env4Atk, 0.01f },
	{ SSN1K_CI_Env4Dec, 0.02f },
	{ SSN1K_CI_Env4Sus, 0.3f },
	{ SSN1K_CI_Env4Rel, 0.1f },

	//{ SSN1K_CI_Osc1Mix, CtrlValue(SSN1K_MM_MUL) },
	{ SSN1K_CI_Osc1Amp, 1.0f },
	//{ SSN1K_CI_Osc1DC, 0.0f },
	//{ SSN1K_CI_Osc1Note, 0.0f },
	//{ SSN1K_CI_Osc1Tune, 0.0f },
	//{ SSN1K_CI_Osc1Fre, 0.0f },
	//{ SSN1K_CI_Osc1Psw, 0.2f },
	{ SSN1K_CI_Osc1Wav, CtrlValue(SSN1K_WF_SIN) },

	{ SSN1K_CI_Osc2Mix, CtrlValue(SSN1K_MM_ADD) },
	//{ SSN1K_CI_Osc2Amp, 0.0f },
	//{ SSN1K_CI_Osc2DC, 0.0f },
	//{ SSN1K_CI_Osc2Note, 0.0f },
	//{ SSN1K_CI_Osc2Tune, 0.1f },
	{ SSN1K_CI_Osc2Fre, 10000.0f },
	//{ SSN1K_CI_Osc2Psw, 0.45f },
	{ SSN1K_CI_Osc2Wav, CtrlValue(SSN1K_WF_RND) },

	//{ SSN1K_CI_Lfo1Mix, CtrlValue(SSN1K_MM_MUL) },
	//{ SSN1K_CI_Lfo1Amp, 8.0f },
	//{ SSN1K_CI_Lfo1DC, 0.0f },
	//{ SSN1K_CI_Lfo1Note, 0.0f },
	//{ SSN1K_CI_Lfo1Tune, 0.0f },
	//{ SSN1K_CI_Lfo1Fre, 6.0f },
	//{ SSN1K_CI_Lfo1Psw, 0.8f },
	//{ SSN1K_CI_Lfo1Wav, CtrlValue(SSN1K_WF_SIN) },

	//{ SSN1K_CI_Lfo2Mix,CtrlValue(SSN1K_MM_MUL) },
	//{ SSN1K_CI_Lfo2Amp, 0.0f },
	//{ SSN1K_CI_Lfo2DC, 0.0f },
	//{ SSN1K_CI_Lfo2Note, 0.0f },
	//{ SSN1K_CI_Lfo2Tune, 0.0f },
	//{ SSN1K_CI_Lfo2Fre, 0.0f },
	//{ SSN1K_CI_Lfo2Psw, 0.0f },
	//{ SSN1K_CI_Lfo2Wav, CtrlValue(SSN1K_WF_SIN) },

	//{ SSN1K_CI_FltMix, 0.0f },
	//{ SSN1K_CI_FltAmp, 0.0f },
	//{ SSN1K_CI_FltDC, 0.0f },
	{ SSN1K_CI_FltRes, 0.4f },
	{ SSN1K_CI_FltMode, CtrlValue(SSN1K_FM_LP) },
	{ -1, -1 }
};
CtrlSetting* synthSettings[] = {
	&synth1Settings[0], &synth2Settings[0], &synth3Settings[0]
};
int voiceCount[] = { 6, 1, 4 };
CtrlSetting mixerSettings[] = {
	{ SSN1K_CI_MixVolume1,	0.2f },
	{ SSN1K_CI_MixBalance1, 0.2f },
	{ SSN1K_CI_MixVolume2,	0.4f },
	{ SSN1K_CI_MixBalance2, 0.3f },
	{ SSN1K_CI_MixVolume3,	1.0f },
	{ SSN1K_CI_MixBalance3, 0.5f },
	{ -1, -1 }
};
void soundCallback(short* buffer, int sampleCount) {
	size_t start = 0;
	size_t end = 0;
	size_t remains = sampleCount;
	while (remains) {
		if (frameCounter == 0) {
			player->run(1);
			frameCounter = 48000 / player->refreshRate();
			frames++;
		}
		size_t len = frameCounter < remains ? frameCounter : remains;
		end = start + len;
		frameCounter -= len;
		mixer->run(buffer, start, end);
		start = end;
		remains -= len;
	}
}

void saveSamples(const char* path, int ticks) {
	int length = (int)floor(ticks * (float)sampleRate / (float)TPS);
	printf("length:%d\n", length);
	char* buffer = MALLOC(char, 2 * sizeof(short)*(length + 1));
	for (size_t i = 0; i < length; i++) {
		((short*)buffer)[i] = 0;
	}
	soundCallback((short*)buffer, length);

	printf("Writing data to %s...\n", path);
	WaveFmt::write(path, sampleRate, 2, 16, buffer, 4 * length);

	//String str(path);
	//WaveFmt waveFmt(str, sampleRate, 2, 16);
	//waveFmt.write(buffer, length);
	//waveFmt.close();

	FREE(buffer);
}

int _main(NS_FW_BASE::Map* args) {
	printf("SSN1K test\n");
	//MemoryMgr::isDebugOn = true;

	bool saveMode = args->get(&String("save")) != Null;
	//int ticks = Integer::parse(args->get(&String("ticks")));

	for (int i = 0; i < 16; i++) {
		synths[i] = NULL;
	}
	player = NEW_(Player);
	synthAdapter = NEW_(SynthAdapter, player);
	mixer = NEW_(Mixer);
	mixer->setup(mixerSettings);

	for (int i = 0; i < 3; i++) {
		synths[i] = NEW_(Synth, voiceCount[i]);
		synths[i]->setup(synthSettings[i]);
		player->addTarget(synths[i], synthAdapter);
		mixer->addInput(synths[i]);
	}

	SSN1K::setSampleRate((float)sampleRate);
	SSN1K::interpolate = SSN1K::smoothstep;
	//SSN1K::interpolate = SSN1K::sinusoid;

	player->addSequence(mainSequence);
	player->addSequence(sequence11);
	player->addSequence(sequence12);
	player->addSequence(sequence13);
	player->addSequence(sequence14);
	player->addSequence(sequence5);
	player->addSequence(sequence6);
	player->addSequence(sequence7);

	if (!saveMode) {
		if (SoundPlayer::start(sampleRate, 1, soundCallback) == 0) {
			while (player->masterChannel()->isActive() &&
				frames < TICKS) {
				Sleep(10);
			}
			printf("\n");
			printf("Stopping audio playback\n");
			SoundPlayer::stop();
		}
		printf("Exiting\n");
	} else {
		saveSamples("sample.wav", TICKS);
	}
	DEL_(synthAdapter);
	DEL_(mixer);
	for (int i = 0; i < 16; i++) {
		DEL_(synths[i]);
	}
	DEL_(player);

	return 0;
}
