#include "soundplayer.h"

#include "initguid.h"
#include "cguid.h"
#include "Mmreg.h"
#include <stdio.h>

const int SoundPlayer::channels_ = 2;
const int SoundPlayer::bitsPerSample_ = 16;
const int SoundPlayer::bufferSize_ = 65536;
DWORD SoundPlayer::sampleRate_ = 48000;

LPDIRECTSOUND8 SoundPlayer::device_ = NULL;
LPDIRECTSOUNDBUFFER SoundPlayer::primaryBuffer_ = NULL;
LPDIRECTSOUNDBUFFER8 SoundPlayer::secondaryBuffer_ = NULL;

short* SoundPlayer::sampleBuffer_ = NULL;
//float* SoundPlayer::callbackBuffer_ = NULL;
DWORD SoundPlayer::latency_ = 80;

bool SoundPlayer::isTerminating_ = false;
bool SoundPlayer::isPlaying_ = false;
FeedSample SoundPlayer::callback_ = NULL;


DWORD WINAPI SoundPlayer::threadProc(LPVOID lpParameter) {
	SoundPlayer::isPlaying_ = true;
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
	DWORD writePosition = 2048;
	DWORD playPosition = 0;
	DWORD sampleCount = SoundPlayer::bufferSize_ >> 2;
	DWORD mask = sampleCount - 1;
	while (!SoundPlayer::isTerminating_) {
		SoundPlayer::secondaryBuffer_->GetCurrentPosition(&playPosition, 0);
		playPosition >>= 2;
		DWORD aheadPosition = playPosition + SoundPlayer::latency_;
		if (playPosition > writePosition) {
			writePosition += sampleCount;
		}
		while (aheadPosition > writePosition) {
			DWORD ix = writePosition & mask;
			SoundPlayer::callback_(&((short*)SoundPlayer::sampleBuffer_)[ix*2], 256);
			writePosition += 256;
		}
		writePosition &= mask;
		Sleep(6);
	}
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);
	SoundPlayer::isPlaying_ = false;
	return 0;
}

//HRESULT SoundPlayer::start(int sampleRate, int channels, FeedSample callback) {
HRESULT SoundPlayer::start(int sampleRate, int channels, void (*callback)(short* buffer, int sampleCount)) {
	HRESULT res = DS_OK;
	LPDIRECTSOUNDBUFFER8 &buffer = SoundPlayer::secondaryBuffer_;
	buffer = NULL;
	while (true) {
		// load dll
		HMODULE dll = LoadLibraryA("dsound.dll");
		if (dll == NULL) {
			res = GetLastError();
			break;
		}
		// get DirectSoundCreate process
		LPDIRECTSOUNDCREATE8 pDirectSoundCreate = (LPDIRECTSOUNDCREATE8)GetProcAddress(dll, "DirectSoundCreate8");
		// create device
		if ((res = pDirectSoundCreate(0, &SoundPlayer::device_, 0)) != DS_OK) {
			break;
		}
		// set cooperative level
		HWND hWnd = GetDesktopWindow();
		if ((res = SoundPlayer::device_->SetCooperativeLevel(hWnd, DSSCL_PRIORITY)) != DS_OK) {
			break;
		}
		// set up WAV format structure
		WAVEFORMATEX wfx;
		wfx.cbSize = sizeof(WAVEFORMATEX);
		wfx.wFormatTag = WAVE_FORMAT_PCM;
		wfx.nSamplesPerSec = sampleRate;
		wfx.nChannels = SoundPlayer::channels_;
		wfx.wBitsPerSample = SoundPlayer::bitsPerSample_;
		wfx.nBlockAlign = SoundPlayer::channels_*(SoundPlayer::bitsPerSample_ >> 3);
		wfx.nAvgBytesPerSec = sampleRate * wfx.nBlockAlign;
		// set up DSBUFFERDESC structure
		DSBUFFERDESC dsbdesc;
		dsbdesc.dwSize = sizeof(DSBUFFERDESC);
		dsbdesc.guid3DAlgorithm = GUID_NULL;
		dsbdesc.dwReserved = 0;
		// create primary buffer
		LPDIRECTSOUNDBUFFER tmpBuffer;
		// create secondary buffer
		dsbdesc.dwBufferBytes = SoundPlayer::bufferSize_;
		dsbdesc.lpwfxFormat = &wfx;
		dsbdesc.dwFlags = DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_GLOBALFOCUS;
		if ((res = SoundPlayer::device_->CreateSoundBuffer(&dsbdesc, &tmpBuffer, 0)) != DS_OK) {
			break;
		}
		if ((res = tmpBuffer->QueryInterface(IID_IDirectSoundBuffer8, (LPVOID*)&buffer)) != DS_OK) {
			tmpBuffer->Release();
			break;
		}
		// get buffer address
		DWORD tmp;
		if ((res = buffer->Lock(0, SoundPlayer::bufferSize_, (LPVOID*)&sampleBuffer_, &tmp, 0, 0, 0)) != DS_OK) {
			break;
		}
		if ((res = buffer->Unlock(sampleBuffer_, SoundPlayer::bufferSize_, 0, 0)) != DS_OK) {
			break;
		}
		if ((res = buffer->Play(0, 0, DSBPLAY_LOOPING)) != DS_OK) {
			break;
		}
		SoundPlayer::sampleRate_ = sampleRate;
		SoundPlayer::callback_ = callback;
		// create audio thread
		DWORD id = 0;
		CreateThread(0, 0x1000, &SoundPlayer::threadProc, NULL, 0, &id);
		setLatency(SoundPlayer::latency_);
		break;
	}
	if (res != DS_OK) {
		if (buffer != NULL) {
			buffer->Release();
		}
	}
	return res;
}
HRESULT SoundPlayer::stop(void) {
	HRESULT res = DS_OK;
	// signal stop to audio thread
	SoundPlayer::isTerminating_ = true;
	// wait for thread to finish
	while (SoundPlayer::isPlaying_) {
		Sleep(1);
	}
	res = SoundPlayer::secondaryBuffer_->Stop();

	SoundPlayer::secondaryBuffer_->Release();
	SoundPlayer::device_->Release();
	return res;
}
void SoundPlayer::setLatency(int ms) {
	SoundPlayer::latency_ = SoundPlayer::sampleRate_ * ms / 1000;
}

