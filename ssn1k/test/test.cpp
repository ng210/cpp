#include "fw/base/consoleapp.h"
#include "soundlib/SoundPlayer.h"
#include "soundlib/WavFmt.h"
#include "fw/base/utils/File.h"

#include <stdio.h>
#include <math.h>
#include "ssn1k/ssn1klib.h"

NS_FW_BASE_USE
NS_SSN1K_USE

void soundCallback(LPVOID pBuffer, int iSamples);
void saveSamples(const char* path, int ms);
EnvCtrls ctrls;
Env env;
const int sampleRate = 48000;

int _main(NS_FW_BASE::Map* args) {
	printf("SoundPlayer test\n");
	ctrls.amp.set(0.7f);
	ctrls.dc.set(0.0f);
	ctrls.atk.set(0.01f);
	ctrls.dec.set(0.03f);
	ctrls.sus.set(0.5f);
	ctrls.rel.set(0.02f);

	env.setCtrls((Ctrl*)&ctrls);
	SSN1K::setSampleRate((float)sampleRate);
	SSN1K::interpolate = SSN1K::smoothstep;
	SSN1K::interpolate = SSN1K::sinusoid;

	//if (SoundPlayer::start(sampleRate, 1, soundCallback) == 0) {
	//	Sleep(4000);
	//	SoundPlayer::stop();
	//}

	saveSamples("sample.wav", 4000);

	return 0;
}

float theta = 2 * 3.1415926535898f / 48000;
int timer = 0;
float f = 1.0f;
int beat = sampleRate / 8;
void soundCallback(LPVOID pBuffer, int iSamples) {
	for (int i = 0; i < iSamples; i++) {
		if (timer % beat == 0) {
			env.slopeUp();
		}
		if (timer % beat == (beat/2)) {
			env.slopeDown();
		}
		//env.run(0);
		float amp = (float)(sin(27.0f * theta * timer))*env.run(0.0);
		float fm1 = (float)(sin(54.0f * theta * timer));
		float fm2 = (float)(sin(56.0f * theta * timer));
		//amp = env.run(0.0f);
		//((short*)pBuffer)[2 * i] = (short)(32767 * amp);
		//((short*)pBuffer)[2 * i + 1] = (short)(32767 * amp);
		((short*)pBuffer)[2 * i] = (short)(amp * floor(32767 * sin(fm1 + f*110.0f * theta * timer)));
		((short*)pBuffer)[2 * i + 1] = (short)(amp * floor(32767 * sin(fm2 + f*110.0f * theta * timer)));
		timer++;
		f *= 1.000008f;
	}
}

void saveSamples(const char* path, int ms) {
	int length = sampleRate * ms / 1000;
	char* buffer = MALLOC(char, 2*sizeof(short)*(length + 1));
	soundCallback(buffer, length);
	File::write("sample.raw", buffer, 4*length);

	WaveFmt::write(path, sampleRate, 2, 16, buffer, 4*length);

	//String str(path);
	//WaveFmt waveFmt(str, sampleRate, 2, 16);
	//waveFmt.write(buffer, length);
	//waveFmt.close();

	FREE(buffer);
}