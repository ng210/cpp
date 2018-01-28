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

//#define SAVE_MODE

#define TPS 18	// ticks per second

const int sampleRate = 48000;

size_t frameCounter = 0;
Player* player = NULL;
Synth* synths[4];
SynthAdapter* synthAdapter;
//Array* sequences[16];
unsigned char sequence1[] = {
	DWtoDB(0),  Player_Cmd_setTempo, 2, TPS, 1,			// set fps to 4, tpf to 4 (makes 5 tps)
	DWtoDB(0),  Player_Cmd_assign,	 3, 1, 1, 3,		// connect target #0 with sequence #1 with status active
	DWtoDB(0),  Player_Cmd_assign,	 3, 2, 2, 3,		// connect target #0 with sequence #1 with status active
	DWtoDB(128), Player_Cmd_assign,	 3, 1, 1, 1,		// connect target #0 with sequence #1 with status active
	DWtoDB(32), Player_Cmd_assign,	 3, 1, 1, 1,		// connect target #0 with sequence #1 with status active
	DWtoDB(32), Player_Cmd_assign,	 3, 1, 1, 1,		// connect target #0 with sequence #1 with status active
	DWtoDB(32), Player_Cmd_end							// end
};
unsigned char sequence2[] = {
	// 0, 1, 0, 1, 0, 0, 1, 0
	DWtoDB( 4), Synth_Cmd_setNote, 3, C(3), 6, 60,		//  4
	DWtoDB( 0), Synth_Cmd_setNote, 3, E(3), 6, 80,
	DWtoDB( 0), Synth_Cmd_setNote, 3, G(3), 6, 40,

	DWtoDB( 8), Synth_Cmd_setNote, 3, G(3), 6, 50,		// 12
	DWtoDB( 0), Synth_Cmd_setNote, 3, H(2), 6, 60,
	DWtoDB( 0), Synth_Cmd_setNote, 3, D(3), 6, 80,

	DWtoDB(12), Synth_Cmd_setNote, 3, G(3), 6, 50,		// 24
	DWtoDB( 0), Synth_Cmd_setNote, 3, H(2), 6, 60,
	DWtoDB( 0), Synth_Cmd_setNote, 3, D(3), 6, 80,

	DWtoDB(12), Synth_Cmd_setNote, 3, F(3), 6, 80,		// 36
	DWtoDB( 0), Synth_Cmd_setNote, 3, A(3), 6, 60,
	DWtoDB( 0), Synth_Cmd_setNote, 3, C(3), 6, 40,

	DWtoDB( 8), Synth_Cmd_setNote, 3, G(3), 6, 50,		// 44
	DWtoDB( 0), Synth_Cmd_setNote, 3, H(2), 6, 60,
	DWtoDB( 0), Synth_Cmd_setNote, 3, D(3), 6, 80,

	DWtoDB(12), Synth_Cmd_setNote, 3, C(3), 6, 80,		// 56
	DWtoDB( 0), Synth_Cmd_setNote, 3, E(3), 6, 60,
	DWtoDB( 0), Synth_Cmd_setNote, 3, G(2), 6, 40,
	DWtoDB( 8), Player_Cmd_end
};
unsigned char sequence3[] = {
	DWtoDB(0), Synth_Cmd_setNote, 3, C(2), 2, 70,	// 0
	DWtoDB(4), Synth_Cmd_setNote, 3, C(3), 2, 60,
	DWtoDB(4), Synth_Cmd_setNote, 3, C(1), 2, 50,	// 2
	DWtoDB(4), Synth_Cmd_setNote, 3, C(2), 2, 60,
	DWtoDB(4), Synth_Cmd_setNote, 3, C(3), 2, 60,	// 4
	DWtoDB(4), Synth_Cmd_setNote, 3, C(2), 2, 50,
	DWtoDB(4), Synth_Cmd_setNote, 3, C(1), 2, 70,	// 6
	DWtoDB(4), Synth_Cmd_setNote, 3, C(3), 2, 60,
	DWtoDB(4), Player_Cmd_end						// 0
};

