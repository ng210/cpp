#ifndef __DATASERIES_H
#define __DATASERIES_H

#include "collection/array.h"

NS_FW_BASE_USE

class DataSeries {
protected:	PROP_R(Array*, data);
public:
	DataSeries() : data_(NULL) {};
	DataSeries(Array* source) : data_(source) {};
	virtual int getLength() { return data_->length(); };
	virtual int get(int ix, int seriesId, double* value) = 0;
	virtual int set(int ix, int seriesId, double* value) = 0;
	virtual void* add(int index, int chnId) = 0;
	virtual void remove(int index, int chnId) = 0;
};

#endif