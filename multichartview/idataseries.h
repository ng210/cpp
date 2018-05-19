#ifndef __DATASERIES_H
#define __DATASERIES_H

#include "collection/Array.h"

NS_FW_BASE_USE

#ifndef POINT
typedef struct POINT_ {
	int x;
	int y;
} POINT;
#endif

typedef POINT& (DATASERIESGET)(int);

class DataSeries : public Array {
	PROP_R(int, cursor);
public:
	DataSeries();
	DataSeries(Array* source);
	virtual ~DataSeries();

	//void insertAt(int ix, void* data);
	//void removeAt(int ix);
	POINT* previous();
	POINT* next();
};

#endif