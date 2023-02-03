//*****************************************************************************
// Base module
//*****************************************************************************

#ifndef __ELEM_H
#define __ELEM_H

#include "basedef.h"
#include "pot.h"

#define _USE_MATH_DEFINES

#define SYNTH_THETA 6.283185307179586476925286766559

NS_FW_BASE_USE
namespace SYNTH {

	typedef union Arg {
		float f;
		int i;
		void* p;
		Arg(float v) : f(v) {}
		Arg(int v) : i(v) {}
		Arg(void* v) : p(v) {}
		Arg() : p(NULL) {}
	} Arg;

	//typedef union CtrlUnion {
	//	float f;
	//	int i;
	//	CtrlUnion() : i(0) {}
	//	CtrlUnion(int v) : i(v) {}
	//	CtrlUnion(float v) : f(v) {}
	//} CtrlUnion;

	//typedef CtrlUnion Ctrl;

	typedef float FloatInt2Float(float y, int ix);

	//struct ElemCtrls {
	//	PotF amp;
	//};

	//enum MixMode : unsigned char {
	//	SSN1K_MM_OVR = 0x00	// overwrite
	//,	SSN1K_MM_ADD = 0x01	// add
	//,	SSN1K_MM_MUL = 0x02	// multiply
	//,	SSN1K_MM_BPS = 0x03	// bypass
	//,	SSN1K_MM_MIX = 0x04	// mix

	//,	SSN1K_MM_COUNT
	//};

	class Elem {
	//protected: PROP(const float*, samplingRate);
	public:
		Elem();

		virtual void assignControls(PotBase* controls) = 0;
		virtual void setFromStream(byte*& stream);
		virtual void connect(int id, void* input);
		virtual float run(Arg params = (void*)NULL);
		//virtual void run(float* buffer, int start, int end);

		static float FrequencyTable[128];
		static void createFrequencyTable();
		static float p2f(float p);
		static void createBezierTable(float* table, float px, int steps, FloatInt2Float transform);

		static const float* samplingRate;
	};

}

#endif