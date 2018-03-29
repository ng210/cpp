#ifndef __KEYVALUEPAIR_H
#define __KEYVALUEPAIR_H

#include "collection/array.h"

NS_FW_BASE_BEGIN
class Map;
//*****************************************************************************
// Key-value pair
//*****************************************************************************
class KeyValuePair {
	friend class Map;
protected: PROP_R(void*, key);
protected: PROP_R(void*, value);
protected:
	PROP_R(unsigned int, index);
public:
	KeyValuePair();
	KeyValuePair(void* key, void* value);
};

NS_FW_BASE_END

#endif