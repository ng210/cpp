/* Load and convert XM patterns
 * - load XM and build structures: patterns
 * - create "raw" sequnces by tracking instruments through the sequenced patterns
 * - optimize sequences by replacing repetitive parts with true repeats
 */

#include <stdio.h>
#include <math.h>
#include "base/string.h"
#include "base/memory.h"
#include "utils/file.h"

#include "synth/src/xmloader.h"
#include "player/src/player-lib.h"

#include "synth/src/device/synth-device.h"
#include "synth/src/device/bass-device.h"
#include "synth/src/device/drums-device.h"
#include "synth/src/device/mixer-device.h"

#define SAMPLING_RATE 48000

NS_FW_BASE_USE
using namespace PLAYER;
using namespace SYNTH;

static const char* NOTES[] = { "C-", "C#", "D-", "D#", "E-", "F-", "F#", "G-", "G#", "A-", "A#", "H-" };

static char dummy[16] = "";
class XmEffectMap : public Map {
public:
	XmEffectMap() {
		initialize(sizeof(int), 4*sizeof(char), Map::hashingItem, Collection::compareInt);
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
			sprintf_s(dummy, 16, "%02x", key);
			value = &dummy;
		}
		return (char*)value;
	}
};

static XmEffectMap* xmEffectMap_ = NULL;

//Device* XmLoader::addDefaultDevice(SynthAdapter* synthAdapter, InstrumentInfo* instrumentInfo) {
//	instrumentInfo->device = NEW_(SynthDevice, synthAdapter);
//	instrumentInfo->voiceCount = 0;
//	var stream = NEW_(Stream);
//	((Module*)instrumentInfo->device->object())->getDefaultSoundbank()->writeToStream(stream);
//	instrumentInfo->dataBlock = stream;
//	return NULL;
//}

Device* XmLoader::addNoEffect(Player* player/*, MixerChannel* channel*/) {
	return NULL;
}

XmLoader::XmLoader(PlayerDevice* playerDevice) {
	bpm_ = 120;
	channelCount_ = 4;
	ticks_ = 4;
	player_ = playerDevice->player();
	playerDevice_ = playerDevice;
	// check synth-adapter
	SynthAdapter synthAdapter;
	synthAdapter_ = (SynthAdapter*)player_->getAdapter(synthAdapter.getInfo()->id);
	if (synthAdapter_ == NULL) {
		synthAdapter_ = (SynthAdapter*)Player::addAdapter(NEW_(SynthAdapter));
	}
	instrumentInfos_.init(sizeof(InstrumentInfo), 16);

	// check synth-device
	Key ix = 0;
	//synthDevice_ = (SynthDevice*)player->devices().search(SynthDevices::DeviceSynth, ix);
	//if (synthDevice_ == NULL) {
	//	synthDevice_ = (SynthDevice*)player->addDevice(synthAdapter_, SynthDevices::DeviceSynth);
	//}

	patternOrder_.init(sizeof(byte), 256);
	patterns_.init(256);
	//instruments_.init(sizeof(XmInstrument), 64);
	if (xmEffectMap_ == NULL) {
		xmEffectMap_ = NEW_(XmEffectMap);
	}
}

XmLoader::~XmLoader(void) {
	ARRAY_FOREACH(&patterns_, FREE(value));
	DEL_(xmEffectMap_);
}

