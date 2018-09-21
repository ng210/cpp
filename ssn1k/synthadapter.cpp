#include <stdarg.h>
#include "synthadapter.h"
#include "player.h"
#include "voice.h"
#include "collection/map.h"

NS_SSN1K_BEGIN

static char* NOTES[] = { "C-", "C#", "D-", "D#", "E-", "F-", "F#", "G-", "G#", "A-", "A#", "H-" };

static UINT8  mixerSettings_[] = {
	SSN1K_CI_MixVolume1,	0x28,	SSN1K_CI_MixBalance1,	0x60,
	SSN1K_CI_MixVolume2,	0x28,	SSN1K_CI_MixBalance2,	0x40,
	SSN1K_CI_MixVolume3,	0x28,	SSN1K_CI_MixBalance3,	0x80,
	SSN1K_CI_MixVolume4,	0x60,	SSN1K_CI_MixBalance4,	0xC0,
	SSN1K_CI_MixVolume5,	0x40,	SSN1K_CI_MixBalance5,	0x20,
	SSN1K_CI_MixVolume6,	0x50,	SSN1K_CI_MixBalance6,	0xE0,
	SSN1K_CI_MixVolume7,	0x60,	SSN1K_CI_MixBalance7,	0x20,
	SSN1K_CI_MixVolume8,	0x50,	SSN1K_CI_MixBalance8,	0x80,
	0xFF
};

Map SynthAdapter::targetTypes_(sizeof(UINT32), sizeof(ADAPTER_TARGET_TYPE), Map::hashingItem, Collection::compareInt);

const ADAPTER_INFO SynthAdapter::adapterInfo_ = {
	('S'<<0) + ('Y'<<8) + ('N'<<16) + (0<<24),		// SYN+00
	"SYN00",
	SynthAdapter::initialize,
	SynthAdapter::destroy,
	SynthAdapter::create,
	&SynthAdapter::targetTypes_
};

void SynthAdapter::initialize() {
	ADAPTER_TARGET_TYPE targetType;
	targetType.id = SYNTHADAPTER_TARGET::POLYSYNTH01;
	strncpy(targetType.name, 32, "Poly-Synth01");
	targetTypes_.put(&targetType.id, &targetType);

	targetType.id = SYNTHADAPTER_TARGET::MONOSYNTH01;
	strncpy(targetType.name, 32, "Mono-Synth02");
	targetTypes_.put(&targetType.id, &targetType);

}

void SynthAdapter::destroy() {
	MAP_FOREACH(&SynthAdapter::targetTypes_, FREE(value));
}

IAdapter* SynthAdapter::create(UINT8** data) {
	SynthAdapter* adapter = NEW_(SynthAdapter);
	if (data != NULL) {
		// initialize adapter from the data
		// count of instruments
		// array of instrument config
		// count of synths
		// array of voice count
		UINT8* ptr = *data;

		// create sound bank
		Ctrl* bank = NULL;
		ptr += createBank((SYNTH_BANK*)ptr, bank);

		adapter->synthCount_ = *ptr++;
		// create synth, specify voice count
		for (int si = 0; si < adapter->synthCount_; si++) {
			Synth* synth = adapter->synths_[si] = NEW_(Synth, *ptr++);
			adapter->mixer_->addInput(synth);
			synth->bank(bank);
		}
		*data = ptr;
	}
	return adapter;
}

const ADAPTER_INFO& SynthAdapter::adapterInfo() {
	return SynthAdapter::adapterInfo_;
}

SynthAdapter::SynthAdapter() {
	synthCount_ = 0;
	for (int i = 0; i < 16; i++) {
		synths_[i] = NULL;
	}
	mixer_ = NEW_(Mixer);
	mixer_->setControls(mixerSettings_);
}
SynthAdapter::~SynthAdapter() {
	for (int i = 0; i < 16; i++) {
		DEL_(synths_[i]);
	}
	DEL_(mixer_);
}

const ADAPTER_INFO* SynthAdapter::getInfo() {
	return &SynthAdapter::adapterInfo_;
}

