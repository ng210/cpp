#ifndef __SOUNDPLAYER_H
#define __SOUNDPLAYER_H

#include <windows.h>
#ifdef __CRT
#include "crt\crt.h"
#endif
#include "initguid.h"
#include "cguid.h"
#include "Mmreg.h"
#include "dsound.h"

//#define USE_DOUBLE_BUFFER

typedef void (*FeedSample)(LPVOID, int);

class SoundPlayer {
private:
	static const int channels_;
	static const int bitsPerSample_;
	static const int bufferSize_;

	static DWORD sampleRate_;
	static DWORD latency_;

	static LPVOID sampleBuffer_;
	static LPVOID callbackBuffer_;

	static LPDIRECTSOUND8 device_;
	static LPDIRECTSOUNDBUFFER primaryBuffer_;
	static LPDIRECTSOUNDBUFFER8 secondaryBuffer_;

	static FeedSample callback_;

	static bool isTerminating_;
	static bool isPlaying_;

	static DWORD WINAPI threadProc(LPVOID lpParameter);

public:
	static HRESULT start(int sampleRate, int channels, FeedSample callback);
	static HRESULT stop();
	static void setLatency(int ms);
};
#endif