#pragma region XmReading
void XmLoader::getVolumeEffect(byte code, byte*& ptr, XmNote* xmNote) {
	if (code & 0x04) {
		var vol = *ptr++;
		xmNote->volume = 0xff;
		while (true) {
			if (vol < 0x10) break;
			vol -= 0x10;
			if (vol <= 0x40) {
				xmNote->volume = vol;
				break;
			}
			vol -= 0x30;
			xmNote->effects[0].code = vol >> 4;
			xmNote->effects[0].parameter = vol & 0x0f;
			break;
		}
	}
}
void XmLoader::getEffect(byte code, byte*& ptr, XmNote* xmNote) {
	if (code & 0x08) {
		xmNote->effects[1].code = *ptr++;
	}
	else {
		xmNote->effects[1].code = -1;
	}
	
	if (code & 0x10) {
		xmNote->effects[1].parameter = *ptr++;
	}
	else {
		xmNote->effects[1].parameter = -1;
	}
}
XmNote* XmLoader::readNote(byte*& ptr, XmNote* xmNote) {
	byte code = *ptr++;
	xmNote->hasData = (code != 0x80);
	bool isCompressed = code & 0x80;
	int note = -1;
	if (!isCompressed) {
		note = code - 1;
		code = 0x9e;
	}
	code &= 0x1f;
	xmNote->note = (code & 0x01) ? *ptr++ - 1 : note;
	xmNote->instrument = (code & 0x02) ? *ptr++ - 1 : -1;
	xmNote->volume = -1;
	
	getVolumeEffect(code, ptr, xmNote);
	getEffect(code, ptr, xmNote);
	return xmNote;
}
XmPattern* XmLoader::readPattern(XmFilePattern* ptr) {
	int patternSize = sizeof(XmPattern) + sizeof(XmNote) * ptr->rowCount * channelCount_;
	var pattern = (XmPattern*)MALLOC(byte, patternSize);
	fmw::memset(pattern, 0, patternSize);
	pattern->rowCount = ptr->rowCount;
	pattern->notes = (XmNote*)((byte*)pattern + sizeof(XmPattern));
	byte* packedData = &ptr->packedData;
	int i = 0;
	for (int r = 0; r < ptr->rowCount; r++) {
		for (int ch = 0; ch < channelCount_; ch++) {
			readNote(packedData, &pattern->notes[i++]);
		}
	}
	return pattern;
}
int XmLoader::load(const char* xmPath, Map* soundbanks) {
	int error = 1;
	byte* xmData = NULL;
	var bytesRead = File::read(xmPath, &xmData);
	if (bytesRead > 0 && xmData != NULL) {
		error = 0;
		// read header
		var* header = (XmFileHeader*)xmData;
		bpm_ = header->defaultBPM;
		ticks_ = header->defaultTempo;
		player_->refreshRate(1.0f / (2.5f * ticks_ / (4.0f * bpm_)));
		channelCount_ = header->channelCount;
		// read patterns
		for (int i = 0; i < header->songLength; i++) {
			patternOrder_.push(&header->patternOrder + i);
		}
		// todo: verify file
		// read patterns
		byte* ptr = xmData + offsetof(XmFileHeader, headerSize) + header->headerSize;
		for (int p = 0; p < header->patternCount; p++) {
			var xmPattern = (XmFilePattern*)ptr;
			patterns_.push(readPattern(xmPattern));
			ptr += offsetof(XmFilePattern, packedData) + xmPattern->packedDataSize;
		}
		// read instruments
		// for each instrument
		// - get name
		// - find matching soundbank.program
		// - pick owner device
		int drumId = 0;
		int channelId = 0;
		int drumsChannelId = 0;
		DrumsDevice* drums = NULL;
		char instrumentName[22];
		for (var ii = 0; ii < header->instrumentCount; ii++) {
			var instrument = (XmInstrument*)ptr;
			strncpy(instrumentName, 21, instrument->name);
			char *name = str_rtrim(instrumentName);
			var hasMatch = false;
			for (var si = 0; si < soundbanks->size(); si++) {
				var sb = (Soundbank*)soundbanks->values()->get(si);
				for (var pi = 0; pi < sb->size(); pi++) {
					var prgId = (char*)sb->keys()->get(pi);
					if (strncmp(name, prgId, 22) == 0) {
						var type = *(int*)soundbanks->keys()->get(si);
						InstrumentInfo info;
						fmw::memset(&info, 0, sizeof(InstrumentInfo));
						if (type == SynthDevices::DeviceDrums) {
							if (drums == NULL) {
								info.deviceId = player_->devices().length();
								drums = (DrumsDevice*)player_->addDevice(synthAdapter_, type);
								drumId = 0;
								drumsChannelId = channelId++;
							}
							info.programId = pi + (drumId<<8);
							info.drumId = drumId++;
							info.device = drums;
							//info.channelId = drumsChannelId;
						}
						else {
							info.deviceId = player_->devices().length();
							info.device = (ModuleDevice*)player_->addDevice(synthAdapter_, type);
							//info.device = (ModuleDevice*)synthAdapter_->createDevice(type);
							//info.channelId = channelId++;
							info.programId = pi;
						}
						info.device->module()->setSoundbank(sb);
						info.soundbank = sb;
						
						info.voiceCount = 0;
						fmw::strncpy(&info.name[0], 24, prgId);
						instrumentInfos_.push(&info);
						hasMatch = true;
						break;
					}
				}
				if (hasMatch) break;
			}
			FREE(name);
			if (!hasMatch) {
				// create default device
				InstrumentInfo info;
				fmw::memset(&info, 0, sizeof(InstrumentInfo));
				info.device = (ModuleDevice*)player_->addDevice(synthAdapter_, 0);
				info.programId = 0;
				info.voiceCount = 1;
				var prgName = (char*)info.device->module()->soundbank()->keys()->get(0);
				fmw::strncpy(&info.name[0], 24, prgName);
				instrumentInfos_.push(&info);
			}
			ptr += *(dword*)ptr;
			// skip samples
			for (var si = 0; si < instrument->sampleCount; si++) {
				ptr += *(dword*)ptr + sizeof(XmSample);
			}
		}
		// create data blocks from used soundbanks
		Map usedSoundbanks(MAP_USE_REF, sizeof(int));
		for (var ii = 0; ii < instrumentInfos_.length(); ii++) {
			var info = (InstrumentInfo*)instrumentInfos_.get(ii);
			var dataBlockId = -1;
			if (info->soundbank != NULL) {
				var pDataBlockId = (int*)usedSoundbanks.get(info->soundbank);
				if (pDataBlockId == NULL) {
					// create data block
					Stream soundbank;
					info->soundbank->writeToStream(&soundbank);
					player_->addDataBlock(soundbank.extract(), soundbank.length());
					dataBlockId = player_->dataBlocks().length() - 1;
					// remember soundbank
					usedSoundbanks.put(info->soundbank, &dataBlockId);
					info->device->setSoundbank(info->soundbank);
				}
				else dataBlockId = *pDataBlockId;
				info->device->setProgram(info->programId);
				info->device->datablockId(dataBlockId);
			}
		}
		convertPatterns();

		error = 0;
		FREE(xmData);
	}
	return error;
}
#pragma endregion

