#include "SoundPlayer.h"
#include "base/MemoryMgr.h"
#include <stdio.h>

NS_FW_BASE_USE

//HMODULE SoundPlayer::hDSoundDll_ = 0;
//LPDIRECTSOUNDCREATE8 SoundPlayer::pDirectSoundCreate_ = 0;

const int SoundPlayer::channels_ = 2;
const int SoundPlayer::bitsPerSample_ = 16;
const int SoundPlayer::bufferSize_ = 65536;
DWORD SoundPlayer::sampleRate_ = 48000;

LPDIRECTSOUND8 SoundPlayer::device_ = NULL;
LPDIRECTSOUNDBUFFER SoundPlayer::primaryBuffer_ = NULL;
LPDIRECTSOUNDBUFFER8 SoundPlayer::secondaryBuffer_ = NULL;

LPVOID SoundPlayer::sampleBuffer_ = NULL;
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

HRESULT SoundPlayer::start(int sampleRate, int channels, FeedSample callback) {
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
		//dsbdesc.dwBufferBytes = 0;
		//dsbdesc.lpwfxFormat = NULL;
		//dsbdesc.dwFlags = DSBCAPS_PRIMARYBUFFER;
		//if ((res = SoundPlayer::device_->CreateSoundBuffer(&dsbdesc, &SoundPlayer::primaryBuffer_, 0)) != DS_OK) {
		//	break;
		//}
		//if ((res = SoundPlayer::primaryBuffer_->SetFormat(&wfx)) != DS_OK) {
		//	break;
		//}
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
		if ((res = buffer->Lock(0, SoundPlayer::bufferSize_, &sampleBuffer_, &tmp, 0, 0, 0)) != DS_OK) {
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
//
//#ifdef USE_DOUBLE_BUFFER	
//DWORD SoundPlayer::iBufferSize_ = 8192;
//
//DWORD WINAPI SoundPlayer::threadProc(LPVOID lpParameter) {
//	SoundPlayer* sp = (SoundPlayer*)lpParameter;
//	DWORD iPlayPos, unused;
//	DWORD iBufferHalf = SoundPlayer::iBufferSize_>>1;
//	DWORD iPhase = 0;
//	BYTE *pBuffer, *pBufferHalf;
//
//	sp->iError_ = sp->pBuffer_->Lock(0, SoundPlayer::iBufferSize_, (LPVOID*)&pBuffer, &unused, 0, 0, DSBLOCK_ENTIREBUFFER);
//	memset(pBuffer, 0, SoundPlayer::iBufferSize_);
//	//iBufferHalf = SoundPlayer::iBufferSize_>>1;
//	pBufferHalf = pBuffer + iBufferHalf;
//
//	while (!sp->isThreadTerminated_) {
//		if (sp->isRunning_) {
//			// poll play pointer
//			sp->pBuffer_->GetCurrentPosition(&iPlayPos, 0);
//			if (iPhase == 0) {
//				// 1st half of the buffer is being played
//				if (iPlayPos < iBufferHalf) {
//					// fill the 2nd half
//					(*sp->pCallBack_)(pBufferHalf, iBufferHalf);
//					iPhase = 1;
//				}
//			} else {
//				// 2nd half of the buffer is being played
//				if (iPlayPos > iBufferHalf) {
//					// fill the 1st half
//					(*sp->pCallBack_)(pBuffer, iBufferHalf);
//					iPhase = 0;
//				}
//			}
//		}
//		// Sleep thread
//		Sleep(4);
//	}
//	sp->isThreadTerminated_= true;
//	sp->iError_ = sp->pBuffer_->Unlock(pBuffer, SoundPlayer::iBufferSize_, 0, 0);
//	return 0;
//}
//
//#else
//DWORD SoundPlayer::iBufferSize_ = 65536;
//
//DWORD WINAPI SoundPlayer::threadProc(LPVOID lpParameter) {
//	SoundPlayer *sp = (SoundPlayer*)lpParameter;
//	DWORD unused;
//	LPBYTE pBuffer = NULL;
//	DWORD iPlayPosition = 0;
//	DWORD iWriteLength = 0;
//	DWORD iLatencyInSamples = sp->iSamplingRate_ * sp->iLatency_ / 1000;
//	DWORD iLastWritePosition = iLatency;
//	DWORD iFrameEnd;
//	DWORD iBytes;
//	LPBYTE pStart;
//	LPBYTE pUserBuffer = MALLOC(BYTE, 256*4);
//	sp->iError_ = sp->pBuffer_->Lock(0, SoundPlayer::iBufferSize_, (LPVOID*)&pBuffer, &unused, 0, 0, DSBLOCK_ENTIREBUFFER);
//	memset(pBuffer, 0, SoundPlayer::iBufferSize_);
//
//	while (!sp->isThreadTerminated_) {
//		printf("Terminated: %d\n", sp->isThreadTerminated_);
//		// poll play pointer
//		sp->pBuffer_->GetCurrentPosition(&iPlayPosition, 0);
//		iFrameEnd = iPlayPosition + iLatencyInSamples;
//		while (iFrameEnd > iLastWritePosition) {
//			if (iPlayPosition > iLastWritePosition) {
//				iLastWritePosition += SoundPlayer::iBufferSize_ / 4;
//			}
//			// call sound render method
//			DWORD ix = iLastWritePosition & (SoundPlayer::iBufferSize_/4 - 1);
//			(*sp->pCallBack_)(pUserBuffer, 256);
//			for (int i = 0; i < 256; i++) {
//				((short*)pBuffer)[ix] = ((short*)pUserBuffer)[i];
//				ix++;
//				ix &= (SoundPlayer::iBufferSize_ / 4 - 1);
//			}
//			iLastWritePosition += 256;
//			iLastWritePosition &= (SoundPlayer::iBufferSize_ / 4 - 1);
//		}
//		// Sleep thread
//		Sleep(2);
//	}
//	printf("STOP3");
//	sp->iError_ = sp->pBuffer_->Unlock(pBuffer, SoundPlayer::iBufferSize_, 0, 0);
//	FREE(pUserBuffer);
//	sp->isRunning_ = false;
//	//sp->isThreadTerminated_ = false;
//	return 0;
//}
//#endif
//
//
//
//HRESULT SoundPlayer::createBuffer(int iSamplingRate, int iChannels, int iBitsPerSample, int iBufferSize, LPDIRECTSOUNDBUFFER8* ppDsb8) {
//	WAVEFORMATEX wfx;
//	DSBUFFERDESC dsbdesc;
//	LPDIRECTSOUNDBUFFER pDsb = NULL;
//	HRESULT hr;
// 
//	// set up WAV format structure
//	wfx.cbSize = sizeof(WAVEFORMATEX);
//	wfx.wFormatTag = WAVE_FORMAT_PCM;
//	wfx.nSamplesPerSec = iSamplingRate;
//	wfx.nChannels = iChannels;
//	wfx.wBitsPerSample = iBitsPerSample;
//	wfx.nBlockAlign = iChannels*(iBitsPerSample>>3);
//	wfx.nAvgBytesPerSec = iSamplingRate * wfx.nBlockAlign;
// 
//	// set up DSBUFFERDESC structure
//	dsbdesc.lpwfxFormat = &wfx;	
//	dsbdesc.dwSize = sizeof(DSBUFFERDESC); 
//	dsbdesc.dwFlags = DSBCAPS_LOCSOFTWARE | DSBCAPS_GLOBALFOCUS;
//	dsbdesc.guid3DAlgorithm = GUID_NULL;
//	dsbdesc.dwBufferBytes = iBufferSize;
//	dsbdesc.dwReserved = 0;
// 
//	// create buffer
//	hr = pDevice_->CreateSoundBuffer(&dsbdesc, &pDsb, 0);
//	if (SUCCEEDED(hr)) { 
//		 hr = pDsb->QueryInterface(IID_IDirectSoundBuffer8, (LPVOID*) ppDsb8);
//		 pDsb->Release();
//	} 
//	return hr;
//}
//void SoundPlayer::reset() {
//	pBuffer_->SetCurrentPosition(0);
//	this->clear();
//}
//void SoundPlayer::start() {
//	if (!isRunning_) {
//		// create play thread
//		if (hThread_ == NULL) {
//			hThread_ = CreateThread(0, 0x1000, &SoundPlayer::threadProc, this, CREATE_SUSPENDED, &iThreadId_);
//			iError_ = -1;
//			if (hThread_ != 0) {
//				SetThreadPriority(hThread_, THREAD_PRIORITY_HIGHEST);
//				ResumeThread(hThread_);
//				iError_ = pBuffer_->Play(0, 0, DSBPLAY_LOOPING);
//				isRunning_ = true;
//			}
//		}
//	}
//}
//void SoundPlayer::restart() {
//	this->reset();
//	this->start();
//}
//int SoundPlayer::pause() {
//	return isRunning_ = !isRunning_;
//}
//void SoundPlayer::stop() {
//	DWORD res = 0;
//	if (isRunning_){
//		// stop the thread
//		isThreadTerminated_ = true;
//		printf("STOP1");
//		while (isRunning_) {
//			Sleep(1);
//		}
//		iError_ = pBuffer_->Stop();
//		printf("STOP2");
//	}
//	if (hThread_) {
//		CloseHandle(hThread_);
//	}
//	hThread_ = NULL;
//	iThreadId_ = 0;
//}
//void SoundPlayer::clear() {
//	LPVOID pSmp;
//	DWORD iByteCount;
//	iError_ = pBuffer_->Lock(0, 0, &pSmp, &iByteCount, NULL, NULL, DSBLOCK_ENTIREBUFFER);
//	if (iError_ == DS_OK) {
//		for (DWORD i = 0; i < iByteCount; i++) {
//			((BYTE*)pSmp)[i] = 0;
//		}
//		iError_ = pBuffer_->Unlock(pSmp, iByteCount, NULL, NULL);
//	}
//}

//void SoundPlayer::update(int index) {
//	LPVOID smpPtr1, smpPtr2;
//	DWORD byteCount1, byteCount2;
//	int start = 0;
//	if (index != 0)
//	{
//		start = pBufferHalf_;
//	}
//	iError_ = pBuffer_->Lock(start, pBufferHalf_, &smpPtr1, &byteCount1, &smpPtr2, &byteCount2, 0);
//	if (iError_ == DS_OK)
//	{
//		(*this->callBack)(smpPtr1, pBufferHalf_);
//		iError_ = pBuffer_->Unlock(smpPtr1, byteCount1, smpPtr2, byteCount2);
//	}
//}

