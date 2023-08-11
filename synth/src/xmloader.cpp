/* Load and convert XM patterns
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

XmEffect::XmEffect() {
	code_ = 0;
	parameter_ = 0;
	//sign_ = '\0';
	//handler_ = NULL;
}

XmEffect::XmEffect(int code, int parameter) {
	code_ = code;
	parameter_ = parameter;
}

Map* xmEffectMap_;

void createXmEffectMap() {
	xmEffectMap_ = NEW_(Map, sizeof(XmFxType), sizeof(char), Map::hashingInt, Collection::compareInt);
	xmEffectMap_->hasRefKey(false);
	char sign;
	sign = '0'; xmEffectMap_->put(0x00, &sign);	// 0 Appregio
	sign = '1'; xmEffectMap_->put(0x01, &sign);	// 1 Porta up
	sign = '2'; xmEffectMap_->put(0x02, &sign);	// 2 Porta down
	sign = '3'; xmEffectMap_->put(0x03, &sign);	// 3 Tone porta
	sign = '4'; xmEffectMap_->put(0x04, &sign);	// 4 Vibrato
	sign = '5'; xmEffectMap_->put(0x05, &sign);	// 5 Tone porta + Volume slide
	sign = '6'; xmEffectMap_->put(0x06, &sign);	// 6 Vibrato + Volume slide
	sign = '7'; xmEffectMap_->put(0x07, &sign);	// 7 Tremolo
	sign = '8'; xmEffectMap_->put(0x08, &sign);	// 8 Set panning
	sign = '9'; xmEffectMap_->put(0x09, &sign);	// 9 Sample offset
	sign = 'A'; xmEffectMap_->put(0x0A, &sign);	// A Volume slide
	sign = 'B'; xmEffectMap_->put(0x0B, &sign);	// B Position jump
	sign = 'C'; xmEffectMap_->put(0x0C, &sign);	// C Set volume
	sign = 'D'; xmEffectMap_->put(0x0D, &sign);	// D Pattern break
	sign = 'E'; xmEffectMap_->put(0xE1, &sign);	// E1 Fine porta up
	sign = 'E'; xmEffectMap_->put(0xE2, &sign);	// E2 Fine porta down
	sign = 'E'; xmEffectMap_->put(0xE3, &sign);	// E3 Set gliss control
	sign = 'E'; xmEffectMap_->put(0xE4, &sign);	// E4 Set vibrato control
	sign = 'E'; xmEffectMap_->put(0xE5, &sign);	// E5 Set finetune
	sign = 'E'; xmEffectMap_->put(0xE6, &sign);	// E6 Set loop begin / loop
	sign = 'E'; xmEffectMap_->put(0xE7, &sign);	// E7 Set tremolo control
	sign = 'E'; xmEffectMap_->put(0xE9, &sign);	// E9 Retrig note
	sign = 'E'; xmEffectMap_->put(0xEA, &sign);	// EA Fine volume slide up
	sign = 'E'; xmEffectMap_->put(0xEB, &sign);	// EB Fine volume slide down
	sign = 'E'; xmEffectMap_->put(0xEC, &sign);	// EC Note cut
	sign = 'E'; xmEffectMap_->put(0xED, &sign);	// ED Note delay
	sign = 'E'; xmEffectMap_->put(0xEE, &sign);	// EE Pattern delay
	sign = 'F'; xmEffectMap_->put(0x0F, &sign);	// F Set tempo / BPM
	sign = 'G'; xmEffectMap_->put(0x10, &sign);	// G  (010h) Set global volume
	sign = 'H'; xmEffectMap_->put(0x11, &sign);	// H  (011h) Global volume slide
	sign = 'I'; xmEffectMap_->put(0x12, &sign);	// I  (012h) Unused
	sign = 'J'; xmEffectMap_->put(0x13, &sign);	// J  (013h) Unused
	sign = 'K'; xmEffectMap_->put(0x14, &sign);	// K  (014h) Unused
	sign = 'L'; xmEffectMap_->put(0x15, &sign);	// L  (015h) Set envelope position
	sign = 'M'; xmEffectMap_->put(0x16, &sign);	// M  (016h) Unused
	sign = 'N'; xmEffectMap_->put(0x17, &sign);	// N  (017h) Unused
	sign = 'O'; xmEffectMap_->put(0x18, &sign);	// O  (018h) Unused
	sign = 'P'; xmEffectMap_->put(0x19, &sign);	// P  (019h) Panning slide
	sign = 'Q'; xmEffectMap_->put(0x1A, &sign);	// Q  (01ah) Unused
	sign = 'R'; xmEffectMap_->put(0x1B, &sign);	// R  (01bh) Multi retrig note
	sign = 'S'; xmEffectMap_->put(0x1C, &sign);	// S  (01ch) Unused
	sign = 'T'; xmEffectMap_->put(0x1D, &sign);	// T  (01dh) Tremor
	sign = 'U'; xmEffectMap_->put(0x1E, &sign);	// U  (01eh) Unused
	sign = 'V'; xmEffectMap_->put(0x1F, &sign);	// V  (01fh) Unused
	sign = 'W'; xmEffectMap_->put(0x20, &sign);	// W  (020h) Unused
	sign = 'X'; xmEffectMap_->put(0x21, &sign);	// X1 (021h) Extra fine porta up
	sign = 'X'; xmEffectMap_->put(0x22, &sign);	// X2 (021h) Extra fine porta down
}

XmLoader::XmLoader(PlayerDevice* playerDevice) {
	createXmEffectMap();
	bpm_ = 120;
	channelCount_ = 4;
	ticks_ = 4;
	playerDevice_ = playerDevice;
	player_ = playerDevice_->player();

	// check synth-adapter
	SynthAdapter synthAdapter;
	synthAdapter_ = (SynthAdapter*)player_->getAdapter(synthAdapter.getInfo()->id);
	if (synthAdapter_ == NULL) {
		synthAdapter_ = (SynthAdapter*)Player::addAdapter(NEW_(SynthAdapter));
	}

	instrumentInfos_.init(sizeof(InstrumentInfo), 16);
	patternOrder_.init(sizeof(byte), 256);
	patterns_.init(256);
}

XmLoader::~XmLoader(void) {
	ARRAY_FOREACH(&patterns_, FREE(value));
	DEL_(xmEffectMap_);
}

#pragma region XmReading
void XmLoader::processVolumeEffect(byte code, byte*& ptr, XmNote* xmNote) {
	// 10..50 V Set volume
	// 60..6F D Volume slide down
	// 70..7F C Volume slide up
	// 80..8F B Fine volume down
	// 90..9F A Fine volume up
	// A0..AF U Vibrato speed
	// B0..BF H Vibrato deph
	// C0..CF P Set panning
	// D0..DF L Pan slide left
	// E0..EF R Pan slide right
	// F0..FF G Tone portamento
	xmNote->volume = xmNote->note != 96 && xmNote->instrument != -1 ? 0xff : -1;
	xmNote->effects[0].code(XmFxTypeNone);
	xmNote->effects[0].parameter(-1);
	if (code & XmNoteBitsVolume) {
		var value = *ptr++;
		var code = value >> 4;
		if (value > 0x10) {
			if (value <= 0x50) {
				xmNote->volume = (value - 0x11) << 4;
			}
			else {
				switch (code) {
				case 0x06:
					xmNote->effects[0].code(XmFxTypeVolumeSlide); break;
				case 0x07:
					xmNote->effects[0].code(XmFxTypeVolumeSlide); break;
				case 0x08:
					xmNote->effects[0].code(XmFxTypeVolumeSlide); break;
				case 0x09:
					xmNote->effects[0].code(XmFxTypeVolumeSlide); break;
				case 0x0a:
					xmNote->effects[0].code(XmFxTypeVibrato); break;
				case 0x0b:
					xmNote->effects[0].code(XmFxTypeVibrato); break;
				case 0x0c:
					xmNote->effects[0].code(XmFxTypeSetPanning); break;
				case 0x0d:
					xmNote->effects[0].code(XmFxTypePanningSlide); break;
				case 0x0e:
					xmNote->effects[0].code(XmFxTypePanningSlide); break;
				case 0x0f:
					xmNote->effects[0].code(XmFxTypeTonePorta); break;
				}

				xmNote->effects[0].parameter(value & 0x0f);
			}
		}
	}
}

void XmLoader::processEffect(byte code, byte*& ptr, XmNote* xmNote) {
	xmNote->effects[1].code(code & XmNoteBitsFxType ? *ptr++ : XmFxTypeNone);
	xmNote->effects[1].parameter(code & XmNoteBitsFxParameter ? *ptr++ : -1);
}

XmNote* XmLoader::readNote(byte*& ptr, XmNote* xmNote) {
	byte code = *ptr++;
	xmNote->hasData = (code != XmNoteBitsCompressed);
	bool isCompressed = code & XmNoteBitsCompressed;
	int note = -1;
	if (!isCompressed) {
		note = code - 1;
		code = XmNoteBitsAll - XmNoteBitsNote;
	}
	code &= XmNoteBitsAll;
	xmNote->note = (code & XmNoteBitsNote) ? *ptr++ - 1 : note;
	xmNote->instrument = (code & XmNoteBitsInstrument) ? *ptr++ - 1 : -1;
	processVolumeEffect(code, ptr, xmNote);
	processEffect(code, ptr, xmNote);
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
			fmw::strncpy(instrumentName, 21, instrument->name);
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
void XmLoader::convertPatterns() {
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
					int volume = xmNote.volume;
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
							volume = -1;
						}
						else if (xmNote.instrument != 0xff) {
							// set note
							var velocity = volume;
							cmd = info->device->makeCommand(CmdSetNote, !isDrums ? note : info->drumId + 1, velocity);
							voiceCount[info->channelId]++;
							if (voiceCount[info->channelId] > info->voiceCount) info->voiceCount = voiceCount[info->channelId];
							channelInfo[ci].note = note;
							volume = -1;
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
					if (volume != -1) {
						// setController(amp)
						var stream = info->device->makeCommand(CmdSetFloat8, SynthCtrlId::amAdsrAmp, xmNote.volume);
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
		int v = xmNote->note;
		ptr += (v != 0xff) ? (v != 96) ? sprintf_s(ptr, 24, "%s%d ", NOTES[v % 12], v / 12) : sprintf_s(ptr, 24, "off ") : sprintf_s(ptr, 24, "--- ");
		v = xmNote->instrument;
		ptr += (v != 0xff) ? sprintf_s(ptr, 24, "%02X ", v) : sprintf_s(ptr, 24, "-- ");
		v = xmNote->volume;
		ptr += (v != -1) ? sprintf_s(ptr, 24, "%02X ", v) : sprintf_s(ptr, 24, "-- ");
		v = xmNote->effects[1].code();
		ptr += (v != XmFxTypeNone) ? sprintf_s(ptr, 24, "%c", *(char*)xmEffectMap_->get(v)) : sprintf_s(ptr, 24, "-");
		v = xmNote->effects[1].parameter();
		ptr += (v != -1) ? sprintf_s(ptr, 24, "%02X", v) : sprintf_s(ptr, 24, "--");
	}
	else {
		ptr += sprintf_s(ptr, 24, "... .. .. ...");
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
#pragma endregion