#pragma region XmProcessing
void XmLoader::convertPatterns(/*ADD_DEVICE_HANDLER addDevice, ADD_EFFECT_HANDLER addEffect*/) {
	// create master sequence
	var masterSequence = player_->addSequence(NEW_(Sequence, playerDevice_));
	// create channel and sequence for each device
	var hasDrums = false;
	Sequence* drumsSequence = NULL;
	int drumsChannelId = 0;
	for (var i = 0; i < instrumentInfos_.length(); i++) {
		var info = (InstrumentInfo*)instrumentInfos_.get(i);
		info->sequenceId = -1;
		var isDrums = info->device->type() == SynthDevices::DeviceDrums;
		if (isDrums && hasDrums) {
			info->sequence = drumsSequence;
			info->channelId = drumsChannelId;
			continue;
		}
		// create sequence
		info->sequence = NEW_(Sequence, info->device);
		info->sequenceId = player_->sequences().length();
		info->sequence->writeHeader();
		player_->addSequence(info->sequence);

		// create channel
		var ch = NEW_(Channel, !isDrums ? info->name : "Drums");
		info->channelId = player_->channels().length();
		player_->channels().push(ch);
		hasDrums = hasDrums || isDrums;

		if (isDrums) {
			drumsSequence = info->sequence;
			drumsChannelId = info->channelId;
		}
	}

	#pragma region Sequences
	// walk through the pattern order, like playing back the track
	// and create commands for each instrument in its sequence
	// each channel has to remember the last note, instrument, effect and parameter
	XmNote channelInfo[64];
	byte voiceCount[64];
	word lastTick[64];
	word length = 0;
	word tickOffset = 0;
	word currentTick = 0;
	fmw::memset(channelInfo, 0, sizeof(XmNote) * channelCount_);
	fmw::memset(voiceCount, 0, sizeof(byte) * 64);
	fmw::memset(lastTick, 0, sizeof(word) * 64);
	//var data = (InstrumentInfo**)instrumentInfos_.data();
	for (var i = 0; i < patternOrder_.length(); i++) {
		var pi = *(byte*)patternOrder_.get(i);
		var pattern = (XmPattern*)patterns_.get(pi);
		var ni = 0;
		for (var ri = 0; ri < pattern->rowCount; ri++) {
			//	//var currentTick = ticks_ * ri + length;
			for (var ci = 0; ci < channelCount_; ci++) {
				var xmNote = pattern->notes[ni++];
				if (xmNote.hasData) {
					// get instrument => sequence
					var inst = xmNote.instrument;
					if (inst == 0xff) inst = channelInfo[ci].instrument;
					else channelInfo[ci].instrument = inst;
					var info = (InstrumentInfo*)instrumentInfos_.get(inst);
					var isDrums = info->device->type() == SynthDevices::DeviceDrums;
					var seq = info->sequence;
					word delta = currentTick - lastTick[info->channelId];
					// read note
					var note = xmNote.note;
					var volume = xmNote.volume;
					// won't write multiple note offs
					// and won't write note offs for drums
					if (note == 0x60 && (channelInfo[ci].note == 0x60 || isDrums)) continue;
					if (seq->length() > 1) {
						// not first entry in the sequence
						if (delta > 0) {
							// frame has to be closed
							seq->writeEOF();
							// new frame starts with delta
							seq->writeDelta(delta);
							lastTick[info->channelId] = currentTick;
						}
					}
					else {
						// first entry in the sequence
						// write delta anyway
						seq->writeDelta(delta);
						lastTick[info->channelId] = currentTick;
					}
					// write note
					if (note != 0xff) {
						Stream* cmd = NULL;
						if (note == 0x60) {
							// note off
							cmd = info->device->makeCommand(CmdSetNote, channelInfo[ci].note, 0);
							channelInfo[ci].note = 0x60;
							if (voiceCount[info->channelId] > 0) voiceCount[info->channelId]--;
							volume = 0xff;
						}
						else if (xmNote.instrument != 0xff) {
							// set note
							var velocity = volume < 0x3f ? volume << 2 : 255;
							cmd = info->device->makeCommand(CmdSetNote, !isDrums ? note : info->drumId + 1, velocity);
							voiceCount[info->channelId]++;
							if (voiceCount[info->channelId] > info->voiceCount) info->voiceCount = voiceCount[info->channelId];
							channelInfo[ci].note = note;
							volume = 0xff;
						}
						else {
							// setController(note)
							cmd = info->device->makeCommand(CmdSetUint8, SynthCtrlId::osc1Note, xmNote.note);
							seq->writeStream(cmd);
							DEL_(cmd);
							cmd = info->device->makeCommand(CmdSetUint8, SynthCtrlId::osc2Note, xmNote.note);
						}
						seq->writeStream(cmd);
						DEL_(cmd);
					}
					if (volume != 0xff) {
						// setController(amp)
						var volume = xmNote.volume < 0x40 ? xmNote.volume << 2 : 0xff;
						var stream = info->device->makeCommand(CmdSetFloat8, SynthCtrlId::amAdsrAmp, volume);
						seq->writeStream(stream);
						DEL_(stream);
					}

					//if (xmNote.effects[0].code != 0xff) {
					//}
					//if (xmNote.effects[1].code != 0xff) {
					//}
				}
			}
			currentTick += ticks_;
		}
		length += pattern->rowCount * ticks_;
	}
	length = currentTick;
	// write EOS
	instrumentInfos_.apply([](COLLECTION_ARGUMENTS) { ((InstrumentInfo*)value)->sequence->writeEOS(); return value; });
	
	masterSequence->writeHeader()->writeDelta(0);
	for (var ii = 0; ii < instrumentInfos_.length(); ii++) {
		var info = (InstrumentInfo*)instrumentInfos_.get(ii);
		// set voice count
		if (info->device->type() == SynthDevices::DeviceBass || info->device->type() == SynthDevices::DeviceSynth) {
			((SynthBase*)info->device->module())->voiceCount(info->voiceCount);
		}
		// write master sequence
		if (info->sequenceId != -1) {
			var cmd = playerDevice_->makeCommand(CmdAssign, info->channelId, info->sequenceId, info->deviceId, 0);
			masterSequence->writeStream(cmd);
			DEL_(cmd);
		}
	}
	masterSequence->writeEOF()->writeDelta(length)->writeEOS();
	#pragma endregion
}


