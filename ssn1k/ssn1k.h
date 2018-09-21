#ifndef __SSN1K_H
#define __SSN1K_H

#define NS_SSN1K ssn1k
#define NS_SSN1K_BEGIN namespace NS_SSN1K {
#define NS_SSN1K_END }
#define NS_SSN1K_USE using namespace NS_SSN1K;

#ifndef offsetof
#define offsetof(t, f) ((int)&((t*)0)->f)
#endif

#define DWtoDB(w) (unsigned char)(w & 0xff), (unsigned char)(w>>8 & 0xff)

//#include "basedef.h"

NS_SSN1K_BEGIN

typedef float (INTERPOLATE)(float x, float min, float max);


#define C(o)	( 0 + o*12)
#define Cis(o)	( 1 + o*12)
#define D(o)	( 2 + o*12)
#define Dis(o)	( 3 + o*12)
#define E(o)	( 4 + o*12)
#define F(o)	( 5 + o*12)
#define Fis(o)	( 6 + o*12)
#define G(o)	( 7 + o*12)
#define Gis(o)	( 8 + o*12)
#define A(o)	( 9 + o*12)
#define Ais(o)	(10 + o*12)
#define H(o)	(11 + o*12)

class SSN1K {
	static const float referenceFrequency;
	static float sampleRate;
	static float sampleRateR;
	static float theta;
	static float _2PI;
public:
	static void setSampleRate(float);
	static float getSampleRate();
	static float getSampleRateR();
	static float p2f(float);
	static float getTheta();
	static float get2PI();
	static INTERPOLATE* interpolate;

	static float sinusoid(float x, float min, float max);
	static float smoothstep(float x, float min, float max);
	static float flat(float x, float min, float max);
	static float rnd();
};

NS_SSN1K_END

#endif

