#include <stdio.h>
#include <math.h>

#include "xmloader.h"
#include "base/memory.h"
#include "utils/file.h"

#include "player.h"
#include "playeradapter.h"

NS_PLAYER_USE
NS_SSN1K_USE

static char* NOTES[] = { "C-", "C#", "D-", "D#", "E-", "F-", "F#", "G-", "G#", "A-", "A#", "H-" };

static char dummy[16] = "";
class XmEffectMap : public Map {
public:
	XmEffectMap() {
		init(sizeof(int), 4*sizeof(char), Map::hashingItem, Collection::compareInt);
		int key; char* value;

		key = 0x00; value = "0"; put(&key, value); // 00 // Appregio
		key = 0x01; value = "1"; put(&key, value); // 01 // Porta up
		key = 0x02; value = "2"; put(&key, value); // 02 // Porta down
		key = 0x03; value = "3"; put(&key, value); // 03 // Tone porta
		key = 0x04; value = "4"; put(&key, value); // Vibrato
		key = 0x05; value = "5"; put(&key, value); // Tone porta + Volume slide
		key = 0x06; value = "6"; put(&key, value); // Vibrato + Volume slide
		key = 0x07; value = "7"; put(&key, value); // Tremolo
		key = 0x08; value = "8"; put(&key, value); // Set panning
		key = 0x09; value = "9"; put(&key, value); // Sample offset
		key = 0x0A; value = "A"; put(&key, value); // Volume slide
		key = 0x0B; value = "B"; put(&key, value); // Position jump
		key = 0x0C; value = "C"; put(&key, value); // Set volume
		key = 0x0D; value = "D"; put(&key, value); // Pattern break
		key = 0x0E; value = "E"; put(&key, value); // Fine porta up
		//key = 0xE1; value = "E1"; put(&key, value); // Fine porta up
		//key = 0xE2; value = "E2"; put(&key, value); // Fine porta down
		//key = 0xE3; value = "E3"; put(&key, value); // Set gliss control
		//key = 0xE4; value = "E4"; put(&key, value); // Set vibrato control
		//key = 0xE5; value = "E5"; put(&key, value); // Set finetune
		//key = 0xE6; value = "E6"; put(&key, value); // Set loop begin / loop
		//key = 0xE7; value = "E7"; put(&key, value); // Set tremolo control
		//key = 0xE9; value = "E9"; put(&key, value); // Retrig note
		//key = 0xEA; value = "EA"; put(&key, value); // Fine volume slide up
		//key = 0xEB; value = "EB"; put(&key, value); // Fine volume slide down
		//key = 0xEC; value = "EC"; put(&key, value); // Note cut
		//key = 0xED; value = "ED"; put(&key, value); // Note delay
		//key = 0xEE; value = "EE"; put(&key, value); // Pattern delay
		key = 0x0F; value = "F"; put(&key, value); // Set tempo / BPM
		key = 0x10; value = "G"; put(&key, value); // Set global volume
		key = 0x11; value = "H"; put(&key, value); // Global volume slide
		key = 0x14; value = "K"; put(&key, value); // Key off 
		key = 0x15; value = "L"; put(&key, value); // Set envelope position
		key = 0x19; value = "P"; put(&key, value); // Panning slide
		key = 0x1B; value = "R"; put(&key, value); // Multi retrig note
		key = 0x1D; value = "T"; put(&key, value); // Tremor
		key = 0x21; value = "X"; put(&key, value); // X1 - Extra fine porta up / X2 - Extra fine porta down
//		key = 0x21; value = "-X2"; put(&key, value); // X2 - Extra fine porta down
	};
	~XmEffectMap() {
	}
	char* get(int key) {
		void* value = Map::get(&key);
		if (value == NULL) {
			sprintf(dummy, "%02x", key);
			value = &dummy;
		}
		return (char*)value;
	}
};

static XmEffectMap* xmEffectMap_ = NULL;

XmLoader::XmLoader(PlayerAdapter* playerAdapter, SynthAdapter* synthAdapter) : patternOrder_(Array(sizeof(UINT8))) {
	playerAdapter_ = playerAdapter;
	synthAdapter_ = synthAdapter;
	if (xmEffectMap_ == NULL) {
		xmEffectMap_ = NEW_(XmEffectMap);
	}
}

XmLoader::~XmLoader(void) {
	ARRAY_FOREACH(&patterns_, FREE(value););
	DEL_(xmEffectMap_);
}