int XmLoader::printNote(XmNote* xmNote, char* buffer) {
	char* ptr = buffer;
	if (xmNote->hasData) {
		byte v = xmNote->note;
		ptr += (v != 0xff) ? (v != 96) ? sprintf_s(ptr, 16, "%s%d ", NOTES[v % 12], v / 12) : sprintf_s(ptr, 16, "off ") : sprintf_s(ptr, 16, "--- ");
		v = xmNote->instrument;
		ptr += (v != 0xff) ? sprintf_s(ptr, 16, "%02X ", v) : sprintf_s(ptr, 16, "-- ");
		v = xmNote->volume;
		ptr += (v != 0xff) ? sprintf_s(ptr, 16, "%02X ", v) : sprintf_s(ptr, 16, "-- ");
		v = xmNote->effects[1].code;
		ptr += (v != 0xff) ? sprintf_s(ptr, 16, "%s", xmEffectMap_->get(v)) : sprintf_s(ptr, 16, "-");
		v = xmNote->effects[1].parameter;
		ptr += (v != 0xff) ? sprintf_s(ptr, 16, "%02X", v) : sprintf_s(ptr, 16, "--");
	}
	else {
		ptr += sprintf_s(ptr, 16, "... .. .. ...");
	}
	return (int)(ptr - buffer);
}
char* XmLoader::printPattern(int id) {
	int n = 0;
	var buffer = MALLOC(char, 65536);
	var ptr = buffer;
	var pattern = (XmPattern*)patterns_.get(id);
	for (int r = 0; r < pattern->rowCount; r++) {
		ptr += sprintf_s(ptr, 16, "%02x: ", r);
		for (int ch = 0; ch < channelCount_; ch++) {
			XmNote * xmNote = &pattern->notes[n++];
			ptr += printNote(xmNote, ptr);
			if (ch < channelCount_ - 1) {
				ptr += sprintf_s(ptr, 16, "|");
			}
		}
		ptr += sprintf_s(ptr, 16, "\n");
	}
	return buffer;
}

