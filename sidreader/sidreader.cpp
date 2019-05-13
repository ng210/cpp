#include "sidreader.h"
#include "../misc/psid.h"

NS_FW_BASE_USE

int _main(NS_FW_BASE::Map* args) {
	printf("SID reader\n");

	//MemoryMgr::isDebugOn = true;

	SidReader reader(*args);
	const char* error = reader.getError();
	if (error != NULL) {
		printf("*** %s\n", error);
	}

	return 0;
}



// ****************************************************************************

const char* SidReader::usageText =
"Small tool to extract information from sid files\n"
"Usage\n"
" sidreader - write this help\n"
" sidreader i=<path> - process file(s) at path\n"
" sidreader json <path> - extract file(s) at path into json format\n\n";

const char* SidReader::genericErrorText = "An error occured.\n";

const char* JsonOpenTag = "[";
const char* JsonCloseTag = "]";

size_t createJsonEntry(PSidHeader* header, Buffer& buffer) {
	int year = atoi(header->str3);
	char str[4096];
	char* title = str_replace(header->str1, "\"", "\'");
	char* author = str_replace(header->str2, "\"", "\'");
	char* copyright = str_replace(header->str3, "\"", "\'");
	int length = sprintf_s(str, "{\"author\":\"%s\",\"title\":\"%s\",\"copyright\":\"%s\",\"year\":%d}", author, title, copyright, year);
	buffer.append(str, length);
	FREE(author);
	FREE(title);
	FREE(copyright);
	return length;
}
size_t createTextEntry(PSidHeader* header, Buffer& buffer) {
	int year = atoi(header->str3);
	char str[4096];
	int length = sprintf_s(str, "################\nauthor:%s\ntitle:%s\ncopyright:%s\nyear:%d\n", header->str1, header->str2, header->str3, year);
	buffer.append(str, length);
	return length;
}

SidReader::SidReader(NS_FW_BASE::Map& args) {
	format_ = EXTRACT_TEXT;
	count_ = 0;
	processArguments(args);
	if (error == NULL) {
		switch (format_) {
			case EXTRACT_TEXT:
				break;
			case EXTRACT_JSON:
				output_.append((char*)"[", 1);
				break;
		}
		Directory::enumEntries(path_, SidReader::processFileCallback, true, this, ".sid");

		const char* ext = NULL;
		switch (format_) {
			case EXTRACT_TEXT:
				ext = "txt";
				break;
			case EXTRACT_JSON:
				output_.append((char*)"]", 1);
				ext = "json";
				break;
		}
		PathInfo pathInfo(path_);
		char* path = str_format("%s.%s", pathInfo.fileName(), ext);
		size_t length = File::write(path, &output_);
		if (verbose_) {
			printf("%zu bytes written into %s.\n", length, path);
		}
		FREE(path);
	}
}

SidReader::~SidReader() {
}

void SidReader::processArguments(NS_FW_BASE::Map& args) {
	if (args.containsKey((char*)"json")) {
		format_ = EXTRACT_JSON;
	}
	path_ = (char*)args.get((char*)"i");
	if (strnullorempty(path_)) {
		error = SidReader::usageText;
	}
	verbose_ = args.containsKey((char*)"v");
}

const char* SidReader::getError() {
	return error;
}

void SidReader::processFile(const char* path) {
	// read header
	PSidHeader* header = NULL;
	File::read(path, (UINT8**)&header, 0, PSIDHEADERSIZE);
	// extract info
	switch (format_) {
		case EXTRACT_TEXT:
			createTextEntry(header, output_);
			break;
		case EXTRACT_JSON:
			if (count_ > 0) {
				output_.append((char*)",", 1);
			}
			createJsonEntry(header, output_);
			break;
	}
	FREE(header);
	count_++;
}

//	int count = 0;
//	if (i >= 0 && i < (int)contents.length()) {
//		PAKENTRY* entry1 = (PAKENTRY*)contents.getAt(i);
//		PAKENTRY* entry2 = (PAKENTRY*)contents.getAt(i + 1);
//		count = entry2->offset - entry1->offset;
//		File::write(entry1->name, data + entry1->offset, count);
//	};
//	return count;
//}

int SidReader::processFileCallback(const char* fileName, const char* path, void* args) {
	SidReader* reader = (SidReader*)args;
	if (reader->verbose_) {
		printf(" - %s\n", fileName);
	}
	char* fullPath = str_format("%s\\%s", path, fileName);
	reader->processFile(fullPath);
	FREE(fullPath);
	return 1;
}
