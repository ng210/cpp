#include <stdio.h>
#include "utils/File.h"
#include "base/memory.h"

NS_FW_BASE_BEGIN

UINT8* File::read(const char* path) {
	UINT8* buffer = NULL;
	FILE* fp = NULL;
	if (fopen_s(&fp, path, "rb") == 0 && fp != NULL) {
		fseek(fp, 0, SEEK_END);
		size_t length = ftell(fp);
		buffer = MALLOC(UINT8, length + 1);
		fseek(fp, 0, SEEK_SET);
		size_t byteCount = fread(buffer, sizeof(char), length, fp);
		fclose(fp);
		buffer[byteCount] = '\0';
	}
	return buffer;
}

size_t File::write(const char* path, UINT8* buffer, size_t length) {
	size_t byteCount = 0;
	FILE* fp = NULL;
	if (fopen_s(&fp, path, "wb") == 0 && fp != NULL) {
		byteCount = fwrite(buffer, sizeof(char), length, fp);
		fclose(fp);
	}
	return byteCount;
}

size_t File::write(const char* path, Buffer* buffer) {
	size_t byteCount = 0;
	FILE* fp = NULL;
	if (fopen_s(&fp, path, "wb") == 0 && fp != NULL) {
		size_t remainingBytes = buffer->length();
		for (UINT32 i = 0; i < buffer->chunks()->length(); i++) {
			BufferChunk* chunk = (BufferChunk*)buffer->chunks()->getAt(i);
			size_t length = chunk->byteCount() < remainingBytes ? chunk->byteCount() : remainingBytes;
			fwrite(chunk->buffer(), sizeof(UINT8), length, fp);
			remainingBytes -= chunk->byteCount();
		}
		fclose(fp);
	}
	return byteCount;
}

NS_FW_BASE_END