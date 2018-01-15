#include "WavFmt.h"

#include "base/MemoryMgr.h"
#include "utils/File.h"

#pragma warning( disable : 4996)

NS_FW_BASE_USE

WaveFmt::WaveFmt(String& path, int sampleRate, int channelCount, int bitsPerSample) {
	fileName_ = path;
	length_ = 0;
	buffer_ = NEW_(Buffer, TYPE_SHORT);
	createHeader(sampleRate, channelCount, bitsPerSample);
}

WaveFmt::~WaveFmt() {
	DEL_(buffer_)
}

WAVEHEADER* WaveFmt::createHeader(int sampleRate, int channelCount, int bitsPerSample) {
	WAVEHEADER* header = NEW_(WAVEHEADER);
	header->chunkSize = sizeof header - 8;

	int blockAlign = channelCount * bitsPerSample >> 3;
	int nAvgBytesPerSec = sampleRate * blockAlign;

	header->SubChunk1.audioFormat = 1;
	header->SubChunk1.nChannels = channelCount;
	header->SubChunk1.nSampleRate = sampleRate;
	header->SubChunk1.nByteRate = sampleRate * blockAlign;
	header->SubChunk1.nBitsPerSample = bitsPerSample;
	header->SubChunk1.nBlockAlign = blockAlign;
	return header;
}
size_t WaveFmt::write(const char* data, size_t length, size_t offset) {
	length_ += length;
	return buffer_->append((void*)data, length, offset);
}
size_t WaveFmt::write(Buffer& data, size_t length, size_t offset) {
	length_ += length;
	return buffer_->append(data, length, offset);
	//write(data.getBuffer(), offset, length);
}

//size_t WaveFmt::write(String& path, int sampleRate, int channelCount, int bitsPerSample, const char* data, size_t length) {
//	WaveFmt waveFmt(path, sampleRate, channelCount, bitsPerSample);
//	waveFmt.write(data, 0, length);
//	return waveFmt.close();
//}
//size_t WaveFmt::write(String& path, int sampleRate, int channelCount, int bitsPerSample, Buffer& data, size_t length) {
//	WaveFmt waveFmt(path, sampleRate, channelCount, bitsPerSample);
//	waveFmt.write(data, 0, length);
//	return waveFmt.close();
//}
//
size_t WaveFmt::write(const char* path, int sampleRate, int channelCount, int bitsPerSample, const char* data, size_t byteCount) {
	WAVEHEADER* header = WaveFmt::createHeader(sampleRate, channelCount, bitsPerSample);
	header->chunkSize = (long)(sizeof(WAVEHEADER) - offsetof(WAVEHEADER, format) + byteCount);
	header->SubChunk2.subchunk2Size = (long)byteCount;
	size_t length = sizeof(WAVEHEADER) + byteCount;

	char* buffer = MALLOC(char, length);
	size_t i = 0, j = 0;
	for (; i < sizeof(WAVEHEADER); i++) {
		buffer[i] = ((char*)header)[i];
	}
	for (; i < byteCount; i++) {
		buffer[i] = data[j++];
	}
	File::write(path, buffer, length);
	FREE(buffer);
	FREE(header);
	return length;
}
//size_t WaveFmt::write(String& path, int sampleRate, int channelCount, int bitsPerSample, Buffer& data) {
//	return write(path, sampleRate, channelCount, bitsPerSample, data.getBuffer(), data.length());
//}
//size_t WaveFmt::write(const char* path, int sampleRate, int channelCount, int bitsPerSample, Buffer& data) {
//	return write(path, sampleRate, channelCount, bitsPerSample, data, data.length());
//}

size_t WaveFmt::close() {
	size_t byteCount = header_.SubChunk1.nBlockAlign * length_;
	header_.chunkSize = (long)(sizeof(WAVEHEADER) - offsetof(WAVEHEADER, format) + byteCount);
	header_.SubChunk2.subchunk2Size = (long)byteCount;
	buffer_->write((char*)&header_, 0, sizeof(WAVEHEADER));
	char* buffer = buffer_->getBuffer<char>();
	File::write(fileName_, buffer, byteCount + sizeof(WAVEHEADER));
	FREE(buffer);
	return byteCount;	// File::write(fileName_, *buffer_);
}

//int WaveFmt::write(float* buffer, int length)
//{
//	int ret = 0;
//	if (this->fp != 0)
//	{
//		short int *ptr = (short int*)buffer;
//		for (int i=0;i<length;i++)
//		{
//			int s;
//			if (buffer[i] < -1.0f)
//				s = -32767;
//			else
//			if (buffer[i] > 1.0f)
//				s = 32767;
//			else
//				s = (short int)(32767*buffer[i]);
//			ptr[i] = s;
//		}
//#ifdef __CRT
//		ret = fp->write(ptr, this->nBlockAlign*length);
//#else
//		ret = (int)fwrite(ptr, this->nBlockAlign, length,fp);
//#endif
//	}
//	this->nSamples += length;
//	return ret;
//}
