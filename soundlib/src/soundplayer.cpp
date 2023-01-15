#include "soundplayer.h"

#include "initguid.h"
#include "cguid.h"
#include "Mmreg.h"
#include <stdio.h>


const int SoundPlayer::defaultLatency_ = 80;	// latency in ms
int SoundPlayer::channels_ = 2;
int SoundPlayer::channelsLog2_ = 1;
int SoundPlayer::bitsPerSample_ = 16;
int SoundPlayer::sampleSizeLog2_ = 2;	// 8bit stereo = 2*2 = 4 byte per sample
int SoundPlayer::bufferSize_ = 65536;
DWORD SoundPlayer::sampleRate_ = 48000;

LPDIRECTSOUND8 SoundPlayer::device_ = NULL;
LPDIRECTSOUNDBUFFER SoundPlayer::primaryBuffer_ = NULL;
LPDIRECTSOUNDBUFFER8 SoundPlayer::secondaryBuffer_ = NULL;

short* SoundPlayer::sampleBuffer_ = NULL;
//float* SoundPlayer::callbackBuffer_ = NULL;
DWORD SoundPlayer::latency_ = 80;
DWORD SoundPlayer::threadId_ = NULL;
LPVOID SoundPlayer::threadContext_ = NULL;

bool SoundPlayer::isTerminating_ = false;
bool SoundPlayer::isPlaying_ = false;
FeedSample SoundPlayer::callback_ = NULL;


DWORD WINAPI SoundPlayer::threadProc(LPVOID lpParameter) {
	SoundPlayer::isPlaying_ = true;
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
	DWORD writePosition = 2048;
	DWORD playPosition = 0;
	DWORD sampleCount = SoundPlayer::bufferSize_ >> SoundPlayer::sampleSizeLog2_;
	DWORD mask = sampleCount - 1;
	while (!SoundPlayer::isTerminating_) {
		SoundPlayer::secondaryBuffer_->GetCurrentPosition(&playPosition, 0);
		playPosition >>= SoundPlayer::sampleSizeLog2_;	// byte => sample count
		DWORD aheadPosition = playPosition + SoundPlayer::latency_;
		if (playPosition > writePosition) {
			writePosition += sampleCount;
		}
		int count = 256;
		int countCh = 256 * SoundPlayer::channels_;
		while (aheadPosition > writePosition) {
			DWORD ix = writePosition & mask;
			ix <<= SoundPlayer::channelsLog2_;
			short* start = &(SoundPlayer::sampleBuffer_[ix]);
			for (int i = 0; i < countCh; i++) start[i] = 0;
			SoundPlayer::callback_(start, count, SoundPlayer::threadContext_);
			writePosition += count;
		}
		writePosition &= mask;
		//Sleep(6);
	}
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);
	SoundPlayer::isPlaying_ = false;
	return 0;
}

HRESULT SoundPlayer::initialize(int sampleRate, int channels) {
	HRESULT res = DS_OK;
	isTerminating_ = false;
	isPlaying_ = false;

	while (true) {
		if (device_ == NULL) {
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
		}

		if (SoundPlayer::secondaryBuffer_ == NULL || sampleRate != SoundPlayer::sampleRate_ || channels != SoundPlayer::channels_) {
			LPDIRECTSOUNDBUFFER8& buffer = SoundPlayer::secondaryBuffer_;
			if (buffer != NULL) {
				// release buffer
				buffer->Release();
			}
			// create new buffer
			buffer = NULL;
			SoundPlayer::sampleRate_ = sampleRate;
			SoundPlayer::sampleSizeLog2_ = 1;	// mono, 16 bit
			if (channels < 2) {
				SoundPlayer::channels_ = 1;
				SoundPlayer::channelsLog2_ = 0;
			} else {
				SoundPlayer::channels_ = 2;
				SoundPlayer::channelsLog2_ = 1;
				SoundPlayer::sampleSizeLog2_++;	// stereo, 16 bit
			}

			// set up WAV format structure
			// and DSBUFFERDESC structure
			DSBUFFERDESC dsbdesc;
			WAVEFORMATEX wfx;
			{
				wfx.cbSize = sizeof(WAVEFORMATEX);
				wfx.wFormatTag = WAVE_FORMAT_PCM;
				wfx.nSamplesPerSec = SoundPlayer::sampleRate_;
				wfx.nChannels = SoundPlayer::channels_;
				wfx.wBitsPerSample = SoundPlayer::bitsPerSample_;
				wfx.nBlockAlign = SoundPlayer::channels_ * (SoundPlayer::bitsPerSample_ >> 3);
				wfx.nAvgBytesPerSec = SoundPlayer::sampleRate_ * wfx.nBlockAlign;
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
			}
			// get buffer address
			DWORD tmp;
			if ((res = buffer->Lock(0, SoundPlayer::bufferSize_, (LPVOID*)&SoundPlayer::sampleBuffer_, &tmp, 0, 0, 0)) != DS_OK) {
				break;
			}
			if ((res = buffer->Unlock(SoundPlayer::sampleBuffer_, SoundPlayer::bufferSize_, 0, 0)) != DS_OK) {
				break;
			}
		}

		// reset playback position
		SoundPlayer::secondaryBuffer_->SetCurrentPosition(0);
		DWORD sampleCount = SoundPlayer::bufferSize_ >> SoundPlayer::sampleSizeLog2_;
		for (DWORD i = 0; i < sampleCount; i++) {
			SoundPlayer::sampleBuffer_[i] = 0;
		}

		// create audio thread
		HANDLE hThread = CreateThread(0, 0x1000, &SoundPlayer::threadProc, NULL, NULL, &SoundPlayer::threadId_);

		SoundPlayer::setLatency(SoundPlayer::defaultLatency_);

		break;
	}
	return res;
}

HRESULT SoundPlayer::start(int sampleRate, int channels, FeedSample callback, void* context) {
	HRESULT res = DS_OK;
	SoundPlayer::initialize(sampleRate, channels);
	SoundPlayer::callback_ = callback;
	SoundPlayer::threadContext_ = context;
	return SoundPlayer::secondaryBuffer_->Play(0, 0, DSBPLAY_LOOPING);
}

HRESULT SoundPlayer::stop(void) {
	HRESULT res = NULL;
	if (SoundPlayer::secondaryBuffer_ != NULL) {
		// signal stop to audio thread
		SoundPlayer::isTerminating_ = true;
		// wait for thread to finish
		while (SoundPlayer::isPlaying_) {
			Sleep(1);
		}
		res = SoundPlayer::secondaryBuffer_->Stop();
	}
	return res;
}

void SoundPlayer::destroy() {
	if (SoundPlayer::secondaryBuffer_) SoundPlayer::secondaryBuffer_->Release();
	if (SoundPlayer::device_) SoundPlayer::device_->Release();
	SoundPlayer::isTerminating_ = false;
}

void SoundPlayer::setLatency(int ms) {
	SoundPlayer::latency_ = SoundPlayer::sampleRate_ * ms / 1000;
}

