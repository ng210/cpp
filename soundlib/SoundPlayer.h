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
typedef HRESULT (WINAPI *LPDIRECTSOUNDCREATE8)(LPCGUID, LPDIRECTSOUND8*, LPUNKNOWN);

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
	static LPDIRECTSOUNDBUFFER8 buffer_;

	static FeedSample callback_;

	static bool isTerminating_;
	static bool isPlaying_;

	static DWORD WINAPI threadProc(LPVOID lpParameter);

public:
	static HRESULT start(int sampleRate, int channels, FeedSample callback);
	static HRESULT stop();
	static void setLatency(int ms);

//	static DWORD iBufferSize_;
//	static DWORD iSamplingRate_;
//	//static DWORD iBytesPerSecond_;
//
//	static HMODULE hDSoundDll_;
//	//static LPDIRECTSOUNDCREATE8 pDirectSoundCreate_;
//
//	IDirectSound8* pDevice_;
//	IDirectSoundBuffer8* pBuffer_;
//
//	bool isThreadTerminated_;
//
//	DWORD iThreadId_;
//	HANDLE hThread_;
//
//	FeedSample pCallBack_;
//
//	HRESULT createBuffer(int iSamplingRate, int iChannels, int iBitsPerSample, int iBufferSize, LPDIRECTSOUNDBUFFER8* ppDsb8);
//
//public:
//	static DWORD WINAPI threadProc(LPVOID lpParameter);
//	int iError_;
//	int isRunning_;
//	DWORD iLatency_;
//
//	SoundPlayer(void);
//	~SoundPlayer(void);
//
//	HRESULT init(int iSamplingRate, int iChannels, int iBitsPerSample, int iLatency, FeedSample callBack, HWND hWnd);
//
//	void reset();
//	void start();
//	int pause();
//	void restart();
//	void stop();
//	void clear();
};
#endif