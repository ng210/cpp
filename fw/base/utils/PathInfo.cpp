#include "utils/PathInfo.h"
#include "base/MemoryMgr.h"
#include "collection/Array.h"
#include "base/str.h"

NS_FW_BASE_BEGIN

PathInfo::PathInfo(String* fileName) {
	char* buffer = fileName->toString();
	init(buffer);
	FREE(buffer);
}
PathInfo::PathInfo(const char* fileName) {
	init(fileName);
}

void PathInfo::init(const char* fileName) {
	char* buffer = strdup(fileName);
	size_t len = strlen(fileName);
	extension_ = NULL;
	fileName_ = NULL;
	path_ = NULL;

	while (--len > 0) {
		if (extension_ == NULL && buffer[len] == '.') {
			buffer[len] = '\0';
			extension_ = NEW_(String, (const char*)&buffer[len + 1]);
			continue;
		}
		if (buffer[len] == '\\' || buffer[len] == '/') {
			if (extension_ == NULL) {
				extension_ = NEW_(String, "");
			}
			buffer[len] = '\0';
			fileName_ = NEW_(String, (const char*)&buffer[len + 1]);
			path_ = NEW_(String, (const char*)buffer);
			break;
		}
	}
	if (fileName_ == NULL) {
		fileName_ = NEW_(String, (const char*)buffer);
		path_ = NEW_(String, "");
		if (extension_ == NULL) {
			extension_ = NEW_(String, "");
		}
	}
	FREE(buffer);
}
PathInfo::~PathInfo() {
	DEL_(path_);
	DEL_(fileName_);
	DEL_(extension_);
}

String* PathInfo::getPath() { return path_; }
String* PathInfo::getFileName() { return fileName_; }
String* PathInfo::getExtension() { return extension_; }

NS_FW_BASE_END
