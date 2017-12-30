#include "SoundPlayer.h"
#include "consoleapp.h"
#include <stdio.h>
#include <math.h>

NS_FW_BASE_USE

void soundCallback(LPVOID pBuffer, int iSamples);

int _main(NS_FW_BASE::Map* args) {
	printf("SoundPlayer test\n");
	SoundPlayer::start(48000, 1, soundCallback);
	Sleep(2000);
	SoundPlayer::stop();
	return 0;
}

float theta = 1382.30076758f/48000;
int timer = 0;

void soundCallback(LPVOID pBuffer, int iSamples) {
	for (int i = 0; i < iSamples; i++) {
		((short*)pBuffer)[i] = (short)floor(12000 * sin(theta * timer));
		timer++;
	}
}