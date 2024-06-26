#ifndef __SOUNDPLAYER_H
#define __SOUNDPLAYER_H

#include "dsound.h"

typedef short* (*FeedSample)(short*, int, void*);

class SoundPlayer {
private:
	static const int defaultLatency_;
	static int channels_;
	static int channelsLog2_;
	static int bitsPerSample_;
	static int sampleSizeLog2_;
	static int bufferSize_;

	static DWORD sampleRate_;
	static DWORD latency_;
	static DWORD threadId_;
	static LPVOID threadContext_;

	static short* sampleBuffer_;
	//static LPVOID callbackBuffer_;

	static LPDIRECTSOUND8 device_;
	static LPDIRECTSOUNDBUFFER primaryBuffer_;
	static LPDIRECTSOUNDBUFFER8 secondaryBuffer_;

	static FeedSample callback_;

	static bool isTerminating_;
	static bool isPlaying_;

	static HRESULT initialize(int sampleRate, int channels);

	static DWORD WINAPI threadProc(LPVOID lpParameter);

public:
	static HRESULT start(int sampleRate, int channels, FeedSample callback, void* context = NULL);
	static HRESULT stop();
	static void setLatency(int ms);
	static void destroy();
};
#endif