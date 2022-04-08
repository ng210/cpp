#include "wavfmt.h"

#include "base/memory.h"

#pragma warning( disable : 4996)

NS_FW_BASE_USE

WaveFmt::WaveFmt(char* path, int sampleRate, int channelCount, int bitsPerSample) {
	fileName_ = path;
	fopen_s(&fp_, path, "wb");
	sizeInBytes_ = 0;
	header_ = createHeader(sampleRate, channelCount, bitsPerSample);
}

WaveFmt::~WaveFmt() {
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

size_t WaveFmt::write(byte* buffer, int byteCount) {
	if (sizeInBytes_ == 0) {
		fwrite(header_, sizeof(WAVEHEADER), 1, fp_);
	}
	sizeInBytes_ += byteCount;
	return fwrite(buffer, sizeof(byte), byteCount, fp_);
}

size_t WaveFmt::close() {
	header_->chunkSize = (long)(sizeof(WAVEHEADER) - offsetof(WAVEHEADER, format) + sizeInBytes_);
	header_->SubChunk2.subchunk2Size = (long)sizeInBytes_;
	fseek(fp_, 0, SEEK_SET);
	fwrite(header_, sizeof(WAVEHEADER), 1, fp_);
	fclose(fp_);
	return sizeInBytes_;
}

size_t WaveFmt::write(const char* path, int sampleRate, int channelCount, int bitsPerSample, UINT8* data, int byteCount) {
	var fp = fopen(path, "wb");
	WAVEHEADER* header = WaveFmt::createHeader(sampleRate, channelCount, bitsPerSample);
	header->chunkSize = (long)(sizeof(WAVEHEADER) - offsetof(WAVEHEADER, format) + byteCount);
	header->SubChunk2.subchunk2Size = (long)byteCount;

	var length = fwrite(header, sizeof(WAVEHEADER), 1, fp);
	length += fwrite(data, sizeof(byte), byteCount, fp);
	fclose(fp);
	FREE(header);
	return length;
}
