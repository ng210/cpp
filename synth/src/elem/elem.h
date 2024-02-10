//*****************************************************************************
// Base module
//*****************************************************************************

#ifndef __ELEM_H
#define __ELEM_H

#define SYNTH_THETA 6.283185307179586476925286766559

#include "basedef.h"
#include "base/stream.h"
#include "player/src/value.h"

NS_FW_BASE_USE
using namespace PLAYER;
namespace SYNTH {

#define SAMPLE_BUFFER_SIZE 256

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
	public:
		virtual void setFromStream(byte*& stream);
		virtual float run(Arg params = (void*)NULL);
		//virtual void run(float* buffer, int start, int end);

		static float FrequencyTable[128];
		static void createFrequencyTable();
		static float p2f(float p);
		static void createBezierTable(double* table, float px, int steps, DoubleInt2Double transform);

		static const float* samplingRate;
	};

}

#endif