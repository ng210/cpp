#include "soundplayer.h"
#include "consoleapp.h"
#include <stdio.h>
#include <math.h>

NS_FW_BASE_USE

void soundCallback(short* pBuffer, int iSamples, void* context);

int _main(NS_FW_BASE::Map* args) {
	printf("SoundPlayer test\n");
	if (SoundPlayer::start(48000, 2, soundCallback) == 0) {
		Sleep(4000);
		SoundPlayer::stop();
	}
	return 0;
}

float theta = 2 * 3.1415926535898f/48000;
int timer = 0;
float f = 1.0f;
void soundCallback(short* pBuffer, int iSamples, void* context) {
	for (int i = 0; i < iSamples; i++) {
		float amp = (float)(8.0f * sin(27.0f * theta * timer));
		float fm1 = (float)(amp * sin(54.0f * theta * timer));
		float fm2 = (float)(amp * sin(56.0f * theta * timer));
		pBuffer[2*i] = (short)(12000 * sin(fm1 + f*110.0f * theta * timer));
		pBuffer[2*i + 1] = (short)floor(12000 * sin(fm2 + f*113.0f * theta * timer));
		timer++;
		f *= 1.000008f;
	}
}