// NOTE
XMNOTE* XmLoader::readNote(UINT8*& ptr, XMNOTE* xmNote) {
	UINT8 code = *ptr++;
	xmNote->hasData = (code != 0x80);
	bool isCompressed = code & 0x80;
	int note = -1;
	if (!isCompressed) {
		note = code - 1;
		code = 0x9e;
	}
	code &= 0x1f;
	xmNote->note = (code & 0x01) ? *ptr++-1 : note;
	xmNote->instrument = (code & 0x02) ? *ptr++-1 : -1;
	xmNote->volume = (code & 0x04) ? *ptr++ : -1;
	xmNote->effect = (code & 0x08) ? *ptr++ : -1;
	xmNote->parameter = (code & 0x10) ? *ptr++ : -1;
	return xmNote;
}
PArray* XmLoader::convertNote(XMNOTE* xmNote, XMNOTE& state) {
	PLAYER_COMMAND cmd = NULL;
	PArray* commands = NEW_(PArray);
	if (xmNote->hasData) {
		// process instrument
		int instrument = xmNote->instrument;
		if (instrument != 0xFF && instrument != state.instrument) {
			state.instrument = instrument;
			cmd = synthAdapter_->createCommand(Synth_Cmd_prgChange, instrument);
			commands->add(cmd);
		}
		// process volume
		int volume = xmNote->volume;
		if (volume != 0xFF) {
			volume -= 0x10;
			if (volume < 0 || volume > 0x40) {
				volume = state.volume;
			}
			state.volume = volume;
		}
		// process note
		int note = xmNote->note;
		if (note >= 0x00 && note < 0x60) {
			note += 12;
			//if (instrument != 0xff) {
				// note on
				int v = volume == 0xFF ? 0x40 : volume;
				cmd = synthAdapter_->createCommand(Synth_Cmd_setControlF, SSN1K_CI_Env1Amp, 1.0);
				commands->add(cmd);
				cmd = synthAdapter_->createCommand(Synth_Cmd_setNoteOn, note, volume*255 >> 6);
				commands->add(cmd);
				volume = 0xFF;
			//} else {
			//	if (note != state.note) {
			//		// set note
			//		cmd = synthAdapter_->createCommand(Synth_Cmd_setControlF, SSN1K_CI_Tune, (double)note);
			//		commands->add(cmd);
			//	}
			//}
			state.note = note;
		}
		if (note == 0x60 && xmNote->note != state.note) {
			cmd = synthAdapter_->createCommand(Synth_Cmd_setNoteOff, state.note);
			commands->add(cmd);
			state.note = xmNote->note;
		}
		if (volume != 0xFF) {
			cmd = synthAdapter_->createCommand(Synth_Cmd_setControlF, SSN1K_CI_Env1Amp, volume / 255.0);
			commands->add(cmd);
		}
	}
	//v = xmNote.instrument;
	//(v != 0xff) ? printf("%02X ", v) : printf("-- ");
	//v = xmNote.volume;
	//(v != 0xff) ? printf("%02X ", v) : printf("-- ");
	//v = xmNote.effect;
	//(v != 0xff) ? printf("%s", xmEffectMap_->get(v)) : printf("--");
	//v = xmNote.parameter;
	//(v != 0xff) ? printf("%02X", v) : printf("--");
	if (commands->length() == 0) {
		DEL_(commands);
		commands = NULL;
	}
	return commands;
}
int XmLoader::printNote(XMNOTE* xmNote, char* buffer) {
	char* ptr = buffer;
	if (xmNote->hasData) {
		UINT8 v = xmNote->note;
		ptr += (v != 0xff) ? (v != 96) ? sprintf(ptr, "%s%d ", NOTES[v % 12], v / 12) : sprintf(ptr, "off ") : sprintf(ptr, "--- ");
		v = xmNote->instrument;
		ptr += (v != 0xff) ? sprintf(ptr, "%02X ", v) : sprintf(ptr, "-- ");
		v = xmNote->volume;
		ptr += (v != 0xff) ? sprintf(ptr, "%02X ", v) : sprintf(ptr, "-- ");
		v = xmNote->effect;
		ptr += (v != 0xff) ? sprintf(ptr, "%s", xmEffectMap_->get(v)) : sprintf(ptr, "-");
		v = xmNote->parameter;
		ptr += (v != 0xff) ? sprintf(ptr, "%02X", v) : sprintf(ptr, "--");
	} else {
		ptr += sprintf(ptr, "... .. .. ...");
	}
	return (int)(ptr - buffer);
}