int SynthAdapter::processCommand(void* object, PLAYER_COMMAND command) {
	Synth* synth = (Synth*)object;
	unsigned char* args = &command[1];
	int ctrlId;
#ifdef _DEBUG
	//char *str = logCommand(command);
	//printf("%s\n", str);
	//FREE(str);
#endif
	switch (command[0]) {
		case Synth_Cmd_setNoteOn:		// note, duration, velocity
			SYNTH_CMD_SET_NOTE_ON* cmdNoteOn;
			cmdNoteOn = (SYNTH_CMD_SET_NOTE_ON*)command;
			synth->noteOn(cmdNoteOn->note, cmdNoteOn->velocity / 255.0f);
			break;
		case Synth_Cmd_setNoteOff:
			synth->noteOff(((SYNTH_CMD_SET_NOTE_OFF*)command)->note);
			break;
		case Synth_Cmd_setControl:	// id, value:UINT8->float
			SYNTH_CMD_SET_CTRL* cmdCtrl;
			cmdCtrl = (SYNTH_CMD_SET_CTRL*)command;
			ctrlId = cmdCtrl->id;
			synth->getControl(ctrlId)->f = (float)(cmdCtrl->value / 255.0f);
			break;
		case Synth_Cmd_setControlB:	// id, value:UINT8
			SYNTH_CMD_SET_CTRL* cmdCtrlB;
			cmdCtrlB = (SYNTH_CMD_SET_CTRL*)command;
			ctrlId = cmdCtrlB->id;
			synth->getControl(ctrlId)->i = cmdCtrlB->value;
			break;
		case Synth_Cmd_setControlF:	// id, value:float
			SYNTH_CMD_SET_CTRLF* cmdCtrlF;
			cmdCtrlF = (SYNTH_CMD_SET_CTRLF*)command;
			Ctrl* ctrl;
			ctrlId = cmdCtrlF->id;
			switch (ctrlId) {
				case SSN1K_CI_Tune:
					ctrl = &synth->activeVoice()->note();
					break;
				default:
					ctrl = synth->getControl(ctrlId);
					break;
			}
			ctrl->f = (float)cmdCtrlF->value;
			break;
		case Synth_Cmd_prgChange:	// prg id
			int prgId = ((SYNTH_CMD_PRG_CHNG*)command)->id;
			synth->changeProgram(prgId);
			break;
	}
	return 1;
}
void SynthAdapter::setTempo(void *object, float ticksPerSecond) {
	((Synth*)object)->ticksPerSample(ticksPerSecond*SSN1K::getSampleRateR());
}
size_t SynthAdapter::fill(void* buffer, size_t start, size_t end) {
	mixer_->run(buffer, start, end);
	return end - start;
}

