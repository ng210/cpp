#include <stdio.h>
#include "utils/file.h"
#include "base/memory.h"

NS_FW_BASE_BEGIN

bool File::exists(const char* path) {
	FILE* fp = NULL;
	return fopen_s(&fp, path, "r") == 0 && fp != NULL;
}

size_t File::read(const char* path, UINT8** ptr, size_t offset, size_t byteCount) {
	size_t bytesRead = -1;
	FILE* fp = NULL;

	if (fopen_s(&fp, path, "rb") == 0 && fp != NULL) {
		// get file size
		fseek(fp, 0, SEEK_END);
		size_t length = (size_t)ftell(fp) + 1;	// add 1 byte to ensure EOF
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
			buffer[length-1] = '\0';
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

size_t File::write(FILE* fp, UINT8* buffer, size_t byteCount) {
	return fwrite(buffer, sizeof(UINT8), byteCount, fp);
}

size_t File::write(FILE* fp, Buffer* buffer) {
	size_t byteCount = 0;
	size_t remainingBytes = buffer->length();
	for (int i = 0; i < buffer->chunks()->length(); i++) {
		BufferChunk* chunk = (BufferChunk*)buffer->chunks()->get(i);
		size_t length = chunk->byteCount() < remainingBytes ? chunk->byteCount() : remainingBytes;
		byteCount += write(fp, chunk->buffer(), length);
		remainingBytes -= chunk->byteCount();
	}
	return byteCount;
}

size_t File::write(const char* path, UINT8* buffer, size_t length, bool update) {
	size_t byteCount = 0;
	FILE* fp = NULL;
	char* mode = !update ? "wb" : "r+b";
	if (fopen_s(&fp, path, mode) == 0 && fp != NULL) {
		byteCount = write(fp, buffer, length);
		fclose(fp);
	}
	return byteCount;
}

size_t File::write(const char* path, Buffer* buffer, bool update) {
	size_t byteCount = 0;
	FILE* fp = NULL;
	char* mode = !update ? "wb" : "r+b";
	if (fopen_s(&fp, path, mode) == 0 && fp != NULL) {
		byteCount = write(fp, buffer);
		fclose(fp);
	}
	return byteCount;
}

size_t File::append(const char* path, UINT8* buffer, size_t length) {
	size_t byteCount = 0;
	FILE* fp = NULL;
	if (fopen_s(&fp, path, "ab") == 0 && fp != NULL) {
		byteCount = fwrite(buffer, sizeof(UINT8), length, fp);
		fclose(fp);
	}
	return byteCount;
}

size_t File::append(const char* path, Buffer* buffer) {
	size_t byteCount = 0;
	FILE* fp = NULL;
	if (fopen_s(&fp, path, "ab") == 0 && fp != NULL) {
		byteCount = write(fp, buffer);
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

FILE* File::open(const char* path, const char* mode) {
	FILE* fp = NULL;
	if (mode == NULL) mode = "rb";
	fopen_s(&fp, path, mode);
	return fp;
}

int File::seek(FILE* fp, long offset, int origin) {
	return fseek(fp, offset, origin);
}

NS_FW_BASE_END