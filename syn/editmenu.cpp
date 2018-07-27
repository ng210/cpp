#include "synapp.h"
#include "IAdapter.h"

LRESULT SynApp::addAdapter() {
	return 0;
}

LRESULT SynApp::addSequence() {
	return 0;
}

LRESULT SynApp::addUserDataBlock() {
	// test: add user data blocks
	// #1 ini for synthadapter
	// #2 sound bank for synth

	// 0000 UINT8 synthCount
	// 0001 UINT8 voiceCount for synth1
	// 0002 UINT8 id of userDataBlock (sound bank)
	// 0003 UINT8 voiceCount for synth2
	// 0004 UINT8 id of userDataBlock (sound bank)

	return 0;
}
