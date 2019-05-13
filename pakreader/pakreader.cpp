#include "pakreader.h"

NS_FW_BASE_USE

int _main(NS_FW_BASE::Map* args)
{
	printf("PAK reader\n");

	//MemoryMgr::isDebugOn = true;

	if (args->size() > 1) {
		char* path = (char*)args->keys()->getAt(1);
		int id = -1;
		if (args->size() > 2) {
			char* arg2 = (char*)args->keys()->getAt(2);
			if (NS_FW_BASE::strncmp(arg2, "*") == 0) {
				id = -2;
			} else {
				id = atoi((char*)args->keys()->getAt(2));
			}
		}

		PakReader reader(path);
		Array& contents = reader.getContents();
		if (id == -1) {
			for (int i = 0; i < (int)contents.length();) {
				PAKENTRY* entry = (PAKENTRY*)contents.getAt(i);
				i++;
				printf("% 2d. %s: %04X\n", i, entry->name, entry->offset);
			}
		} else {
			if (id != -2) {
				int count = reader.extract(id);
				if (count > 0) {
					printf("%d bytes written.\n", count);
				} else {
					printf("No data.\n");
				}
			} else {
				for (int i = 0; i < (int)contents.length(); i++) {
					PAKENTRY* entry = (PAKENTRY*)contents.getAt(i);
					int count = reader.extract(i);
					printf("% 2d. %s: ", i, entry->name);
					if (count > 0) {
						printf("%d bytes written.\n", count);
					} else {
						printf("write error.\n");
					}
				}
			}
		}
	} else {
		printf("Usage: pakreader path [id]\n");
	}
	return 0;
}

// ****************************************************************************


PakReader::PakReader(const char* path) : contents(sizeof(PAKENTRY), 64) {
	this->data = NULL;
	size_t size = File::read(path, &this->data);
	// fill contents array
	UINT8* ptr = this->data;
	PAKENTRY entry;
	if (ptr != NULL) {
		while ((*(UINT32*)ptr != NULL)) {
			entry.offset = *(UINT32*)ptr; ptr += 4;
			entry.name = (char*)ptr;
			while (*ptr++ != '\0');
			this->contents.add(&entry);
		}
		entry.offset = (UINT32)size;
		entry.name = NULL;
		this->contents.add(&entry);
	}
}

PakReader::~PakReader() {
	FREE(this->data);
	this->contents.clear();
}

int PakReader::extract(int i) {
	int count = 0;
	if (i >= 0 && i < (int)this->contents.length()) {
		PAKENTRY* entry1 = (PAKENTRY*)this->contents.getAt(i);
		PAKENTRY* entry2 = (PAKENTRY*)this->contents.getAt(i+1);
		count = entry2->offset - entry1->offset;
		File::write(entry1->name, this->data + entry1->offset, count);
	};
	return count;
}