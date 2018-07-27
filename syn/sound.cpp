#include "synapp.h"
//#include "bank.h"
//
//static UINT8* instruments[] = { chords, solo, /*saw, pls,*/ bass, bassline, kick, snare };
//static SYNTH_BANK instrumentData;
//
//static SYNTH_BANK* getInstrumentData() {
//	instrumentData.instrumentCount = arraysize(instruments);
//	instrumentData.instrumentData = instruments;
//	return &instrumentData;
//}

void SynApp::setupSound() {
	SSN1K::setSampleRate((float)sampleRate_);
	//SSN1K::interpolate = SSN1K::smoothstep;
	//SSN1K::interpolate = SSN1K::sinusoid;
	SSN1K::interpolate = SSN1K::flat;
}

void soundCallback(short* buffer, int sampleCount) {
	SynApp::instance()->feedSample(buffer, sampleCount);
}

static size_t frameCounter_;
static size_t frames_;
void SynApp::feedSample(short* buffer, int sampleCount) {
	size_t start = 0;
	size_t end = 0;
	size_t remains = sampleCount;
	while (remains) {
		if (frameCounter_ == 0) {
			player_->run(1);
			frameCounter_ = (size_t)(48000.0f / player_->refreshRate());
			frames_++;
		}
		size_t len = frameCounter_ < remains ? frameCounter_ : remains;
		end = start + len;
		frameCounter_ -= len;
		synthAdapter_->run(buffer, start, end);
		start = end;
		remains -= len;
	}
}