CtrlSetting synth1Settings[] = {
	//{ SSN1K_CI_SynthMix, 0.0f },
	{ SSN1K_CI_SynthAmp, 0.7f },
	{ SSN1K_CI_SynthBal, 0.8f },		// = dc

	//{ SSN1K_CI_Env1Mix, 0.0f },
	{ SSN1K_CI_Env1Amp, 0.3f },
	//{ SSN1K_CI_Env1DC, 0.0f },
	{ SSN1K_CI_Env1Atk, 0.03f },
	{ SSN1K_CI_Env1Dec, 0.1f },
	{ SSN1K_CI_Env1Sus, 0.3f },
	{ SSN1K_CI_Env1Rel, 0.3f },

	//{ SSN1K_CI_Env2Mix, 0.0f },
	{ SSN1K_CI_Env2Amp, 0.6f },
	//{ SSN1K_CI_Env2DC, 0.0f },
	{ SSN1K_CI_Env2Atk, 0.1f },
	{ SSN1K_CI_Env2Dec, 0.2f },
	{ SSN1K_CI_Env2Sus, 0.8f },
	{ SSN1K_CI_Env2Rel, 0.4f },

	//{ SSN1K_CI_Env3Mix, 0.0f },
	{ SSN1K_CI_Env3Amp, 1500.0f },
	{ SSN1K_CI_Env3DC, 1500.0f },
	{ SSN1K_CI_Env3Atk, 0.1f },
	{ SSN1K_CI_Env3Dec, 0.2f },
	{ SSN1K_CI_Env3Sus, 0.6f },
	{ SSN1K_CI_Env3Rel, 0.3f },

	//{ SSN1K_CI_Env4Mix, 0.0f },
	//{ SSN1K_CI_Env4Amp, 0.0f },
	//{ SSN1K_CI_Env4DC, 0.0f },
	//{ SSN1K_CI_Env4Atk, 0.0f },
	//{ SSN1K_CI_Env4Dec, 0.0f },
	//{ SSN1K_CI_Env4Sus, 0.0f },
	//{ SSN1K_CI_Env4Rel, 0.0f },
	//{ SSN1K_CI_Env4Gate, 0.0f },

	{ SSN1K_CI_Osc1Mix, CtrlValue(SSN1K_MM_MUL) },
	{ SSN1K_CI_Osc1Amp, 0.6f },
	//{ SSN1K_CI_Osc1DC, 0.0f },
	{ SSN1K_CI_Osc1Note, 0.0f },
	{ SSN1K_CI_Osc1Tune, 12.0f },
	{ SSN1K_CI_Osc1Fre, 0.0f },
	{ SSN1K_CI_Osc1Psw, 0.2f },
	{ SSN1K_CI_Osc1Wav, CtrlValue(SSN1K_WF_PSAW) },

	{ SSN1K_CI_Osc2Mix, CtrlValue(SSN1K_MM_MUL) },
	{ SSN1K_CI_Osc2Amp, 0.4f },
	//{ SSN1K_CI_Osc2DC, 0.0f },
	{ SSN1K_CI_Osc2Note, 0.0f },
	{ SSN1K_CI_Osc2Tune, 12.07f },
	{ SSN1K_CI_Osc2Fre, 0.0f },
	{ SSN1K_CI_Osc2Psw, 0.2f },
	{ SSN1K_CI_Osc2Wav, CtrlValue(SSN1K_WF_PSAW) },

	//{ SSN1K_CI_Lfo1Mix, CtrlValue(SSN1K_MM_MUL) },
	{ SSN1K_CI_Lfo1Amp, 4.0f },
	//{ SSN1K_CI_Lfo1DC, 0.0f },
	//{ SSN1K_CI_Lfo1Note, 0.0f },
	//{ SSN1K_CI_Lfo1Tune, 0.0f },
	{ SSN1K_CI_Lfo1Fre, 6.0f },
	//{ SSN1K_CI_Lfo1Psw, 0.8f },
	{ SSN1K_CI_Lfo1Wav, CtrlValue(SSN1K_WF_SIN) },

	//{ SSN1K_CI_Lfo2Mix,CtrlValue(SSN1K_MM_MUL) },
	{ SSN1K_CI_Lfo2Amp, 0.2f },
	//{ SSN1K_CI_Lfo2DC, 0.0f },
	//{ SSN1K_CI_Lfo2Note, 0.0f },
	//{ SSN1K_CI_Lfo2Tune, 0.0f },
	{ SSN1K_CI_Lfo2Fre, 0.5f },
	//{ SSN1K_CI_Lfo2Psw, 0.0f },
	{ SSN1K_CI_Lfo2Wav, CtrlValue(SSN1K_WF_SIN) },

	//{ SSN1K_CI_FltMix, 0.0f },
	//{ SSN1K_CI_FltAmp, 0.0f },
	//{ SSN1K_CI_FltDC, 0.0f },
	{ SSN1K_CI_FltRes, 0.5f },
	{ SSN1K_CI_FltMode, CtrlValue(SSN1K_FM_LP) },
	{ -1, -1 }
};
CtrlSetting synth2Settings[] = {
	//{ SSN1K_CI_SynthMix, 0.0f },
	{ SSN1K_CI_SynthAmp, 0.7f },
	{ SSN1K_CI_SynthBal, 0.4f },		// = dc

	//{ SSN1K_CI_Env1Mix, 0.0f },
	{ SSN1K_CI_Env1Amp, 0.8f },
	//{ SSN1K_CI_Env1DC, 0.0f },
	{ SSN1K_CI_Env1Atk, 0.01f },
	{ SSN1K_CI_Env1Dec, 0.3f },
	{ SSN1K_CI_Env1Sus, 0.7f },
	{ SSN1K_CI_Env1Rel, 0.4f },

	//{ SSN1K_CI_Env2Mix, 0.0f },
	{ SSN1K_CI_Env2Amp, 0.3f },
	//{ SSN1K_CI_Env2DC, 0.0f },
	{ SSN1K_CI_Env2Atk, 0.01f },
	{ SSN1K_CI_Env2Dec, 0.1f },
	{ SSN1K_CI_Env2Sus, 0.3f },
	{ SSN1K_CI_Env2Rel, 0.2f },

	//{ SSN1K_CI_Env3Mix, 0.0f },
	{ SSN1K_CI_Env3Amp, 500.0f },
	{ SSN1K_CI_Env3DC, 200.0f },
	{ SSN1K_CI_Env3Atk, 0.01f },
	{ SSN1K_CI_Env3Dec, 0.1f },
	{ SSN1K_CI_Env3Sus, 0.2f },
	{ SSN1K_CI_Env3Rel, 0.3f },

	//{ SSN1K_CI_Env4Mix, 0.0f },
	//{ SSN1K_CI_Env4Amp, 0.0f },
	//{ SSN1K_CI_Env4DC, 0.0f },
	//{ SSN1K_CI_Env4Atk, 0.0f },
	//{ SSN1K_CI_Env4Dec, 0.0f },
	//{ SSN1K_CI_Env4Sus, 0.0f },
	//{ SSN1K_CI_Env4Rel, 0.0f },
	//{ SSN1K_CI_Env4Gate, 0.0f },

	{ SSN1K_CI_Osc1Mix, CtrlValue(SSN1K_MM_MUL) },
	{ SSN1K_CI_Osc1Amp, 0.4f },
	//{ SSN1K_CI_Osc1DC, 0.0f },
	{ SSN1K_CI_Osc1Note, 0.0f },
	{ SSN1K_CI_Osc1Tune, 0.0f },
	{ SSN1K_CI_Osc1Fre, 0.0f },
	{ SSN1K_CI_Osc1Psw, 0.2f },
	{ SSN1K_CI_Osc1Wav, CtrlValue(SSN1K_WF_PLS) },

	{ SSN1K_CI_Osc2Mix, CtrlValue(SSN1K_MM_MUL) },
	{ SSN1K_CI_Osc2Amp, 0.3f },
	//{ SSN1K_CI_Osc2DC, 0.0f },
	{ SSN1K_CI_Osc2Note, 0.0f },
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
	{ SSN1K_CI_Lfo2Amp, 0.0f },
	//{ SSN1K_CI_Lfo2DC, 0.0f },
	//{ SSN1K_CI_Lfo2Note, 0.0f },
	//{ SSN1K_CI_Lfo2Tune, 0.0f },
	{ SSN1K_CI_Lfo2Fre, 0.0f },
	//{ SSN1K_CI_Lfo2Psw, 0.0f },
	{ SSN1K_CI_Lfo2Wav, CtrlValue(SSN1K_WF_SIN) },

	//{ SSN1K_CI_FltMix, 0.0f },
	//{ SSN1K_CI_FltAmp, 0.0f },
	//{ SSN1K_CI_FltDC, 0.0f },
	{ SSN1K_CI_FltRes, 0.2f },
	{ SSN1K_CI_FltMode, CtrlValue(SSN1K_FM_LP) },
	{ -1, -1 }
};

