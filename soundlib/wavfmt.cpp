#include "WavFmt.h"

#include "base/memory.h"
#include "utils/file.h"

#pragma warning( disable : 4996)

NS_FW_BASE_USE

WaveFmt::WaveFmt(char* path, int sampleRate, int channelCount, int bitsPerSample) {
	fileName_ = path;
	length_ = 0;
	buffer_ = NEW_(Buffer);
	header_ = createHeader(sampleRate, channelCount, bitsPerSample);
	buffer_->write(header_, sizeof(WAVEHEADER));
}

WaveFmt::~WaveFmt() {
	DEL_(buffer_);
	DEL_(header_);
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

size_t WaveFmt::write(const char* data, UINT32 length, UINT32 offset) {
	length_ += length;
	return buffer_->append((void*)data, length, offset);
}

size_t WaveFmt::close() {
	size_t byteCount = header_->SubChunk1.nBlockAlign * length_;
	header_->chunkSize = (long)(sizeof(WAVEHEADER) - offsetof(WAVEHEADER, format) + byteCount);
	header_->SubChunk2.subchunk2Size = (long)byteCount;

	buffer_->write((char*)header_, sizeof(WAVEHEADER));
	//buffer_->append(buffer, byteCount);
	File::write(fileName_, buffer_);
	return byteCount;
}

size_t WaveFmt::write(const char* path, int sampleRate, int channelCount, int bitsPerSample, UINT8* data, UINT32 byteCount) {
	WAVEHEADER* header = WaveFmt::createHeader(sampleRate, channelCount, bitsPerSample);
	header->chunkSize = (long)(sizeof(WAVEHEADER) - offsetof(WAVEHEADER, format) + byteCount);
	header->SubChunk2.subchunk2Size = (long)byteCount;
	size_t length = sizeof(WAVEHEADER) + byteCount;

	UINT8* buffer = MALLOC(UINT8, length);
	size_t i = 0;
	for (; i < sizeof(WAVEHEADER); i++) {
		buffer[i] = ((char*)header)[i];
	}
	for (size_t j = 0; j < byteCount; j++) {
		buffer[i + j] = data[j];
	}
	File::write(path, buffer, length);
	FREE(buffer);
	FREE(header);
	return length;
}
