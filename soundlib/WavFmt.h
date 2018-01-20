#ifndef __WAVE_H
#define __WAVE_H

#ifdef __CRT
	#include "crt.h"
	#include "File.h"
#else
	#include "base/String.h"
	#include "collection/Array.h"
	#include "utils/Buffer.h"
	//#include <stdio.h>
	//#include <string.h>
#endif

NS_FW_BASE_USE

#define BUFFERCHUNK_SIZE 1024*1024	// 1MB

typedef struct SUBCHUNK1_ {
	char subchunk1ID[4] = { 'f', 'm', 't', ' ' };
	long int subchunk1Size = (long)(sizeof(SUBCHUNK1_) - offsetof(SUBCHUNK1_, audioFormat));
	short int audioFormat = 1;
	short int nChannels;		// 1,2 for stereo data is (l,r) pairs
	long int nSampleRate;
	long int nByteRate;	// = SampleRate * NumChannels * BitsPerSample/8
	short int nBlockAlign;		// = NumChannels * BitsPerSample/8
	short int nBitsPerSample;
} SUBCHUNK1;

typedef struct SUBCHUNK2_ {
	char subchunk2ID[4] = { 'd', 'a', 't', 'a' };
	long int subchunk2Size;		// NumSamples * NumChannels * BitsPerSample/8
} SUBCHUNK2;

typedef struct WAVEHEADER_ {
	char chunkID[4] = {'R', 'I', 'F', 'F'};
	long int chunkSize;
	char format[4] = { 'W', 'A', 'V', 'E' };
	SUBCHUNK1 SubChunk1;
	SUBCHUNK2 SubChunk2;
} WAVEHEADER;

class WaveFmt {
	WAVEHEADER header_;
	String fileName_;
	Buffer* buffer_;
private: PROP_R(size_t, length);

	static WAVEHEADER* createHeader(int sampleRate, int channelCount, int bitsPerSample);
public:
	WaveFmt(String& path, int sampleRate, int channelCount, int bitsPerSample);
	~WaveFmt();
	size_t write(Buffer& data, size_t length, size_t offset = 0);
	size_t write(const char* data, size_t length, size_t offset = 0);
	size_t close();

	//static size_t write(const char* path, int sampleRate, int channelCount, int bitsPerSample, Buffer& data);
	static size_t write(const char* path, int sampleRate, int channelCount, int bitsPerSample, const char* data, size_t byteCount);
	//static size_t write(String& path, int sampleRate, int channelCount, int bitsPerSample, Buffer& data);
	//static size_t write(String& path, int sampleRate, int channelCount, int bitsPerSample, const char* data, size_t length);
};

#endif