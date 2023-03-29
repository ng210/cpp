#include "base/memory.h"
#include "adapter.h"

using namespace PLAYER;

Adapter::Adapter() {
}
//Adapter::~Adapter() {}

void Adapter::initialize(byte** pData) {}

void Adapter::prepare() {}
void Adapter::cleanUp() {}

int Adapter::compare(COLLECTION_ARGUMENTS) {
	return (*(Adapter**)value)->getInfo()->id - (*(Adapter**)key.p)->getInfo()->id;
}
#ifdef PLAYER_EDIT_MODE
void Adapter::writeToStream(Stream* data) {
	data->writeByte(getInfo()->id);
}
#endif