//*****************************************************************************
// EArray - Extending array
//*****************************************************************************

#include "EArray.h"

//*********************************************************
EArray::EArray() {
}

EArray::~EArray() {
	_VS_debug("EArray::~EArray\n");
}

//*********************************************************
EArray::EArray(size_t iExtendSize) {
	init(sizeof(void*), iExtendSize, 1);
}
//*********************************************************
EArray::EArray(size_t iItemSize, size_t iExtendSize) {
	init(iItemSize, iExtendSize, 0);
}
//*********************************************************
void EArray::init(size_t iItemSize, size_t iExtendSize, int bRef) {
	// set reference flag
	Collection::init(bRef);
	// initial size is 0
	m_iSize = 0;
	// item size is 4 bytes aligned
	m_iItemSize = ALIGN4(iItemSize);
	// calculate extend size in bytes
	m_iExtendSize = iExtendSize * m_iItemSize;
	// initial capacity is 0
	m_iCapacity = 0;
	m_pData = NULL;
}
//*********************************************************
COLLECTIONITEM EArray::add() {
	if (m_iSize == (size_t)m_iCapacity) {
		m_iCapacity += m_iExtendSize;
		char *pData;
		NEWBUF(pData, m_iCapacity);
		if (m_iSize != 0) {
			memcpy(pData, m_pData, m_iSize);
			DEL(m_pData);
		}
		m_pData = pData;
	}
	return SArray::add();
}
