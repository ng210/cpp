#ifndef __WAVE_H
#define __WAVE_H

#ifdef __CRT
	#include "crt.h"
	#include "file.h"
#else
	#include "base/string.h"
	#include "collection/array.h"
	#include "utils/buffer.h"
#endif

NS_FW_BASE_USE

#define BUFFERCHUNK_SIZE 1024*1024	// 1MB

typedef struct SUBCHUNK1_ {
	char subchunk1ID[4] = { 'f', 'm', 't', ' ' };
	long int subchunk1Size = (long)(sizeof(SUBCHUNK1_) - offsetof(SUBCHUNK1_, audioFormat));
	short int audioFormat = 1;
	short int nChannels;		// 1,2 for stereo data is (l,r) pairs
	long int nSampleRate;
	long int nByteRate;			// = SampleRate * NumChannels * BitsPerSample/8
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
	WAVEHEADER* header_;
	char* fileName_;
	Buffer* buffer_;
private: PROP_R(size_t, length);

	static WAVEHEADER* createHeader(int sampleRate, int channelCount, int bitsPerSample);
public:
	WaveFmt(char* path, int sampleRate, int channelCount, int bitsPerSample);
	~WaveFmt();
	//size_t write(Buffer& data, size_t length, size_t offset = 0);
	size_t write(const char* data, UINT32 length, UINT32 offset = 0);
	size_t close();

	static size_t write(const char* path, int sampleRate, int channelCount, int bitsPerSample, UINT8* data, UINT32 byteCount);
};

#endif