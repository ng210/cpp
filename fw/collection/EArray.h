#ifndef __EARRAY_H
#define __EARRAY_H

//*****************************************************************************
// Extending EArray
//*****************************************************************************

#include "Collection/SArray.h"

class EArray : public SArray
{
	// Number of bytes to extend.
	int m_iExtendSize;
public:
	EArray();
	EArray(size_t iExtendSize);
	EArray(size_t iItemSize, size_t iExtendSize);
	virtual ~EArray(void);

	void init(size_t iItemSize, size_t iExtendSize, int bRef);

	COLLECTIONITEM add();
};
#endif