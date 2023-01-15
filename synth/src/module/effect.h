#ifndef __EFFECT_H
#define __EFFECT_H

#include "elem.h"

NS_FW_BASE_USE
namespace SYNTH {
	class Effect : public Elem {
	public:
		void run(float* buffer, int start, int length);
	};

}

#endif