#include "soundplayer.h"
#include "wavfmt.h"
#include "consoleapp.h"
#include <stdio.h>
#include <math.h>
#include "base/memory.h"

NS_FW_BASE_USE

void soundCallback(short* pBuffer, int iSamples, void* context);

int _main(NS_FW_BASE::Map* args) {
	printf("SoundPlayer test\n");
	if (SoundPlayer::start(48000, 2, soundCallback) == 0) {
		Sleep(7600);
		SoundPlayer::stop();
	}

	printf("Wavefmt test\n");
	var wave = NEW_(WaveFmt, "test.wav", 48000, 2, 16);
	const int size = 16000;
	var buffer = MALLOC(short, 2*size);
	for (var i = 0; i < 48000/size; i++) {
		soundCallback(buffer, size, NULL);
		wave->write((byte*)buffer, sizeof(word) * size);
	}
	wave->close();
	DEL_(wave);
	FREE(buffer);

	SoundPlayer::destroy();
	return 0;
}

float theta = 2 * 3.1415926535898f/48000;
int timer = 0;
float f = 1.0f;
void soundCallback(short* pBuffer, int iSamples, void* context) {
	var j = 0;
	for (int i = 0; i < iSamples; i++) {
		float amp = (float)(8.0f * sin(0.07f * theta * timer));
		float fm1 = (float)(amp * sin(110.7f * theta * timer));
		float fm2 = (float)(amp * sin(55.0f * theta * timer));
		pBuffer[j++] = (short)(12000 * sin(fm1 + f*110.0f * theta * timer));
		pBuffer[j++] = (short)floor(12000 * sin(fm2 + f*113.0f * theta * timer));
		timer++;
		f *= 1.00000f;
	}
}