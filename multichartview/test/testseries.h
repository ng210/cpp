#ifndef __TESTSERIES_H
#define __TESTSERIES_H

#include "multichartview/dataseries.h"

NS_FW_BASE_USE

typedef struct TESTDATA_ {
	int i;
	float f;
	int i2;
	int i3;
} TESTDATA;

class TestSeries : public DataSeries {
public:
	TestSeries(int size);
	int get(int ix, int seriesId, double* value);
	int set(int ix, int seriesId, double* value);
	void* add(int index, int chnId);
	void remove(int index, int chnId);
};

#endif