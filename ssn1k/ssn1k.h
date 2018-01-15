#ifndef __SSN1K_H
#define __SSN1K_H

#ifndef _USE_CUSTOM_RND
#include <stdlib.h>
#endif

#define NS_SSN1K ssn1k
#define NS_SSN1K_BEGIN  namespace NS_SSN1K {
#define NS_SSN1K_END }
#define NS_SSN1K_USE  using namespace NS_SSN1K;

#ifndef offsetof
#define offsetof(t, f) ((int)&((t*)0)->f)
#endif

NS_SSN1K_BEGIN

typedef float (INTERPOLATE)(float);

class SSN1K {
	static const float referenceFrequency;
	static float sampleRate;
	static float sampleRateR;
	static float PI_sampleRateR;
public:
	static void setSampleRate(float);
	static float getSampleRate();
	static float SSN1K::getSampleRateR();
	static float p2f(float);
	static INTERPOLATE* interpolate;

	static float sinusoid(float x);
	static float smoothstep(float x);
	static float flat(float x);
	static float rnd();
};

NS_SSN1K_END

#endif