void soundCallback(LPVOID buffer, int sampleCount) {
	for (size_t i = 0; i < sampleCount; i++) {
		((dword*)buffer)[i] = 0;
	}
	size_t start = 0;
	size_t end = 0;
	size_t remains = sampleCount;
	while (remains) {
		if (frameCounter == 0) {
			player->run(1);
			frameCounter = 48000 / player->refreshRate();
			//printf(".");
		}
		size_t len = frameCounter < remains ? frameCounter : remains;
		end = start + len;
		frameCounter -= len;
		synths[0]->run(buffer, start, end);
		synths[1]->run(buffer, start, end);
		//((short*)buffer)[2*i] = smp;
		//((short*)buffer)[2*i + 1] = smp;
		start = end;
		remains -= len;
	}
}

void saveSamples(const char* path, int ticks) {
	int length = (int)floor(ticks * sampleRate / TPS);
	printf("length:%d\n", length);
	char* buffer = MALLOC(char, 2 * sizeof(short)*(length + 1));
	for (size_t i = 0; i < length; i++) {
		((short*)buffer)[i] = 0;
	}
	soundCallback(buffer, length);

	printf("Writing data...");
	WaveFmt::write(path, sampleRate, 2, 16, buffer, 4 * length);

	//String str(path);
	//WaveFmt waveFmt(str, sampleRate, 2, 16);
	//waveFmt.write(buffer, length);
	//waveFmt.close();

	FREE(buffer);
}

