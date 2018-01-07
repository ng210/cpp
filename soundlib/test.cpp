#include "SoundPlayer.h"
#include "consoleapp.h"
#include <stdio.h>
#include <math.h>
#include "easySound.h"

NS_FW_BASE_USE

void soundCallback(LPVOID pBuffer, int iSamples);

int _main(NS_FW_BASE::Map* args) {
	printf("SoundPlayer test\n");
	if (SoundPlayer::start(48000, 1, soundCallback) == 0) {
		//EasySound_Start(&soundCallback, 48000, 1, 0);
		Sleep(4000);
		SoundPlayer::stop();
		//EasySound_Stop();
	}
	return 0;
}

float theta = 2 * 3.1415926535898f/48000;
int timer = 0;
void soundCallback(LPVOID pBuffer, int iSamples) {
	for (int i = 0; i < iSamples; i++) {
		float amp = (float)40.0f * sin(0.05 * theta * timer);
		float fm1 = (float)(amp * sin(55.1f * theta * timer));
		float fm2 = (float)(amp * sin(54.9f * theta * timer));
		((short*)pBuffer)[2*i] = (short)floor(12000 * sin(fm1 +440.0f * theta * timer));
		((short*)pBuffer)[2*i + 1] = (short)floor(12000 * sin(fm2 + 110.0f * theta * timer));
		timer++;
	}
}