#include <stdarg.h>
#include "synthadapter.h"
#include "player.h"
#include "voice.h"

NS_SSN1K_BEGIN

static char* NOTES[] = { "C-", "C#", "D-", "D#", "E-", "F-", "F#", "G-", "G#", "A-", "A#", "H-" };


SynthAdapter::SynthAdapter(Player* player) {
	id_ = SYNTH_ADAPTER_ID;
	player_ = player;
}

SynthAdapter::~SynthAdapter() {
}

int SynthAdapter::prepareObject(void* synth) {
	return 0;
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
		case Synth_Cmd_setControl:	// id, value:UINT8
			SYNTH_CMD_SET_CTRL* cmdCtrl;
			cmdCtrl = (SYNTH_CMD_SET_CTRL*)command;
			ctrlId = cmdCtrl->id;
			synth->getControl(ctrlId)->set(cmdCtrl->value / 255.0f);
			break;
		case Synth_Cmd_setControlB:	// id, value:UINT8->float
			SYNTH_CMD_SET_CTRL* cmdCtrlB;
			cmdCtrlB = (SYNTH_CMD_SET_CTRL*)command;
			ctrlId = cmdCtrlB->id;
			synth->getControl(ctrlId)->set(cmdCtrlB->value);
			break;
		case Synth_Cmd_setControlF:	// id, value:float
			SYNTH_CMD_SET_CTRLF* cmdCtrlF;
			cmdCtrlF = (SYNTH_CMD_SET_CTRLF*)command;
			ctrlId = cmdCtrlF->id;
			switch (ctrlId) {
				case SSN1K_CI_Tune:
					synth->activeVoice()->note().set(36.0f);	// cmdCtrlF->value);
					break;
				default:
					synth->getControl(ctrlId)->set(cmdCtrlF->value);
					break;
			}
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

#ifdef _DEBUG
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
		str = AbstractAdapter::logCommand(command);
		break;
	}
	return str;
}
#endif

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
NS_SSN1K_END