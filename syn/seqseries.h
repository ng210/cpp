#ifndef __SEQSERIES_H
#define __SEQSERIES_H

//#include "syndef.h"
#include "multichartview/dataseries.h"
#include "sequence.h"

NS_FW_BASE_USE
NS_PLAYER_USE

class SeqSeries : public DataSeries {
	Sequence* sequence();
	IAdapter* adapter_;
public:
	SeqSeries(Sequence* sequence, IAdapter* adapter);
	int get(int ix, int seriesId, double* value);
	int set(int ix, int seriesId, double* value);
	void* add(int index, int chnId);
	void remove(int index, int chnId);
};

#endif