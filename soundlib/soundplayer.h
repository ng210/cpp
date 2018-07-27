#ifndef __SOUNDPLAYER_H
#define __SOUNDPLAYER_H

#include <windows.h>
#include "dsound.h"
#ifdef __CRT
#include "crt\crt.h"
#endif

//#include "base/memory.h"
//#include "types.h"

//#define USE_DOUBLE_BUFFER

typedef void (*FeedSample)(short*, int);

class SoundPlayer {
private:
	static const int channels_;
	static const int bitsPerSample_;
	static const int bufferSize_;

	static DWORD sampleRate_;
	static DWORD latency_;

	static short* sampleBuffer_;
	//static LPVOID callbackBuffer_;

	static LPDIRECTSOUND8 device_;
	static LPDIRECTSOUNDBUFFER primaryBuffer_;
	static LPDIRECTSOUNDBUFFER8 secondaryBuffer_;

	static FeedSample callback_;

	static bool isTerminating_;
	static bool isPlaying_;

	static DWORD WINAPI threadProc(LPVOID lpParameter);

public:
	static HRESULT start(int sampleRate, int channels, void(*callback)(short* buffer, int sampleCount));
	static HRESULT stop();
	static void setLatency(int ms);
};
#endif