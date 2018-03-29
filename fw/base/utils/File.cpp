#include <stdio.h>
#include "utils/File.h"
#include "base/memory.h"

NS_FW_BASE_BEGIN

BYTE* File::read(const char* path) {
	BYTE* buffer = NULL;
	FILE* fp = NULL;
	if (fopen_s(&fp, path, "rb") == 0 && fp != NULL) {
		fseek(fp, 0, SEEK_END);
		size_t length = ftell(fp);
		buffer = MALLOC(BYTE, length + 1);
		fseek(fp, 0, SEEK_SET);
		size_t byteCount = fread(buffer, sizeof(char), length, fp);
		fclose(fp);
		buffer[byteCount] = '\0';
	}
	return buffer;
}

size_t File::write(const char* path, BYTE* buffer, size_t length) {
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
		ARRAY_FOREACH(buffer->chunks(), fwrite(((BufferChunk*)value)->buffer(), sizeof(BYTE), ((BufferChunk*)value)->byteCount(), fp););
		fclose(fp);
	}
	return byteCount;
}

NS_FW_BASE_END