int _main(NS_FW_BASE::Map* args) {
	printf("SSN1K test\n");
	
	player = NEW_(Player);
	synths[0] = NEW_(Synth, 4);
	synths[1] = NEW_(Synth, 1);

	SSN1K::setSampleRate((float)sampleRate);
	SSN1K::interpolate = SSN1K::smoothstep;
	//SSN1K::interpolate = SSN1K::sinusoid;

	synths[0]->setup(synth1Settings);
	synths[1]->setup(synth2Settings);

	synthAdapter = NEW_(SynthAdapter, player);
	player->addTarget(synths[0], synthAdapter);
	player->addTarget(synths[1], synthAdapter);

	player->addSequence(sequence1);
	player->addSequence(sequence2);
	player->addSequence(sequence3);

#ifndef SAVE_MODE
	if (SoundPlayer::start(sampleRate, 1, soundCallback) == 0) {
		while (player->getCurrentFrame() < 128) {
			Sleep(10);
		}
		printf("\n");
		printf("Stopping audio playback\n");
		SoundPlayer::stop();
	}
	printf("Exiting\n");
#else
	saveSamples("sample.wav", 64);
#endif
	DEL_(synthAdapter);
	DEL_(synths[0]);
	DEL_(synths[1]);
	DEL_(player);

	return 0;
}
