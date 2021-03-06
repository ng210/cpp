#include <stdio.h>
#include "utils/file.h"
#include "base/memory.h"

NS_FW_BASE_BEGIN

size_t File::read(const char* path, UINT8** ptr, size_t offset, size_t byteCount) {
	size_t bytesRead = -1;
	FILE* fp = NULL;

	if (fopen_s(&fp, path, "rb") == 0 && fp != NULL) {
		// get file size
		fseek(fp, 0, SEEK_END);
		size_t length = ftell(fp);
		// byteCount = 0 reads whole file
		if (byteCount == 0) {
			byteCount = length;
		}
		// offset must not point beyond the file
		size_t remaining = length - offset;
		if (remaining > 0) {
			// seek to offset
			fseek(fp, 0, SEEK_SET);
			if (offset > 0) {
				fseek(fp, (long)offset, SEEK_CUR);
				if (byteCount > remaining) {
					byteCount = remaining;
				}
			}			
			if (*ptr == NULL) {
				*ptr = MALLOC(UINT8, byteCount);
			}
			UINT8* buffer = *ptr;
			bytesRead = fread(buffer, sizeof(char), byteCount, fp);
			fclose(fp);
		}
	}
	return bytesRead;
}

size_t File::read(const char* path, Buffer* buffer, size_t offset, size_t byteCount) {
	UINT8* ptr = NULL;
	size_t length = File::read(path, &ptr, offset, byteCount);
	buffer->append(ptr, (UINT32)length);
	FREE(ptr);
	return length;
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
			byteCount += length;
			remainingBytes -= chunk->byteCount();
		}
		fclose(fp);
	}
	return byteCount;
}

size_t File::getSize(const char* path) {
	FILE* fp = NULL;
	size_t length = -1;
	if (fopen_s(&fp, path, "rb") == 0 && fp != NULL) {
		fseek(fp, 0, SEEK_END);
		length = ftell(fp);
	}
	return length;
}

NS_FW_BASE_END