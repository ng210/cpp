#include "testseries.h"
#include "base/memory.h"

int TestSeries::get(int ix, int seriesId, double& value) {
	int result = 0;
	TESTDATA* data = (TESTDATA*)data_->getAt(ix);
	if (data != NULL) {
		switch (seriesId) {
			case 0: value = data->i / 100.0; break;
			case 1: value = (double)data->f; break;
			case 2: value = data->i / 100.0 - data->f; break;
		}
		result = 1;
	}
	return result;
}
int TestSeries::set(int ix, int seriesId, double value) {
	int result = 0;
	TESTDATA* data = (TESTDATA*)data_->getAt(ix);
	if (data != NULL) {
		switch (seriesId) {
			case 0: data->i = (int)(value * 100.0); break;
			case 1: data->f = (float)value; break;
			case 2: break;
		}
		result = 1;
	}
	return result;
}
void* TestSeries::add(int index, int chnId) {
	void* res = NULL;
	return res;
}
void TestSeries::remove(int index, int chnId) {
}