//PArray* XmLoader::convertNote(XMNOTE* xmNote, XMNOTE& state) {
//	PlayerCommands cmd;
//	PArray* commands = NEW_(PArray);
//	if (xmNote->hasData) {
//		// process instrument
//		int instrument = xmNote->instrument;
//		if (instrument != 0xFF && instrument != state.instrument) {
//			state.instrument = instrument;
//			cmd = synthAdapter_->makeCommand(CmdSetProgram, instrument);
//			commands->push(cmd);
//		}
//		// process volume
//		int volume = xmNote->volume;
//		if (volume != 0xFF) {
//			volume -= 0x10;
//			if (volume < 0 || volume > 0x40) {
//volume = state.volume;
//			}
//			state.volume = volume;
//		}
//		// process note
//		int note = xmNote->note;
//		if (note >= 0x00 && note < 0x60) {
//			note += 12;
//			//if (instrument != 0xff) {
//				// note on
//			int v = volume == 0xFF ? 0x40 : volume;
//			cmd = synthAdapter_->createCommand(Synth_Cmd_setControlF, SSN1K_CI_Env1Amp, 1.0);
//			commands->push(cmd);
//			cmd = synthAdapter_->createCommand(Synth_Cmd_setNoteOn, note, volume * 255 >> 6);
//			commands->push(cmd);
//			volume = 0xFF;
//			//} else {
//			//	if (note != state.note) {
//			//		// set note
//			//		cmd = synthAdapter_->createCommand(Synth_Cmd_setControlF, SSN1K_CI_Tune, (double)note);
//			//		commands->push(cmd);
//			//	}
//			//}
//			state.note = note;
//		}
//		if (note == 0x60 && xmNote->note != state.note) {
//			cmd = synthAdapter_->createCommand(Synth_Cmd_setNoteOff, state.note);
//			commands->push(cmd);
//			state.note = xmNote->note;
//		}
//		if (volume != 0xFF) {
//			cmd = synthAdapter_->createCommand(Synth_Cmd_setControlF, SSN1K_CI_Env1Amp, volume / 255.0);
//			commands->push(cmd);
//		}
//	}
//	//v = xmNote.instrument;
//	//(v != 0xff) ? printf("%02X ", v) : printf("-- ");
//	//v = xmNote.volume;
//	//(v != 0xff) ? printf("%02X ", v) : printf("-- ");
//	//v = xmNote.effect;
//	//(v != 0xff) ? printf("%s", xmEffectMap_->get(v)) : printf("--");
//	//v = xmNote.parameter;
//	//(v != 0xff) ? printf("%02X", v) : printf("--");
//	if (commands->length() == 0) {
//		DEL_(commands);
//		commands = NULL;
//	}
//	return commands;
//}
//void XmLoader::convertPattern(XMPATTERN* pattern, PArray* sequences) {
//	PArray* seqMaps = NEW_(PArray);
//	for (int ch = 0; ch < channelCount_; ch++) {
//		Map* seqMap = NEW_(Map, sizeof(int), MAP_USE_REF, Map::hashingItem, Collection::compareInt);
//		seqMaps->push(seqMap);
//		pattern->channels[ch] = NEW_(Array, sizeof(int));
//		for (int ri = 0; ri < pattern->rowCount; ri++) {
//			XMNOTE* xmNote = &pattern->data[ch + ri*channelCount_];
//			if (xmNote->hasData) {
//				PArray* commands = convertNote(xmNote, states_[ch]);
//				if (commands != NULL) {
//					for (UINT32 ci = 0; ci < commands->length(); ci++) {
//						PLAYER_COMMAND cmd = (PLAYER_COMMAND)commands->get(ci);
//						int code = ((SYNTH_CMD_SET_CTRL_*)cmd)->code;
//						if (code == Synth_Cmd_setControl ||
//							code == Synth_Cmd_setControlB ||
//							code == Synth_Cmd_setControlF) {
//							code |= (((SYNTH_CMD_SET_CTRL_*)cmd)->id << 8);
//						}
//						Sequence* sequence = (Sequence*)seqMap->get(&code);
//						if (sequence == NULL) {
//							sequence = NEW_(Sequence);
//							int seqId = sequences->length();
//							sequences->push(sequence);
//							pattern->channels[ch]->push(&seqId);
//							seqMap->put(&code, sequence);
//						}
//						// int delta = getDelta(ri);
//						Frame* frame = sequence->addFrame(ri);
//						frame->addCommand(cmd);
//					}
//					DEL_(commands);
//				}
//			}
//		}
//	}
//	for (UINT32 i = 0; i < seqMaps->length(); i++) {
//		Map* seqMap = (Map*)seqMaps->get(i);
//		for (UINT32 si = 0; si < seqMap->values()->length(); si++) {
//			Sequence* sequence = (Sequence*)seqMap->values()->get(si);
//			// int delta = getDelta(ri);
//			Frame* frame = sequence->addFrame(pattern->rowCount);
//			frame->addCommand(playerAdapter_->createEndCommand());
//		}
//	}
//	ARRAY_FOREACH(seqMaps, DEL_((Map*)value));
//	DEL_(seqMaps);
//}

