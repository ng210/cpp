#ifndef __PATHINFO_H
#define __PATHINFO_H

#include <string.h>

class PathInfo {
	char *buffer;
	char *path;
	char *fileName;
	char *extension;
public:
	PathInfo(const char *fileName) {
		this->buffer = _strdup(fileName);
		this->path = "";
		this->fileName = "";
		this->extension = "";
		size_t len = strlen(fileName);
		int step = 0;
		while (--len > 0) {
			if (step == 0 && this->buffer[len] == '.') {
				this->buffer[len] = '\0';
				this->extension = &this->buffer[len + 1];
				step++;
				continue;
			}
			if (step == 1 && this->buffer[len] == '\\') {
				this->buffer[len] = '\0';
				this->fileName = &this->buffer[len + 1];
				this->path = this->buffer;
				step++;
				break;
			}
		}
		if (step == 1) {
			this->fileName = this->buffer;
		}
	}
	~PathInfo() {
		delete[] this->buffer;
	}
	char* getPath() { return this->path; }
	char* getFileName() { return this->fileName; }
	char* getExtension() { return this->extension; }
};

#endif