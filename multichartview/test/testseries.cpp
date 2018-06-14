#include "testseries.h"
#include "base/memory.h"
#include "utils/utils.h"

TestSeries::TestSeries(int size) {
	data_ = NEW_(Array, sizeof(TESTDATA));
	int delta = 0;
	for (int i = 0; i < size; i++) {
		TESTDATA testData = {
			(int)Utils::random(100),
			(float)Utils::random(),
			-1,
			0
		};
		if (delta-- > 0) {
		}
		else {
			delta = (int)Utils::random(3) + 1;
			testData.i2 = (int)Utils::random(100);
			testData.i3 = delta;
		}
		data_->add(&testData);
	}

}

int TestSeries::get(int ix, int seriesId, double* value) {
	int result = 0;
	TESTDATA* data = (TESTDATA*)data_->getAt(ix);
	if (data != NULL) {
		switch (seriesId) {
			case 0: value[0] = data->i / 100.0; break;
			case 1: value[0] = (double)data->f; break;
			case 2: value[0] = data->i / 100.0 - data->f; break;
			case 3: 
				value[0] = data->i2 / 100.0;
				value[1] = (double)data->i3; break;
		}
		result = 1;
	}
	return result;
}
int TestSeries::set(int ix, int seriesId, double* value) {
	int result = 0;
	TESTDATA* data = (TESTDATA*)data_->getAt(ix);
	if (data != NULL) {
		switch (seriesId) {
			case 0: data->i = (int)(*value * 100.0); break;
			case 1: data->f = (float)*value; break;
			case 2: break;
			case 3: data->i2 = (int)(*value * 100.0); break;
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
