#include "mdl.h"
#include "math.h"
#include "base/memory.h"

NS_FW_BASE_USE
using namespace SYNTH;

float Mdl::FrequencyTable[128];
const float DefaultSamplingRate = 48000.0f;
//   0 = 0Hz
//   9
// A440 = 57
const double FreqC0 = 440.0 / pow(2.0, 5.0) * pow(2.0, 3.0 / 12.0);     // C frequency 5 octaves below A:440Hz;

// 00 01 02 03 04 05 06 07 08 09 10 11 12
// C  C# D  D# E  F  F# G  G# A  A# H  C'
// 0 = 0, 1 = C-1, 70 = A4 = 440Hz
void Mdl::createFrequencyTable() {
	Mdl::FrequencyTable[0] = 0.0f;
	for (var i = 0; i < 127; i++) {
		var oi = trunc(i / 12.0);
		var ti = (i % 12) - 9;
		var base = 440.0 * pow(2.0, oi - 5.0);
		Mdl::FrequencyTable[i + 1] = (float)(base * pow(2.0, ti / 12.0));
	}
}

Mdl::Mdl() {
	samplingRate_ = &DefaultSamplingRate;
}

//float Mdl::mix(Pot in1, Pot in2) {
//	MdlCtrls& ctrls = (MdlCtrls&)controls_;
//	float smp = in1.f * ctrls.amp.f + ctrls.dc.f;
//	switch (ctrls.mix.i) {
//	case SSN1K_MM_ADD:
//		smp = 0.5f * (smp + in2.f);
//		break;
//	case SSN1K_MM_MUL:
//		smp *= in2.f;
//		break;
//	case SSN1K_MM_BPS:
//		smp = in2.f;
//		break;
//	case SSN1K_MM_OVR:
//	default:
//		break;
//	}
//	return smp;
//}

void Mdl::assignControls(Pot* controls) {}

void Mdl::setFromStream(byte* stream) {}

void Mdl::setFromStream(byte*& stream, Pot* target) {
	var ctrls = (MdlCtrls*)target;
	ctrls->amp.setFromStream(stream);
}

float Mdl::p2f(float p) {
	//if (p > 1.0f) {
	//	var i = (int)p;
	//	var f = (double)p - i;
	//	var f1 = Mdl::FrequencyTable[i];
	//	var f2 = Mdl::FrequencyTable[i + 1];
	//	return (float)((1 - f) * f1 + f * f2);
	//}
	//else {
	//	return 0.0;
	//}

	return p == 0.0f ? 0.0f : (float)(pow(2.0, p / 12.0) * FreqC0);
};