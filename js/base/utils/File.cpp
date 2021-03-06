#include <stdio.h>
#include "utils/File.h"
#include "base/MemoryMgr.h"
#include "base/String.h"

NS_FW_BASE_BEGIN

char* File::read(const char* path) {
	char* buffer = NULL;
	FILE* fp = fopen(path, "rb");
	if (fp != NULL) {
		fseek(fp, 0, SEEK_END);
		size_t length = ftell(fp);
		buffer = MALLOC(char, length + 1);
		fseek(fp, 0, SEEK_SET);
		size_t byteCount = fread(buffer, sizeof(char), length, fp);
		fclose(fp);
		buffer[byteCount] = '\0';
	}
	return buffer;
}
char* File::read(String& path) {
	char* buf = path.toString();
	char* res = File::read(buf);
	DEL_(buf);
	return res;
}

size_t File::write(const char* path, char* buffer, size_t length) {
	size_t byteCount = 0;
	FILE* fp = fopen(path, "wb");
	if (fp != NULL) {
		byteCount = fwrite(buffer, sizeof(char), length, fp);
		fclose(fp);
	}
	return byteCount;
}
size_t File::write(String& path, char* buffer, size_t length) {
	char* buf = path.toString();
	size_t byteCount = File::write(buf, buffer, length);
	FREE(buf);
	return byteCount;
}

size_t File::write(const char* path, Buffer& buffer) {
	size_t byteCount = 0;
	FILE* fp = fopen(path, "wb");
	if (fp != NULL) {
		for (size_t i = 0; i < buffer.chunks_.length(); i++) {
			BufferChunk& chunk = *(BufferChunk*)buffer.chunks_[i];
			fwrite(chunk.buffer(), sizeof(char), chunk.byteCount(), fp);
		}
		fclose(fp);
	}
	return byteCount;
}
size_t File::write(String& path, Buffer& buffer) {
	char* str = path.toString();
	size_t byteCount = write(str, buffer);
	FREE(str);
	return byteCount;
}

NS_FW_BASE_END