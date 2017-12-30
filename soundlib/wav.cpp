#include "wav.h"

#pragma warning( disable : 4996)

WaveFmt::WaveFmt(char* fileName, int format, int nChannels, int sampleRate, int bitsPerSample)
{
	create(fileName, format, nChannels, sampleRate, bitsPerSample);
}

void WaveFmt::create(char* fileName, int format, int nChannels, int sampleRate, int bitsPerSample)
{
	//int subChunk1Size = (format == 1) ? 16 : 16;
	//int nAvgBytesPerSec = sampleRate * nChannels * bitsPerSample/8;
	strncpy(this->fName, fileName, 255);
	//strcpy_s(this->fName, 256, fileName);
	this->format = format;
	this->nBlockAlign = nChannels * bitsPerSample/8;
	this->nChannels = nChannels;
	this->sampleRate = sampleRate;
	this->bitsPerSample = bitsPerSample;
	this->nSamples = 0;
	//SUBCHUNK1 subchunk1 = { {'f','m','t',' '}, subChunk1Size, (short int)format, (short int)nChannels, sampleRate, nAvgBytesPerSec, (short int)this->nBlockAlign, (short int)bitsPerSample };
	//SUBCHUNK2 subchunk2 = { {'d','a','t','a'}, -1 };
	//WAVEHEADER header = {{'R','I','F','F'}, -1, {'W','A','V','E'}, subchunk1, subchunk2 };

#ifdef __CRT
	if ((this->fp = File::open(fileName)) != NULL)
	{
		WAVEHEADER dumy;
		fp->write(&dumy, sizeof(WAVEHEADER));
	}
#else
	if (fopen_s(&this->fp, fileName, "wb") == 0)
	{
		WAVEHEADER dumy;
		fwrite(&dumy, 1, sizeof(WAVEHEADER), fp);
	}
#endif
}

int WaveFmt::write(float* buffer, int length)
{
	int ret = 0;
	if (this->fp != 0)
	{
		short int *ptr = (short int*)buffer;
		for (int i=0;i<length;i++)
		{
			int s;
			if (buffer[i] < -1.0f)
				s = -32767;
			else
			if (buffer[i] > 1.0f)
				s = 32767;
			else
				s = (short int)(32767*buffer[i]);
			ptr[i] = s;
		}
#ifdef __CRT
		ret = fp->write(ptr, this->nBlockAlign*length);
#else
		ret = (int)fwrite(ptr, this->nBlockAlign, length,fp);
#endif
	}
	this->nSamples += length;
	return ret;
}

int WaveFmt::write(short* buffer, int length)
{
	int ret = 0;
	if (this->fp != 0)
	{
		//short int *ptr = buffer;
		//for (int i=0;i<length;i++)
		//{
		//	ptr[i] = buffer[i];
		//}
#ifdef __CRT
		ret = fp->write(ptr, this->nBlockAlign*length);
#else
		ret = (int)fwrite(buffer, this->nBlockAlign, length,fp);
#endif
	}
	this->nSamples += length;
	return ret;
}

void WaveFmt::close()
{
	if (this->fp != 0)
	{
		int subChunk1Size = (format == 1) ? 16 : 16;
		int subChunk2Size = this->nSamples * this->nBlockAlign;
		int nAvgBytesPerSec = this->sampleRate * this->nBlockAlign;
		int chunkSize = 4 + (8 + subChunk1Size) + (8 + subChunk2Size);
		SUBCHUNK1 subchunk1 = { {'f','m','t',' '}, subChunk1Size, (short int)format, (short int)this->nChannels, this->sampleRate, nAvgBytesPerSec, (short int)this->nBlockAlign, (short int)this->bitsPerSample };
		SUBCHUNK2 subchunk2 = { {'d','a','t','a'}, subChunk2Size };
		WAVEHEADER header = {{'R','I','F','F'}, chunkSize, {'W','A','V','E'}, subchunk1, subchunk2 };
		//int num = fseek(this->fp,0,0);
#ifdef __CRT
		int num = fp->seek(SEEK_SET, 0);
		num = fp->write(&header, sizeof(WAVEHEADER));
		fp->close();
#else
		int num = fseek(fp, SEEK_SET, 0);
		num = (int)fwrite(&header, sizeof(WAVEHEADER), 1, fp);
		fclose(fp);
#endif

	}
}