// LOADER

//PArray* XmLoader::convert() {
//	// create master sequence
//	PArray* sequences = NEW_(PArray);
//	Sequence* masterSequence = NEW_(Sequence);
//	sequences->push(masterSequence);
//	Frame* frame = masterSequence->addFrame(0);
//	frame->addCommand(playerAdapter_->createCommand(Player_Cmd_setTempo, bpm_*ticks_));
//
//	for (int i = 0; i < channelCount_; i++) {
//		// reset channel states
//		memset(states_, -1, sizeof(XMNOTE));
//		states_[i].hasData = 0;
//		// add synths
//		//frame->addCommand(playerAdapter_->createCommand(Player_Cmd_create, 1, i));
//	}	
//
//	// convert patterns
//	for (UINT32 pi = 0; pi < patterns_.length(); pi++) {
//		XMPATTERN* pattern = (XMPATTERN*)patterns_.get(pi);
//		convertPattern(pattern, sequences);
//	}
//
//	// add sequences to master sequence
//	int rowCounter = 0;
//	for (UINT32 i = 0; i < patternOrder_.length(); i++) {
//		byte pi = *(byte*)patternOrder_.get(i);
//		XMPATTERN* pattern = (XMPATTERN*)patterns_.get(pi);
//		for (int ch = 0; ch < channelCount_; ch++) {
//			Array* seqIds = pattern->channels[ch];
//			for (UINT32 si = 0; si < seqIds->length(); si++) {
//				int seqId = *(int*)seqIds->get(si);
//				Frame* frame = masterSequence->frame(i);
//					if (frame == NULL) {
//						frame = masterSequence->addFrame(rowCounter);
//					}
//					frame->addCommand(playerAdapter_->createCommand(Player_Cmd_assign, ch + 1, seqId, Player_Flg_Active));
//			}
//		}
//		rowCounter += pattern->rowCount;
//	}
//
//	masterSequence->addFrame(rowCounter)->addCommand(playerAdapter_->createEndCommand());
//	return sequences;
//}
#pragma endregion