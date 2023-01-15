#include "collection/keyvaluepair.h"

NS_FW_BASE_USE


KeyValuePair::KeyValuePair() {
	key_ = NULL;
	value_ = NULL;
	index_ = 0;
}

KeyValuePair::KeyValuePair(Key key, void* value) {
	key_ = key;
	value_ = value;
	index_ = 0;
}
