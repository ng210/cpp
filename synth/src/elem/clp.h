#ifndef __CLP_H
#define __CLP_H

#include "elem.h"

NS_FW_BASE_USE
namespace SYNTH {

	typedef struct ClpCtrls {
		PotF amp;
		PotF8 lvl;
	} ClpCtrls;

#define ClpCtrlsCount (sizeof(ClpCtrls) / sizeof(Pot))


	class Clp : public Elem {
	private: PROP_R(ClpCtrls*, controls);
	public:
		Clp();
		virtual ~Clp();
		void assignControls(PotBase* controls);
		void setFromStream(byte* stream);
		float run(Arg param = (void*)NULL);
	};
}
#endif