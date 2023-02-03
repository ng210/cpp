#include "base/memory.h"
#include "adapter.h"

using namespace PLAYER;

Adapter::Adapter() {
}
//Adapter::~Adapter() {}

void Adapter::initialize(byte** pData) {}

int Adapter::compare(void* item, Key key, UINT32 ix, Collection* collection, void* args) {
	return (*(Adapter**)item)->getInfo()->id - (*(Adapter**)key.p)->getInfo()->id;
}
#ifdef PLAYER_EDIT_MODE
void Adapter::writeToStream(Stream* data) {
	data->writeByte(getInfo()->id);
}
#endif