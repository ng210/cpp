#ifndef __CLP_H
#define __CLP_H

#include "synth/src/elem/elem.h"

NS_FW_BASE_USE
namespace SYNTH {

	typedef struct ClpValues_ {
		Value amp, lvl;
	} ClpValues;



	class Clp : public Elem {
	protected: PROP(ClpValues*, values);
	public:
		Clp();
		float run(Arg param = (void*)NULL);
	};
}
#endif