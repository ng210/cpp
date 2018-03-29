#include "DataSeries.h"



DataSeries::DataSeries() {
	cursor_ = 0;
}
DataSeries::DataSeries(Array* src) {
	cursor_ = 0;
}
DataSeries::~DataSeries() {
}

//void DataSeries::insertAt(int ix, void* data);
//void DataSeries::removeAt(int ix);
POINT* DataSeries::previous() {
	return NULL;
}
POINT* DataSeries::next() {
	return NULL;
}
