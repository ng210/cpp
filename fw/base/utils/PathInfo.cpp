#include "utils/PathInfo.h"
#include "base/str.h"

NS_FW_BASE_BEGIN

PathInfo::PathInfo(const char* fileName) {
	buffer_ = strdup(fileName);
	path_ = "";
	fileName = "";
	extension_ = "";
	size_t len = strlen(fileName);
	int step = 0;
	while (--len > 0) {
		if (step == 0 && buffer_[len] == '.') {
			buffer_[len] = '\0';
			extension_ = &buffer_[len + 1];
			step++;
			continue;
		}
		if (step == 1 && buffer_[len] == '\\') {
			buffer_[len] = '\0';
			fileName = &buffer_[len + 1];
			path_ = buffer_;
			step++;
			break;
		}
	}
	if (step == 1) {
		fileName = buffer_;
	}
}
PathInfo::~PathInfo() {
	delete[] buffer_;
}

char* PathInfo::getPath() { return path_; }
char* PathInfo::getFileName() { return fileName_; }
char* PathInfo::getExtension() { return extension_; }

NS_FW_BASE_END
