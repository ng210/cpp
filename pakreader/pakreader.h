#include "consoleapp.h"

NS_FW_BASE_USE

typedef struct _PAKENTRY {
	__declspec (align(1)) UINT32 offset;
	__declspec (align(1)) const char* name;
} PAKENTRY;


class PakReader {
	UINT8* data;
	Array contents;
public:
	PakReader(const char* path);
	~PakReader();

	Array& getContents() { return this->contents; };
	int extract(int i);
};