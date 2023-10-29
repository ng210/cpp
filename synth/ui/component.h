#ifndef __COMPONENT_H
#define __COMPONENT_H

#include "win/winapp.h"

template <class T> class Component {
	PROP_R(T*, hostApp);

public:
	Component(T* app) : hostApp_(app) { }
};

#endif