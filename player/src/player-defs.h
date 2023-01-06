#ifndef __PLAYER_DEFS_H
#define __PLAYER_DEFS_H

#include "basedef.h"

enum class DataBlockItemFlag {
	None = 0,
	Allocated = 1
};
inline int operator &(DataBlockItemFlag a, DataBlockItemFlag b) {
	return (int)a & (int)b;
}
inline int operator |(DataBlockItemFlag a, DataBlockItemFlag b) {
	return (int)a | (int)b;
}

typedef struct DataBlockItem {
	int length;
	byte* dataBlock;
	DataBlockItemFlag flag;
	DataBlockItem() { length = 0; dataBlock = NULL; flag = DataBlockItemFlag::None; }
	DataBlockItem(int len, byte* data, DataBlockItemFlag f) : length(len), dataBlock(data), flag(f) {}
} DataBlockItem;


#endif