// PATTERN
XMPATTERN* XmLoader::readPattern(XMFILEPATTERN* ptr) {
	XMPATTERN* pattern = (XMPATTERN*)MALLOC(UINT8, sizeof(XMPATTERN) + sizeof(XMNOTE)*ptr->rowCount*channelCount_);
	pattern->rowCount = ptr->rowCount;
	pattern->data = (XMNOTE*)((UINT8*)pattern + sizeof(XMPATTERN));
	UINT8* packedData = &ptr->packedData;
	int i = 0;
	for (int r = 0; r < ptr->rowCount; r++) {
		for (int ch = 0; ch < channelCount_; ch++) {
			readNote(packedData, &pattern->data[i++]);
		}
	}
	return pattern;
}
void XmLoader::convertPattern(XMPATTERN* pattern, PArray* sequences) {
	for (int ch = 0; ch < channelCount_; ch++) {
		bool hasFrames = false;
		Sequence* sequence = NEW_(Sequence);
		for (int ri = 0; ri < pattern->rowCount; ri++) {
			XMNOTE* xmNote = &pattern->data[ch + ri*channelCount_];
			if (xmNote->hasData) {
				PArray* commands = convertNote(xmNote, states_[ch]);
				if (commands != NULL) {
					Frame* frame = sequence->addFrame(ri);
					hasFrames = true;
					ARRAY_FOREACH(commands, frame->addCommand((PLAYER_COMMAND)value));
					DEL_(commands);
				}
			}
		}
		int id = -1;
		if (hasFrames) {
			id = sequences->length();
			sequence->addFrame(pattern->rowCount)->addCommand(playerAdapter_->createEndCommand());
			sequences->add(sequence);
		} else {
			DEL_(sequence);
		}
		pattern->sequences[ch] = id;
	}
}
int XmLoader::printPattern(XMPATTERN* pattern, char* buffer) {
	int n = 0;
	char* ptr = buffer;
	for (int r = 0; r < pattern->rowCount; r++) {
		ptr += sprintf(ptr, "%02x: ", r);
		for (int ch = 0; ch < channelCount_; ch++) {
			XMNOTE* xmNote = &pattern->data[n++];
			ptr += printNote(xmNote, ptr);
			if (ch < channelCount_ - 1) {
				ptr += sprintf(ptr, "|");
			}
		}
		ptr += sprintf(ptr, "\n");
	}
	return (int)(ptr - buffer);
}

// LOADER
int XmLoader::load(const char *szPath) {
	int res = 1;
	UINT8* xm = File::read(szPath);
	if (xm != NULL) {
		res = 0;
		// read header
		XMFILEHEADER *header = (XMFILEHEADER*)xm;
		bpm_ = (float)header->defaultBPM;
		ticks_ = 24.0f / header->defaultTempo;
		channelCount_ = header->channelCount;
		for (int i = 0; i < header->songLength; i++) {
			patternOrder_.add(&header->patternOrder + i);
		}
		// read pattern
		UINT8* ptr = xm + offsetof(XMFILEHEADER, headerSize) + header->headerSize;
		for (int p = 0; p < header->patternCount; p++) {
			XMFILEPATTERN* xmPattern = (XMFILEPATTERN*)ptr;
			patterns_.add(readPattern(xmPattern));
			ptr += offsetof(XMFILEPATTERN, packedData) + xmPattern->packedDataSize;
		}
		res = 0;
		FREE(xm);
	}
	return res;
}
PArray* XmLoader::convert() {
	// create master sequence
	PArray* sequences = NEW_(PArray);
	Sequence* masterSequence = NEW_(Sequence);
	sequences->add(masterSequence);
	Frame* frame = masterSequence->addFrame(0);
	frame->addCommand(playerAdapter_->createCommand(Player_Cmd_setTempo, bpm_, ticks_));

	// reset channel states
	for (int i = 0; i < channelCount_; i++) {
		memset(states_, -1, sizeof(XMNOTE));
		states_[i].hasData = 0;
	}	

	// convert patterns
	for (UINT32 pi = 0; pi < patterns_.length(); pi++) {
		XMPATTERN* pattern = (XMPATTERN*)patterns_.getAt(pi);
		convertPattern(pattern, sequences);
	}

	// add sequences to master sequence
	int rowCounter = 0;
	for (UINT32 i = 0; i < patternOrder_.length(); i++) {
		UINT8 pi = *(UINT8*)patternOrder_.getAt(i);
		if (i > 0) frame = NULL;
		XMPATTERN* pattern = (XMPATTERN*)patterns_.getAt(pi);
		for (int ch = 0; ch < channelCount_; ch++) {
			int sequenceId = pattern->sequences[ch];
			if (sequenceId != -1) {
				if (frame == NULL) {
					frame = masterSequence->addFrame(rowCounter);
				}
				frame->addCommand(playerAdapter_->createCommand(Player_Cmd_assign, ch+1, sequenceId, Player_Flg_Active));
			}
		}
		rowCounter += pattern->rowCount;
	}

	masterSequence->addFrame(rowCounter)->addCommand(playerAdapter_->createEndCommand());
	return sequences;
}