// Misc. methods
PLAYER_COMMAND SynthAdapter::createCommand(int code, ...) {
	PLAYER_COMMAND cmd = NULL;
	va_list args;
	va_start(args, code);
	switch (code) {
	case Synth_Cmd_setNoteOn:
		SYNTH_CMD_SET_NOTE_ON* cmdNoteOn;
		cmdNoteOn = MALLOC(SYNTH_CMD_SET_NOTE_ON, 1);
		cmdNoteOn->note = (UINT8)va_arg(args, int);			// note
		cmdNoteOn->velocity = (UINT8)va_arg(args, int);		// velocity
		cmd = (PLAYER_COMMAND)cmdNoteOn;
		break;
	case Synth_Cmd_setNoteOff:
		SYNTH_CMD_SET_NOTE_OFF* cmdNoteOff;
		cmdNoteOff = MALLOC(SYNTH_CMD_SET_NOTE_OFF, 1);
		cmdNoteOff->note = (UINT8)va_arg(args, int);			// note
		cmd = (PLAYER_COMMAND)cmdNoteOff;
		break;
	case Synth_Cmd_setControl:
	case Synth_Cmd_setControlB:
		SYNTH_CMD_SET_CTRL* cmdCtrl;
		cmdCtrl = MALLOC(SYNTH_CMD_SET_CTRL, 1);
		cmdCtrl->id = (UINT8)va_arg(args, int);				// id
		cmdCtrl->value = (UINT8)va_arg(args, int);			// value
		cmd = (PLAYER_COMMAND)cmdCtrl;
		break;
	case Synth_Cmd_setControlF:
		SYNTH_CMD_SET_CTRLF* cmdCtrlF;
		cmdCtrlF = MALLOC(SYNTH_CMD_SET_CTRLF, 1);
		cmdCtrlF->id = (UINT8)va_arg(args, int);				// id
		cmdCtrlF->value = (float)va_arg(args, double);		// value
		cmd = (PLAYER_COMMAND)cmdCtrlF;
		break;
	case Synth_Cmd_prgChange:
		SYNTH_CMD_PRG_CHNG* cmdPrg;
		cmdPrg = MALLOC(SYNTH_CMD_PRG_CHNG, 1);
		cmdPrg->id = (UINT8)va_arg(args, int);				// id
		cmd = (PLAYER_COMMAND)cmdPrg;
		break;
	}
	((ABSTRACT_ADAPTER_COMMAND_*)cmd)->code = code;
	va_end(args);
	return cmd;
}
char* SynthAdapter::logCommand(PLAYER_COMMAND command) {
	char* str = MALLOC(char, 32);
	SYNTH_CMD_SET_NOTE_ON* cmdNoteOn;
	SYNTH_CMD_SET_NOTE_OFF* cmdNoteOff;
	SYNTH_CMD_SET_CTRL* cmdCtrl;
	SYNTH_CMD_SET_CTRLF* cmdCtrlF;
	SYNTH_CMD_PRG_CHNG* cmdPrg;
	switch (command[0]) {
	case Synth_Cmd_setNoteOn:		// noteOn(C-4, 80)
		cmdNoteOn = (SYNTH_CMD_SET_NOTE_ON*)command;
		sprintf_s(str, 32, "noteOn(%s%d,%.3d)", NOTES[cmdNoteOn->note % 12], cmdNoteOn->note / 12, cmdNoteOn->velocity);
		break;
	case Synth_Cmd_setNoteOff:		// noteOff(C-4,)
		cmdNoteOff = (SYNTH_CMD_SET_NOTE_OFF*)command;
		sprintf_s(str, 32, "noteOff(%s%d)", NOTES[cmdNoteOff->note % 12], cmdNoteOff->note / 12);
		break;
	case Synth_Cmd_setControl:	// ctrl(123, 85)
	case Synth_Cmd_setControlB:
		cmdCtrl = (SYNTH_CMD_SET_CTRL*)command;
		sprintf_s(str, 32, "ctrl(%.3d, %.3d)", cmdCtrl->id, cmdCtrl->value);
		break;
	case Synth_Cmd_setControlF:	// ctrl(123, 0.5)
		cmdCtrlF = (SYNTH_CMD_SET_CTRLF*)command;
		sprintf_s(str, 32, "ctrl(%.3d, %.2f)", cmdCtrlF->id, cmdCtrlF->value);
		break;
	case Synth_Cmd_prgChange:	// prg(01)
		cmdPrg = (SYNTH_CMD_PRG_CHNG*)command;
		sprintf_s(str, 32, "prg(%02X)", cmdPrg->id);
		break;

	default:
		FREE(str);
		str = IAdapter::logCommand(command);
		break;
	}
	return str;
}
int SynthAdapter::dumpCommand(PLAYER_COMMAND command, Buffer* buffer) {
	SYNTH_CMD_SET_NOTE_ON* cmdNoteOn;
	SYNTH_CMD_SET_NOTE_OFF* cmdNoteOff;
	SYNTH_CMD_SET_CTRL* cmdCtrl;
	UINT8 tmp[64];
	int length = 0;
	tmp[length++] = command[0];
	switch (command[0]) {
	case Synth_Cmd_setNoteOn:
		cmdNoteOn = (SYNTH_CMD_SET_NOTE_ON*)command;
		tmp[length++] = cmdNoteOn->note;
		tmp[length++] = cmdNoteOn->velocity;
		break;
	case Synth_Cmd_setNoteOff:
		cmdNoteOff = (SYNTH_CMD_SET_NOTE_OFF*)command;
		tmp[length++] = cmdNoteOff->note;
		break;
	case Synth_Cmd_setControl:
		cmdCtrl = (SYNTH_CMD_SET_CTRL*)command;
		tmp[length++] = cmdCtrl->id;
		tmp[length++] = cmdCtrl->value;
		break;
	default:
		break;
	}
	if (buffer != NULL) {
		buffer->append(tmp, length);
	}
	return length;
}
int SynthAdapter::matchCommand(int filter, PLAYER_COMMAND cmd) {
	int ret = 0;
	switch (filter & 0xFF) {
	case Synth_Cmd_setNoteOn:
	case Synth_Cmd_setNoteOff:
		ret = 1;
		break;
	case Synth_Cmd_setControl:
	case Synth_Cmd_setControlB:
	case Synth_Cmd_setControlF:
		int ctrlId;
		ctrlId = cmd[1];
		ret = cmd[1] == ((UINT8*)&filter)[1];
		break;
	default:
		break;
	}
	return ret;
}
int SynthAdapter::getCommandParameters(PLAYER_COMMAND cmd, double* parameters) {
	int count = -1;
	SYNTH_CMD_SET_NOTE_ON* cmdNoteOn;
	//SYNTH_CMD_SET_NOTE_OFF* cmdNoteOff;
	SYNTH_CMD_SET_CTRL* cmdCtrl;
	switch (cmd[0]) {
	case Synth_Cmd_setNoteOn:
		cmdNoteOn = (SYNTH_CMD_SET_NOTE_ON*)cmd;
		parameters[0] = cmdNoteOn->note;
		parameters[1] = cmdNoteOn->velocity;
		break;
	case Synth_Cmd_setControl:
		cmdCtrl = (SYNTH_CMD_SET_CTRL*)cmd;
		parameters[0] = cmdCtrl->id;
		parameters[1] = cmdCtrl->value;
		break;
	case Synth_Cmd_setControlB:
		cmdCtrl = (SYNTH_CMD_SET_CTRL*)cmd;
		parameters[0] = cmdCtrl->id;
		parameters[1] = cmdCtrl->value;
		break;
	case Synth_Cmd_setControlF:
		cmdCtrl = (SYNTH_CMD_SET_CTRL*)cmd;
		parameters[0] = cmdCtrl->id;
		parameters[1] = cmdCtrl->value;
		break;
	default:
		break;
	}
	return count;
}

// Editor
Target* SynthAdapter::createTarget(int id, UINT8* data) {
	void* obj = NULL;
	switch (id) {
		case SYNTHADAPTER_TARGET::POLYSYNTH01:
			obj = NEW_(Synth, data != NULL ? data[0] : 16);
			// add sound bank
			break;
		case SYNTHADAPTER_TARGET::MONOSYNTH01:
			obj = NEW_(Synth, 1);
			// add sound bank
			break;

	}
	Target* target = (obj != NULL) ? NEW_(Target, obj, this) : NULL;
	return target;
}

int SynthAdapter::createBank(SYNTH_BANK* bankConfig, out Ctrl*& bank) {
	int count = SSN1K_CI_COUNT * bankConfig->instrumentCount;
	bank = MALLOC(Ctrl, count);
	memset(bank, 0, sizeof(Ctrl)*count);
	UINT8* ptr = &bankConfig->instrumentData;
	Ctrl* bankPtr = bank;
	for (int i = 0; i < bankConfig->instrumentCount; i++) {
		// setup instrument from byte data
		ptr = Synth::setControls(bankPtr, ptr);
		bankPtr += SSN1K_CI_COUNT;
	}
	return (int)(ptr - (UINT8*)bankConfig);
}
NS_SSN1K_END