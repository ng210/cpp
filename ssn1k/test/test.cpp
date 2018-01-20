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
const float bpm = 4*60.0f;	// => bpm/60=bps

float theta = 2 * 3.1415926535898f / 48000;
int timer = 0;
float f = 1.0f;
float samplePerBeat = sampleRate * 60.0f / bpm;
float pitch = 0.0f;

int _main(NS_FW_BASE::Map* args) {
	printf("SoundPlayer test\n");
	ctrls.amp.set(0.7f);
	ctrls.dc.set(0.0f);
	ctrls.atk.set(0.1f);
	ctrls.dec.set(0.3f);
	ctrls.sus.set(0.5f);
	ctrls.rel.set(0.5f);

	env.setCtrls((Ctrl*)&ctrls);
	env.bpm(bpm);
	SSN1K::setSampleRate((float)sampleRate);
	//SSN1K::interpolate = SSN1K::smoothstep;
	//SSN1K::interpolate = SSN1K::sinusoid;

	//if (SoundPlayer::start(sampleRate, 1, soundCallback) == 0) {
	//	Sleep((int)floor(1000 * 60 / bpm * 4));
	//	SoundPlayer::stop();
	//}

	saveSamples("sample.wav", (int)floor(1000*60/bpm*4));

	return 0;
}


void soundCallback(LPVOID pBuffer, int iSamples) {
	for (int i = 0; i < iSamples; i++) {
		int beat = (int)samplePerBeat;
		if (timer % beat == 0) {
			env.slopeUp();
		}
		if (timer % beat == (beat>>1)) {
			env.slopeDown();
		}
		if (timer % (2 * beat) == 0) {
			pitch = 110.0f;
		} else {
			if (timer % (2 * beat) == beat) {
				pitch = 220.0;
			}
		}
		float envOut = env.run(0);
		float amp = (float)(sin(27.0f * theta * timer)) * envOut;
		float fm1 = (float)(2*sin(54.0f * theta * timer));
		float fm2 = (float)(2*sin(56.0f * theta * timer + 0.001));
		((short*)pBuffer)[2 * i] = (short)floor(32767.0f * envOut * sin(f*fm1 + pitch * theta * timer));
		((short*)pBuffer)[2 * i + 1] = (short)floor(32767.0f * envOut * sin(f*fm2 + pitch * theta * timer));
		//((short*)pBuffer)[2 * i] = (short)floor(32767.0f * envOut);
		//((short*)pBuffer)[2 * i + 1] = (short)floor(32767.0f * envOut);

		timer++;
		f *= 1.000008f;
	}
}

void saveSamples(const char* path, int ms) {
	int length = (int)floor(sampleRate * ms / 1000.0f);
	printf("length:%d\n", length);
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