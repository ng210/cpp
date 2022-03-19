//*****************************************************************************
// Base module
//*****************************************************************************

#ifndef __MDL_H
#define __MDL_H

#include "basedef.h"
#include "pot.h"

#define _USE_MATH_DEFINES

#define SYNTH_THETA 6.283185307179586476925286766559

NS_FW_BASE_USE
namespace SYNTH {

	//typedef union CtrlUnion {
	//	float f;
	//	int i;
	//	CtrlUnion() : i(0) {}
	//	CtrlUnion(int v) : i(v) {}
	//	CtrlUnion(float v) : f(v) {}
	//} CtrlUnion;

	//typedef CtrlUnion Ctrl;

	struct MdlCtrls {
		PotF amp;
	};

	//enum MixMode : unsigned char {
	//	SSN1K_MM_OVR = 0x00	// overwrite
	//,	SSN1K_MM_ADD = 0x01	// add
	//,	SSN1K_MM_MUL = 0x02	// multiply
	//,	SSN1K_MM_BPS = 0x03	// bypass
	//,	SSN1K_MM_MIX = 0x04	// mix

	//,	SSN1K_MM_COUNT
	//};

	class Mdl {
	protected: PROP(const float*, samplingRate);
	public:
		Mdl();

		void setFromStream(byte*& stream, Pot* target);

		virtual void assignControls(Pot* controls) = 0;
		virtual void setFromStream(byte* stream);
		virtual float run(void* params) = 0;

		static float FrequencyTable[128];
		static void createFrequencyTable();
		static float p2f(float p);
	};

}

#endif