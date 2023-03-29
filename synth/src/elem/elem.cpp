#include "elem.h"
#include "math.h"
#include "base/memory.h"

NS_FW_BASE_USE
using namespace SYNTH;

float Elem::FrequencyTable[128];
const float DefaultSamplingRate = 48000.0f;
const float* Elem::samplingRate = &DefaultSamplingRate;

//   0 = 0Hz
//   9
// A440 = 57
const double FreqC0 = 440.0 / pow(2.0, 5.0) * pow(2.0, 3.0 / 12.0);     // C frequency 5 octaves below A:440Hz;

// 00 01 02 03 04 05 06 07 08 09 10 11 12
// C  C# D  D# E  F  F# G  G# A  A# H  C'
// 0 = 0, 1 = C-1, 70 = A4 = 440Hz
void Elem::createFrequencyTable() {
	Elem::FrequencyTable[0] = 0.0f;
	for (var i = 0; i < 127; i++) {
		var oi = trunc(i / 12.0);
		var ti = (i % 12) - 9;
		var base = 440.0 * pow(2.0, oi - 5.0);
		Elem::FrequencyTable[i + 1] = (float)(base * pow(2.0, ti / 12.0));
	}
}

Elem::Elem(int controlCount) : controlCount_(controlCount) {
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

float Elem::run(Arg params) {
	return 0.0f;
}

void Elem::assignControls(PotBase* controls) {}

void Elem::setFromStream(byte*& stream) {}

void Elem::connect(int id, void* input) { }

void Elem::writeToStream(byte*& stream) {
	for (var i = 0; i < controlCount_; i++) {
		controls_[i].writeToStream(stream);
	}
}

float Elem::p2f(float p) {
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

void Elem::createBezierTable(double* table, float px, int steps, DoubleInt2Double transform) {
	var py = 1 - px;
	var ax = 1 - 2 * px, ay = 1 - 2 * py;
	var bx = 2 * px, by = 2 * py;
	for (var i = 0; i <= steps; i++) {
		var x = (float)i / steps;
		var r = (-bx + sqrt(bx * bx + 4.0f * ax * x)) / (2.0f * ax);
		var y = r * r * ay + by * r;
		table[i] = transform(y, i);
	}
}