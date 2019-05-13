#include "collection/keyvaluepair.h"

NS_FW_BASE_BEGIN


KeyValuePair::KeyValuePair() {
	key_ = NULL;
	value_ = NULL;
}

KeyValuePair::KeyValuePair(void* key, void* value) {
	key_ = key;
	value_ = value;
}

NS_FW_BASE_END
