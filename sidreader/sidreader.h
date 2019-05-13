#include "consoleapp.h"

NS_FW_BASE_USE

enum EXTRACT_FORMAT {
	EXTRACT_TEXT = 0,
	EXTRACT_JSON = 1
};


class SidReader {
	static const char* usageText;
	static const char* genericErrorText;

	EXTRACT_FORMAT format_;
	char* path_;
	bool verbose_;
	Buffer output_;
	int count_;

	const char* error;

	static ENUMDIRENTRYPROC processFileCallback;

	void processArguments(NS_FW_BASE::Map& args);
	void processFile(const char* fullPath);

public:
	SidReader(NS_FW_BASE::Map& args);
	~SidReader();

	const char* getError();
};