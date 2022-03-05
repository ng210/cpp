#include "source.h"
#include "base/string.h"
#include "base/memory.h"
#include "utils/file.h"

NS_FW_BASE_USE

Source::Source(void) {
	init(NULL);
}
Source::Source(const char* name) {
	init(name);
}
Source::~Source(void) {
	//for (UINT32 i = 0; i < includes_.length(); i++) {
	//	Source* source = (Source*)includes_.getAt(i);
	//	if (--source->flag_ == 0) {
	//		DEL_(source);
	//	}
	//}
	if (content_ != NULL) {
		ARRAY_FOREACH(content_, FREE(value));
		DEL_(content_);
	}
	DEL_(name_);
}
void Source::init(const char* name) {
	name_ = strdup(name);
	flag_ = 0;
	BYTE* buf = NULL;
	File::read(name_, &buf);
	content_ = NULL;
	if (buf != NULL) {
		content_ = PArray::str_split((const char*)buf, "\n");
	}
	FREE(buf);
}
//void Source::addInclude(Source *inc) {
//	includes_.add(inc);
//}
//size_t Source::hasIncludes() {
//	return includes_.length();
//}
//Source* Source::getInclude(UINT32 ix) {
//	return (Source*)includes_.getAt(ix);
//}