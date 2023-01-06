#include "base/memory.h"
#include "adapter.h"

using namespace PLAYER;

Adapter::Adapter() {
}
//Adapter::~Adapter() {}

void Adapter::initialize(byte** pData) {}

int Adapter::compare(void* item, UINT32 ix, Collection* collection, void* key) {
	return (*(Adapter**)item)->getInfo()->id - (*(Adapter**)key)->getInfo()->id;
}
#ifdef PLAYER_EDIT_MODE
void Adapter::writeToStream(Stream* data) {
	data->writeByte(getInfo()->id);
}
#endif