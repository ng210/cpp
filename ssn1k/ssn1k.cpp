#include "ssn1k.h"

#define _USE_MATH_DEFINES
#include <math.h>

NS_SSN1K_BEGIN

const float SSN1K::referenceFrequency = 20.60172230705f;
float SSN1K::sampleRate = 48000.0f;
float SSN1K::sampleRateR = 1.0f/48000.0f;
float SSN1K::PI_sampleRateR = (float)(M_PI / 48000.0f);
INTERPOLATE* SSN1K::interpolate = SSN1K::flat;


void SSN1K::setSampleRate(float value) {
	SSN1K::sampleRate = value;
	SSN1K::sampleRateR = 1.0f / value;
	SSN1K::PI_sampleRateR = (float)M_PI / value;
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
	float f = ((*(int*)&pitch) == 0 ? 0.0f : (float)pow(1.05946309436f, pitch) * SSN1K::referenceFrequency);
#ifdef _PROFILE
	SSN1K_PROFILER.leave(4);
#endif
	return f;
}

float SSN1K::sinusoid(float x) {
	return (float)(1.0f - cos(x * M_PI)) / 2;
}
float SSN1K::smoothstep(float x) {
	return x*x*(3 - 2 * x);
}
float SSN1K::flat(float x) {
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
	return (float)(rand() - (RAND_MAX >> 1)) / RAND_MAX;
};
#endif

NS_SSN1K_END