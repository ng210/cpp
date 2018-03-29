#include "utils/PathInfo.h"
#include "base/memory.h"
#include "collection/Array.h"
#include "base/str.h"

NS_FW_BASE_BEGIN

PathInfo::PathInfo(char* fileName) {
	init(fileName);
}
PathInfo::PathInfo(const char* fileName) {
	init(fileName);
}

void PathInfo::init(const char* fileName) {
	buffer_ = strdup(fileName);
	long long len = strlen(fileName);
	extension_ = NULL;
	fileName_ = NULL;
	path_ = NULL;

	while (--len >= 0) {
		if (extension_ == NULL && buffer_[len] == '.') {
			buffer_[len] = '\0';
			extension_ = &buffer_[len + 1];
			continue;
		}
		if (buffer_[len] == '\\' || buffer_[len] == '/') {
			if (extension_ == NULL) {
				extension_ = "";
			}
			fileName_ = &buffer_[len + 1];
			if (buffer_[len-1] == '\\' || buffer_[len-1] == '/') {
				len--;
			}
			buffer_[len] = '\0';
			size_t length = len;
			while (--len >= 0) {
				if (buffer_[len] == '/') {
					buffer_[len] = '\\';
				}
				if (buffer_[len] == '\\' && buffer_[len + 1] == '\\') {
					strncpy(&buffer_[len], length, &buffer_[len + 1]);
				}
			}
			path_ = buffer_;
			break;
		}
	}
	if (fileName_ == NULL) {
		fileName_ = buffer_;
		path_ = "";
		if (extension_ == NULL) {
			extension_ = "";
		}
	}
}
PathInfo::~PathInfo() {
	FREE(buffer_);
}

NS_FW_BASE_END
