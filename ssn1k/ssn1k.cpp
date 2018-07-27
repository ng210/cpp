#include "ssn1k.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include "utils/utils.h"

NS_FW_BASE_USE
NS_SSN1K_BEGIN

const float SSN1K::referenceFrequency = 16.351597831287414667365624595207f; // C-1
float SSN1K::sampleRate = 48000.0f;
float SSN1K::sampleRateR = 1.0f/48000.0f;
float SSN1K::theta = (float)(2*M_PI / 48000.0f);
INTERPOLATE* SSN1K::interpolate = SSN1K::flat;
float SSN1K::_2PI = 0.0f;


void SSN1K::setSampleRate(float value) {
	SSN1K::sampleRate = value;
	SSN1K::sampleRateR = 1.0f / value;
	SSN1K::theta = (float)(2*M_PI / value);
	SSN1K::_2PI = (float)(2.0 * M_PI);
}
float SSN1K::getSampleRate() {
	return SSN1K::sampleRate;
}
float SSN1K::getSampleRateR() {
	return SSN1K::sampleRateR;
}
float SSN1K::p2f(float pitch) {
#ifdef _PROFILE
	SSN1K_PROFILER.enter(4);
#endif
	// c = pow(2, 1/12); f = pow(c, pitch)*ref_freq
	float f = (*(int*)&pitch == 0) ? 0.0f : (float)pow(1.0594630943592952645618252949463f, pitch) * SSN1K::referenceFrequency;
#ifdef _PROFILE
	SSN1K_PROFILER.leave(4);
#endif
	return f;
}
float SSN1K::getTheta() {
	return SSN1K::theta;
}
float SSN1K::get2PI() {
	return SSN1K::_2PI;
}

float SSN1K::sinusoid(float x, float min, float max) {
	float length = max - min;
	x = (x - min) / length;
	return length*(float)(1.0f - cos(x * M_PI)) / 2 + min;
}
float SSN1K::smoothstep(float x, float min, float max) {
	float length = max - min;
	x = (x - min) / length;
	return length * x*x*(3 - 2 * x) + min;
}
float SSN1K::flat(float x, float min, float max) {
	return x;
}

#ifdef _USE_CUSTOM_RND
int random_seed = 0x79933;

__declspec(naked) float SSN1K::rnd() {
	__asm {
		imul	eax, random_seed, 16807
		push	dword ptr 0xc0400000	// -3.0f
		mov		random_seed, eax
		and		eax, 0x007fffff
		fld		dword ptr[esp]
		or eax, 0x40000000
		mov[esp], eax
		fadd	dword ptr[esp]
		pop		eax
		ret
	}
}
#else

float SSN1K::rnd() {
	return (float)Utils::randomSigned();
};
#endif

NS_SSN1K_END