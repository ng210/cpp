#include "seqseries.h"
#include "base/memory.h"
#include "utils/utils.h"
#include "player/frame.h"

NS_PLAYER_USE

SeqSeries::SeqSeries(Sequence* sequence, IAdapter* adapter) {
	data_ = (Array*)sequence;
	adapter_ = adapter;
}

Sequence* SeqSeries::sequence() {
	return (Sequence*)data_;
}

int SeqSeries::get(int ix, int seriesId, double* cmdParameters) {
	int result = 0;
	// get frame at ix
	Frame* frame = sequence()->frame(ix);
	if (frame != NULL) {
		// get command at frame
		for (UINT32 i = 0; i < frame->commands()->length(); i++) {
			PLAYER_COMMAND cmd = (PLAYER_COMMAND)frame->commands()->getAt(i);
			if (adapter_->matchCommand(seriesId, cmd)) {
				result = adapter_->getCommandParameters(cmd, cmdParameters);
			}
		}
	}

	return result;
}
int SeqSeries::set(int ix, int seriesId, double* value) {
	int result = 0;

	return result;
}
void* SeqSeries::add(int index, int chnId) {
	void* res = NULL;
	return res;
}
void SeqSeries::remove(int index, int chnId) {
}
