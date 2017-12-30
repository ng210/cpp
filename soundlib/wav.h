#ifndef __WAVE_H
#define __WAVE_H

#ifdef __CRT
	#include "crt.h"
	#include "File.h"
#else
	#include <stdio.h>
	#include <string.h>
#endif

struct SUBCHUNK1 {
	char subchunk1ID[4];		// 'fmt '
	long int subchunk1Size;		// 16 for PCM
	short int audioFormat;		// PCM = 1
	short int nChannels;		// 1,2 for stereo data is (l,r) pairs
	long int nSamplesPerSec;
	long int nAvgBytesPerSec;	// = SampleRate * NumChannels * BitsPerSample/8
	short int nBlockAlign;		// = NumChannels * BitsPerSample/8
	short int nBitsPerSample;
};

struct SUBCHUNK2 {
	char subchunk1ID[4];		// 'data'
	long int subchunk1Size;		// NumSamples * NumChannels * BitsPerSample/8
};

struct WAVEHEADER {
	char chunkID[4];			// 'RIFF'
	long int chunkSize;
	char format[4];				// 'WAVE'
	SUBCHUNK1 SubChunk1;
	SUBCHUNK2 SubChunk2;

};

class WaveFmt {
	int format;
	int nChannels;
	int nBlockAlign;
	int sampleRate;
	int bitsPerSample;
	int nSamples;
public:
	FILE *fp;
	char fName[256];
	WaveFmt(char* fname, int format, int nChannels, int sampleRate, int bitsPerSample);
	void create(char* fname, int format, int nChannels, int sampleRate, int bitsPerSample);
	void update(FILE* fp, int numSamples);
	int write(float* buffer, int length);
	int write(short* buffer, int length);
	void close();
};

#endif