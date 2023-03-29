//*****************************************************************************
// Base module
//*****************************************************************************

#ifndef __ELEM_H
#define __ELEM_H

#include "basedef.h"
#include "pot.h"

#include "base/stream.h"

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

	typedef float FloatInt2Float(float y, int ix);
	typedef double DoubleInt2Double(double y, int ix);

	class Elem {
	protected: PotBase* controls_;
	protected: PROP(int, controlCount);
	public:
		Elem(int controlCount);

		virtual void assignControls(PotBase* controls) = 0;
		virtual void setFromStream(byte*& stream);
		virtual void connect(int id, void* input);
		virtual float run(Arg params = (void*)NULL);
		//virtual void run(float* buffer, int start, int end);
		virtual void writeToStream(byte*& stream);

		static float FrequencyTable[128];
		static void createFrequencyTable();
		static float p2f(float p);
		static void createBezierTable(double* table, float px, int steps, DoubleInt2Double transform);

		static const float* samplingRate;
	};

}

#endif