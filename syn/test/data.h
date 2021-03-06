#ifndef __DATA_H
#define __DATA_H

#include "ssn1k/synth.h"

NS_SSN1K_USE

#define dd2db(dd) (UINT8)(dd & 0xff), (UINT8)((dd>>8) & 0xff), (UINT8)((dd>>16) & 0xff), (UINT8)((dd>>24) & 0xff)

/// <summary>bank configuration</summary>
/// <remarks>
/// - number of instruments
///  - instrument description as binary data
/// </remarks>
unsigned char bankConfig[] = {
	0x08,  // instrument count

	/********************************
	* chords
	********************************/
	SSN1K_CI_SynthMix, SSN1K_MM_OVR,
	SSN1K_CI_SynthAmp, 0xFF,
	SSN1K_CI_SynthBal, 0x80,

	SSN1K_CI_Env1Mix, dd2db(0x3F600000),
	SSN1K_CI_Env1Amp, dd2db(0x3F800000),
	//SSN1K_CI_Env1DC, 0.0f,
	SSN1K_CI_Env1Atk, 0x10,
	SSN1K_CI_Env1Dec, 0x20,
	SSN1K_CI_Env1Sus, 0x80,
	SSN1K_CI_Env1Rel, 0x80,

	//SSN1K_CI_Env2Mix, 0.0f,
	//SSN1K_CI_Env2Amp, 0.1f,
	//SSN1K_CI_Env2DC, 0.0f,
	//SSN1K_CI_Env2Atk, 0.1f,
	//SSN1K_CI_Env2Dec, 0.2f,
	//SSN1K_CI_Env2Sus, 0.2f,
	//SSN1K_CI_Env2Rel, 0.4f,

	SSN1K_CI_Env3Mix, dd2db(0x3F400000),
	SSN1K_CI_Env3Amp, dd2db(0x43FA0000),
	SSN1K_CI_Env3DC, dd2db(0x43480000),
	//SSN1K_CI_Env3Amp, dd2db(0x447A0000),
	//SSN1K_CI_Env3DC, dd2db(0x41C80000),
	SSN1K_CI_Env3Atk, 0x0C,
	SSN1K_CI_Env3Dec, 0x18,
	SSN1K_CI_Env3Sus, 0x80,
	SSN1K_CI_Env3Rel, 0x60,

	//SSN1K_CI_Env4Mix, SSN1K_MM_BPS,
	//SSN1K_CI_Env4Amp, 0.0f,
	//SSN1K_CI_Env4DC, 0.0f,
	//SSN1K_CI_Env4Atk, 0.0f,
	//SSN1K_CI_Env4Dec, 0.0f,
	//SSN1K_CI_Env4Sus, 0.0f,
	//SSN1K_CI_Env4Rel, 0.0f,
	//SSN1K_CI_Env4Gate, 0.0f,

	//SSN1K_CI_Osc1Mix, CtrlValue(SSN1K_MM_OVR),
	SSN1K_CI_Osc1Amp, 0xA0,
	//SSN1K_CI_Osc1DC, 0.0f,
	//SSN1K_CI_Osc1Note, 0.0f,
	//SSN1K_CI_Osc1Tune, 0.0f,
	//SSN1K_CI_Osc1Fre, 0.0f,
	SSN1K_CI_Osc1Psw, 0x80,
	SSN1K_CI_Osc1Wav, SSN1K_WF_PSAW,

	SSN1K_CI_Osc2Mix, SSN1K_MM_ADD,
	SSN1K_CI_Osc2Amp, 0x80,
	//SSN1K_CI_Osc2DC, 0.0f,
	//SSN1K_CI_Osc2Note, 0.0f,
	SSN1K_CI_Osc2Tune, dd2db(0x4140A3D7),
	//SSN1K_CI_Osc2Fre, 0.0f,
	SSN1K_CI_Osc2Psw, 0x80,
	SSN1K_CI_Osc2Wav, SSN1K_WF_PSAW,

	//SSN1K_CI_Lfo1Mix, SSN1K_MM_MUL,
	SSN1K_CI_Lfo1Amp, dd2db(0x3E833333),
	//SSN1K_CI_Lfo1DC, 0.0f,
	//SSN1K_CI_Lfo1Note, 0.0f,
	//SSN1K_CI_Lfo1Tune,dd2db(0xBF800000),
	SSN1K_CI_Lfo1Fre, dd2db(0x40C33333),
	//SSN1K_CI_Lfo1Psw, 0.8f,
	SSN1K_CI_Lfo1Wav, SSN1K_WF_SIN,

	//SSN1K_CI_Lfo2Mix,SSN1K_MM_MUL,
	SSN1K_CI_Lfo2Amp, dd2db(0x3ACCCCCD),
	//SSN1K_CI_Lfo2DC, 0.0f,
	//SSN1K_CI_Lfo2Note, 0.0f,
	//SSN1K_CI_Lfo1Tune,dd2db(0xBF800000),
	SSN1K_CI_Lfo2Fre, dd2db(0x404CCCCD),
	//SSN1K_CI_Lfo2Psw, 0.0f,
	SSN1K_CI_Lfo2Wav, SSN1K_WF_SIN,

	//SSN1K_CI_FltMix, 0.0f,
	//SSN1K_CI_FltAmp, 0.0f,
	//SSN1K_CI_FltDC, 0.0f,
	SSN1K_CI_FltRes, 0x80,
	SSN1K_CI_FltMode, SSN1K_FM_LP,
	0xFF,

	/********************************
	 * solo
	 ********************************/
	SSN1K_CI_SynthMix, SSN1K_MM_OVR,
	SSN1K_CI_SynthAmp, 0xFF,
	SSN1K_CI_SynthBal, 0x80,
	//SSN1K_CI_Tune, dd2db(0xC1400000),

	SSN1K_CI_Env1Mix, dd2db(0x3F600000),
	SSN1K_CI_Env1Amp, dd2db(0x3F800000),
	//SSN1K_CI_Env1DC, 0.0f,
	SSN1K_CI_Env1Atk, 0x11,
	SSN1K_CI_Env1Dec, 0x10,
	SSN1K_CI_Env1Sus, 0xA0,
	SSN1K_CI_Env1Rel, 0x60,

	//SSN1K_CI_Env2Mix, 0.0f,
	//SSN1K_CI_Env2Amp, 0.1f,
	//SSN1K_CI_Env2DC, 0.0f,
	//SSN1K_CI_Env2Atk, 0.1f,
	//SSN1K_CI_Env2Dec, 0.2f,
	//SSN1K_CI_Env2Sus, 0.2f,
	//SSN1K_CI_Env2Rel, 0.4f,

	SSN1K_CI_Env3Mix, dd2db(0x3F400000),
	SSN1K_CI_Env3Amp, dd2db(0x44FA0000),
	SSN1K_CI_Env3DC, dd2db(0x42C80000),
	//SSN1K_CI_Env3Amp, dd2db(0x447A0000),
	//SSN1K_CI_Env3DC, dd2db(0x41C80000),
	//SSN1K_CI_Env3Amp, dd2db(0x46FA0000),
	//SSN1K_CI_Env3DC, dd2db(0x437A0000),
	SSN1K_CI_Env3Atk, 0x08,
	SSN1K_CI_Env3Dec, 0x18,
	SSN1K_CI_Env3Sus, 0xA0,
	SSN1K_CI_Env3Rel, 0x40,

	//SSN1K_CI_Env4Mix, SSN1K_MM_BPS,
	//SSN1K_CI_Env4Amp, 0.0f,
	//SSN1K_CI_Env4DC, 0.0f,
	//SSN1K_CI_Env4Atk, 0.0f,
	//SSN1K_CI_Env4Dec, 0.0f,
	//SSN1K_CI_Env4Sus, 0.0f,
	//SSN1K_CI_Env4Rel, 0.0f,
	//SSN1K_CI_Env4Gate, 0.0f,

	//SSN1K_CI_Osc1Mix, CtrlValue(SSN1K_MM_OVR),
	SSN1K_CI_Osc1Amp, 0xC0,
	//SSN1K_CI_Osc1DC, 0.0f,
	//SSN1K_CI_Osc1Note, 0.0f,
	//SSN1K_CI_Osc1Tune, dd2db(0xC1400000),
	//SSN1K_CI_Osc1Fre, 0.0f,
	SSN1K_CI_Osc1Psw, 0x80,
	SSN1K_CI_Osc1Wav, SSN1K_WF_TRI,

	SSN1K_CI_Osc2Mix, SSN1K_MM_ADD,
	SSN1K_CI_Osc2Amp, 0x80,
	//SSN1K_CI_Osc2DC, 0.0f,
	//SSN1K_CI_Osc2Note, 0.0f,
	SSN1K_CI_Osc2Tune, dd2db(0x3DCCCCCD),
	//SSN1K_CI_Osc2Fre, 0.0f,
	SSN1K_CI_Osc2Psw, 0x80,
	SSN1K_CI_Osc2Wav, SSN1K_WF_TRI,

	//SSN1K_CI_Lfo1Mix, SSN1K_MM_MUL,
	SSN1K_CI_Lfo1Amp, dd2db(0x3DCCCCCD),
	//SSN1K_CI_Lfo1DC, 0.0f,
	//SSN1K_CI_Lfo1Note, 0.0f,
	//SSN1K_CI_Lfo1Tune, 0.0f,
	SSN1K_CI_Lfo1Fre, dd2db(0x4059999A),
	//SSN1K_CI_Lfo1Psw, 0.8f,
	SSN1K_CI_Lfo1Wav, SSN1K_WF_SIN,

	//SSN1K_CI_Lfo2Mix,SSN1K_MM_MUL,
	SSN1K_CI_Lfo2Amp, dd2db(0x3C000000),
	//SSN1K_CI_Lfo2DC, 0.0f,
	//SSN1K_CI_Lfo2Note, 0.0f,
	//SSN1K_CI_Lfo2Tune, 0.0f,
	SSN1K_CI_Lfo2Fre, dd2db(0x3E99999A),
	//SSN1K_CI_Lfo2Psw, 0.0f,
	SSN1K_CI_Lfo2Wav, SSN1K_WF_SIN,

	//SSN1K_CI_FltMix, 0.0f,
	//SSN1K_CI_FltAmp, 0.0f,
	//SSN1K_CI_FltDC, 0.0f,
	SSN1K_CI_FltRes, 0x80,
	SSN1K_CI_FltMode, SSN1K_FM_LP,
	0xFF,

	/********************************
	* bass
	********************************/
	SSN1K_CI_SynthMix, SSN1K_MM_OVR,
	SSN1K_CI_SynthAmp, 0xFF,
	SSN1K_CI_SynthBal, 0x80,
	//SSN1K_CI_Tune, C(5),

	SSN1K_CI_Env1Mix, dd2db(0x3F600000),
	SSN1K_CI_Env1Amp, dd2db(0x3F800000),
	//SSN1K_CI_Env1DC, 0.0f,
	SSN1K_CI_Env1Atk, 0x20,
	SSN1K_CI_Env1Dec, 0x40,
	SSN1K_CI_Env1Sus, 0x80,
	SSN1K_CI_Env1Rel, 0x80,

	//SSN1K_CI_Env2Mix, 0.0f,
	//SSN1K_CI_Env2Amp, 0.1f,
	//SSN1K_CI_Env2DC, 0.0f,
	//SSN1K_CI_Env2Atk, 0.1f,
	//SSN1K_CI_Env2Dec, 0.2f,
	//SSN1K_CI_Env2Sus, 0.2f,
	//SSN1K_CI_Env2Rel, 0.4f,

	SSN1K_CI_Env3Mix, dd2db(0x3F400000),
	SSN1K_CI_Env3Amp, dd2db(0x43FA0000),
	SSN1K_CI_Env3DC, dd2db(0x42C86666),
	SSN1K_CI_Env3Atk, 0xE0,
	SSN1K_CI_Env3Dec, 0x80,
	SSN1K_CI_Env3Sus, 0x20,
	SSN1K_CI_Env3Rel, 0x60,

	//SSN1K_CI_Env4Mix, SSN1K_MM_BPS,
	//SSN1K_CI_Env4Amp, 0.0f,
	//SSN1K_CI_Env4DC, 0.0f,
	//SSN1K_CI_Env4Atk, 0.0f,
	//SSN1K_CI_Env4Dec, 0.0f,
	//SSN1K_CI_Env4Sus, 0.0f,
	//SSN1K_CI_Env4Rel, 0.0f,
	//SSN1K_CI_Env4Gate, 0.0f,

	//SSN1K_CI_Osc1Mix, CtrlValue(SSN1K_MM_OVR),
	SSN1K_CI_Osc1Amp, 0xC0,
	//SSN1K_CI_Osc1DC, 0.0f,
	//SSN1K_CI_Osc1Note, 0.0f,
	//SSN1K_CI_Osc1Tune, 0.0f,
	//SSN1K_CI_Osc1Fre, 0.0f,
	SSN1K_CI_Osc1Psw, 0x80,
	SSN1K_CI_Osc1Wav, SSN1K_WF_PLS,

	SSN1K_CI_Osc2Mix, SSN1K_MM_ADD,
	SSN1K_CI_Osc2Amp, 0x60,
	//SSN1K_CI_Osc2DC, 0.0f,
	//SSN1K_CI_Osc2Note, 0.0f,
	SSN1K_CI_Osc2Tune, dd2db(0x41433333),
	//SSN1K_CI_Osc2Fre, 0.0f,
	SSN1K_CI_Osc2Psw, 0x80,
	SSN1K_CI_Osc2Wav, SSN1K_WF_PLS,

	//SSN1K_CI_Lfo1Mix, SSN1K_MM_MUL,
	SSN1K_CI_Lfo1Amp, dd2db(0x34000000),
	//SSN1K_CI_Lfo1DC, 0.0f,
	//SSN1K_CI_Lfo1Note, 0.0f,
	SSN1K_CI_Lfo1Tune, dd2db(0xBF800000),
	SSN1K_CI_Lfo1Fre, dd2db(0x40400000),
	//SSN1K_CI_Lfo1Psw, 0.8f,
	SSN1K_CI_Lfo1Wav, SSN1K_WF_PLS,

	//SSN1K_CI_Lfo2Mix,SSN1K_MM_MUL,
	SSN1K_CI_Lfo2Amp, dd2db(0x3E800000),
	//SSN1K_CI_Lfo2DC, 0.0f,
	//SSN1K_CI_Lfo2Note, 0.0f,
	//SSN1K_CI_Lfo2Tune, dd2db(0x3C333333),
	SSN1K_CI_Lfo2Fre, dd2db(0x3F000000),
	//SSN1K_CI_Lfo2Psw, 0.0f,
	SSN1K_CI_Lfo2Wav, SSN1K_WF_SIN,

	//SSN1K_CI_FltMix, 0.0f,
	//SSN1K_CI_FltAmp, 0.0f,
	//SSN1K_CI_FltDC, 0.0f,
	SSN1K_CI_FltRes, 0x80,
	SSN1K_CI_FltMode, SSN1K_FM_LP,
	0xFF,

	/********************************
	* bassline
	********************************/
	SSN1K_CI_SynthMix, SSN1K_MM_OVR,
	SSN1K_CI_SynthAmp, 0xFF,
	SSN1K_CI_SynthBal, 0x80,
	//SSN1K_CI_Tune, C(5),

	SSN1K_CI_Env1Mix, dd2db(0x3F800000),
	SSN1K_CI_Env1Amp, dd2db(0x3F000000),
	//SSN1K_CI_Env1DC, 0.0f,
	SSN1K_CI_Env1Atk, 0x01,
	SSN1K_CI_Env1Dec, 0x28,
	SSN1K_CI_Env1Sus, 0x60,
	SSN1K_CI_Env1Rel, 0x10,

	SSN1K_CI_Env2Mix, dd2db(0x3F400000),
	SSN1K_CI_Env2Amp, dd2db(0x38800000),
	//SSN1K_CI_Env2DC, dd2db(0x3F800000),
	SSN1K_CI_Env2Atk, 0x00,
	SSN1K_CI_Env2Dec, 0x20,
	SSN1K_CI_Env2Sus, 0x40,
	SSN1K_CI_Env2Rel, 0x10,

	SSN1K_CI_Env3Mix, dd2db(0x3F400000),
	SSN1K_CI_Env3Amp, dd2db(0x42960000),
	SSN1K_CI_Env3DC, dd2db(0x44880000),
	SSN1K_CI_Env3Atk, 0x02,
	SSN1K_CI_Env3Dec, 0x10,
	SSN1K_CI_Env3Sus, 0x20,
	SSN1K_CI_Env3Rel, 0x10,

	//SSN1K_CI_Env4Mix, SSN1K_MM_BPS,
	//SSN1K_CI_Env4Amp, 0.0f,
	//SSN1K_CI_Env4DC, 0.0f,
	//SSN1K_CI_Env4Atk, 0.0f,
	//SSN1K_CI_Env4Dec, 0.0f,
	//SSN1K_CI_Env4Sus, 0.0f,
	//SSN1K_CI_Env4Rel, 0.0f,
	//SSN1K_CI_Env4Gate, 0.0f,

	//SSN1K_CI_Osc1Mix, CtrlValue(SSN1K_MM_OVR),
	SSN1K_CI_Osc1Amp, 0xC0,
	//SSN1K_CI_Osc1DC, 0.0f,
	//SSN1K_CI_Osc1Note, 0.0f,
	//SSN1K_CI_Osc1Tune, 0.0f,
	//SSN1K_CI_Osc1Fre, 0.0f,
	SSN1K_CI_Osc1Psw, 0x80,
	SSN1K_CI_Osc1Wav, SSN1K_WF_PSAW,

	SSN1K_CI_Osc2Mix, SSN1K_MM_ADD,
	SSN1K_CI_Osc2Amp, 0x80,
	//SSN1K_CI_Osc2DC, 0.0f,
	//SSN1K_CI_Osc2Note, 0.0f,
	SSN1K_CI_Osc2Tune, dd2db(0x414051EC),
	//SSN1K_CI_Osc2Fre, 0.0f,
	SSN1K_CI_Osc2Psw, 0x84,
	SSN1K_CI_Osc2Wav, SSN1K_WF_PLS,

	//SSN1K_CI_Lfo1Mix, SSN1K_MM_MUL,
	SSN1K_CI_Lfo1Amp, dd2db(0x3C000000),
	//SSN1K_CI_Lfo1DC, 0.0f,
	//SSN1K_CI_Lfo1Note, 0.0f,
	//SSN1K_CI_Lfo1Tune, 0.0f,
	SSN1K_CI_Lfo1Fre, dd2db(0x40400000),
	//SSN1K_CI_Lfo1Psw, 0.8f,
	SSN1K_CI_Lfo1Wav, SSN1K_WF_SIN,

	//SSN1K_CI_Lfo2Mix,SSN1K_MM_MUL,
	SSN1K_CI_Lfo2Amp, dd2db(0x3A800000),
	//SSN1K_CI_Lfo2DC, 0.0f,
	//SSN1K_CI_Lfo2Note, 0.0f,
	//SSN1K_CI_Lfo2Tune, 0.0f,
	SSN1K_CI_Lfo2Fre, dd2db(0x3E99999A),
	//SSN1K_CI_Lfo2Psw, 0.0f,
	SSN1K_CI_Lfo2Wav, SSN1K_WF_SIN,

	//SSN1K_CI_FltMix, 0.0f,
	//SSN1K_CI_FltAmp, 0.0f,
	//SSN1K_CI_FltDC, 0.0f,
	SSN1K_CI_FltRes, 0x80,
	SSN1K_CI_FltMode, SSN1K_FM_LP,
	0xFF,

	/********************************
	* kick
	********************************/
	SSN1K_CI_SynthMix, SSN1K_MM_OVR,
	SSN1K_CI_SynthAmp, 0xFF,
	SSN1K_CI_SynthBal, 0x80,
	SSN1K_CI_Tune, dd2db(0xC2200000),

	SSN1K_CI_Env1Mix, dd2db(0x3F800000),
	SSN1K_CI_Env1Amp, dd2db(0x3F000000),
	//SSN1K_CI_Env1DC, 0.0f,
	SSN1K_CI_Env1Atk, 0x01,
	SSN1K_CI_Env1Dec, 0x28,
	SSN1K_CI_Env1Sus, 0x60,
	SSN1K_CI_Env1Rel, 0x10,

	//SSN1K_CI_Env2Mix, dd2db(0x3F400000),
	//SSN1K_CI_Env2Amp, dd2db(0x38800000),
	////SSN1K_CI_Env2DC, dd2db(0x3F800000),
	//SSN1K_CI_Env2Atk, 0x00,
	//SSN1K_CI_Env2Dec, 0x20,
	//SSN1K_CI_Env2Sus, 0x40,
	//SSN1K_CI_Env2Rel, 0x10,

	SSN1K_CI_Env3Mix, dd2db(0x3F400000),
	SSN1K_CI_Env3Amp, dd2db(0x43960000),
	SSN1K_CI_Env3DC, dd2db(0x42880000),
	SSN1K_CI_Env3Atk, 0x03,
	SSN1K_CI_Env3Dec, 0x10,
	SSN1K_CI_Env3Sus, 0x20,
	SSN1K_CI_Env3Rel, 0x10,

	//SSN1K_CI_Env4Mix, SSN1K_MM_BPS,
	SSN1K_CI_Env4Amp, dd2db(0x43860000),
	//SSN1K_CI_Env4DC, dd2db(0x42C80000),
	SSN1K_CI_Env4Atk, 0x00,
	SSN1K_CI_Env4Dec, 0x03,
	SSN1K_CI_Env4Sus, 0x08,
	SSN1K_CI_Env4Rel, 0x10,
	//SSN1K_CI_Env4Gate, 0.0f,

	//SSN1K_CI_Osc1Mix, CtrlValue(SSN1K_MM_OVR),
	SSN1K_CI_Osc1Amp, 0xC0,
	//SSN1K_CI_Osc1DC, 0.0f,
	//SSN1K_CI_Osc1Note, 0.0f,
	//SSN1K_CI_Osc1Tune, 0.0f,
	//SSN1K_CI_Osc1Fre, 0.0f,
	//SSN1K_CI_Osc1Psw, 0x80,
	SSN1K_CI_Osc1Wav, SSN1K_WF_SIN,

	SSN1K_CI_Osc2Mix, SSN1K_MM_ADD,
	SSN1K_CI_Osc2Amp, 0x40,
	//SSN1K_CI_Osc2DC, 0.0f,
	//SSN1K_CI_Osc2Note, 0.0f,
	SSN1K_CI_Osc2Tune, dd2db(0x4140A3D7),
	//SSN1K_CI_Osc2Fre, 0.0f,
	//SSN1K_CI_Osc2Psw, 0x48,
	SSN1K_CI_Osc2Wav, SSN1K_WF_SIN,

	//SSN1K_CI_Lfo1Mix, SSN1K_MM_MUL,
	SSN1K_CI_Lfo1Amp, dd2db(0x3C000000),
	//SSN1K_CI_Lfo1DC, 0.0f,
	//SSN1K_CI_Lfo1Note, 0.0f,
	//SSN1K_CI_Lfo1Tune, 0.0f,
	SSN1K_CI_Lfo1Fre, dd2db(0x40400000),
	//SSN1K_CI_Lfo1Psw, 0.8f,
	SSN1K_CI_Lfo1Wav, SSN1K_WF_SIN,

	//SSN1K_CI_Lfo2Mix,SSN1K_MM_MUL,
	SSN1K_CI_Lfo2Amp, dd2db(0x3A800000),
	//SSN1K_CI_Lfo2DC, 0.0f,
	//SSN1K_CI_Lfo2Note, 0.0f,
	//SSN1K_CI_Lfo2Tune, 0.0f,
	SSN1K_CI_Lfo2Fre, dd2db(0x3E99999A),
	//SSN1K_CI_Lfo2Psw, 0.0f,
	SSN1K_CI_Lfo2Wav, SSN1K_WF_SIN,

	//SSN1K_CI_FltMix, 0.0f,
	//SSN1K_CI_FltAmp, 0.0f,
	//SSN1K_CI_FltDC, 0.0f,
	SSN1K_CI_FltRes, 0x80,
	SSN1K_CI_FltMode, SSN1K_FM_LP,
	0xFF,

	/********************************
	* snare
	********************************/
	SSN1K_CI_SynthMix, SSN1K_MM_OVR,
	SSN1K_CI_SynthAmp, 0xFF,
	SSN1K_CI_SynthBal, 0x80,
	SSN1K_CI_Tune, dd2db(0xC1600000),

	SSN1K_CI_Env1Mix, dd2db(0x3E800000),
	SSN1K_CI_Env1Amp, dd2db(0x3F000000),
	//SSN1K_CI_Env1DC, 0.0f,
	SSN1K_CI_Env1Atk, 0x01,
	SSN1K_CI_Env1Dec, 0x10,
	SSN1K_CI_Env1Sus, 0x30,
	SSN1K_CI_Env1Rel, 0x10,

	//SSN1K_CI_Env2Mix, 0.0f,
	//SSN1K_CI_Env2Amp, 0.1f,
	//SSN1K_CI_Env2DC, 0.0f,
	//SSN1K_CI_Env2Atk, 0.1f,
	//SSN1K_CI_Env2Dec, 0.2f,
	//SSN1K_CI_Env2Sus, 0.2f,
	//SSN1K_CI_Env2Rel, 0.4f,

	SSN1K_CI_Env3Mix, dd2db(0x3F600000),
	SSN1K_CI_Env3Amp, dd2db(0x447A0000),
	SSN1K_CI_Env3DC, dd2db(0x427A0000),
	SSN1K_CI_Env3Atk, 0x08,
	SSN1K_CI_Env3Dec, 0x14,
	SSN1K_CI_Env3Sus, 0x40,
	SSN1K_CI_Env3Rel, 0x10,

	//SSN1K_CI_Env4Mix, SSN1K_MM_BPS,
	SSN1K_CI_Env4Amp, dd2db(0x42800000),
	//SSN1K_CI_Env4DC, 0.0f,
	SSN1K_CI_Env4Atk, 0x01,
	SSN1K_CI_Env4Dec, 0x03,
	SSN1K_CI_Env4Sus, 0x40,
	SSN1K_CI_Env4Rel, 0x40,

	//SSN1K_CI_Osc1Mix, CtrlValue(SSN1K_MM_OVR),
	SSN1K_CI_Osc1Amp, 0x80,
	//SSN1K_CI_Osc1DC, 0.0f,
	//SSN1K_CI_Osc1Note, 0.0f,
	//SSN1K_CI_Osc1Tune, dd2db(0xC1D00000),
	//SSN1K_CI_Osc1Fre, 0.0f,
	SSN1K_CI_Osc1Psw, 0xA0,
	SSN1K_CI_Osc1Wav, SSN1K_WF_SIN,

	SSN1K_CI_Osc2Mix, SSN1K_MM_MUL ,
	SSN1K_CI_Osc2Amp, 0xE0,
	//SSN1K_CI_Osc2DC, 0.0f,
	//SSN1K_CI_Osc2Note, 0.0f,
	SSN1K_CI_Osc2Tune, dd2db(0x42800000),
	//SSN1K_CI_Osc2Fre, 0.0f,
	//SSN1K_CI_Osc2Psw, 0x78,
	SSN1K_CI_Osc2Wav, SSN1K_WF_RND,

	//SSN1K_CI_Lfo1Mix, SSN1K_MM_MUL,
	//SSN1K_CI_Lfo1Amp, dd2db(0x31000000),
	//SSN1K_CI_Lfo1DC, 0.0f,
	//SSN1K_CI_Lfo1Note, 0.0f,
	//SSN1K_CI_Lfo1Tune, 0.0f,
	//SSN1K_CI_Lfo1Fre, dd2db(0x40400000),
	//SSN1K_CI_Lfo1Psw, 0.8f,
	//SSN1K_CI_Lfo1Wav, SSN1K_WF_SIN,

	//SSN1K_CI_Lfo2Mix,SSN1K_MM_MUL,
	//SSN1K_CI_Lfo2Amp, dd2db(0x31000000),
	//SSN1K_CI_Lfo2DC, 0.0f,
	//SSN1K_CI_Lfo2Note, 0.0f,
	//SSN1K_CI_Lfo2Tune, 0.0f,
	//SSN1K_CI_Lfo2Fre, dd2db(0x3E99999A),
	//SSN1K_CI_Lfo2Psw, 0.0f,
	//SSN1K_CI_Lfo2Wav, SSN1K_WF_SIN,

	//SSN1K_CI_FltMix, 0.0f,
	//SSN1K_CI_FltAmp, 0.0f,
	//SSN1K_CI_FltDC, 0.0f,
	SSN1K_CI_FltRes, 0x88,
	SSN1K_CI_FltMode, SSN1K_FM_LP,
	0xFF,

	/********************************
	* saw
	********************************/
	SSN1K_CI_SynthMix, SSN1K_MM_OVR,
	SSN1K_CI_SynthAmp, 0xFF,
	SSN1K_CI_SynthBal, 0x80,
	SSN1K_CI_Tune, dd2db(0x31C00000),

	SSN1K_CI_Env1Amp, dd2db(0x3F800000),
	SSN1K_CI_Env1Atk, 0x00,
	SSN1K_CI_Env1Dec, 0x10,
	SSN1K_CI_Env1Sus, 0x80,
	SSN1K_CI_Env1Rel, 0x20,

	SSN1K_CI_Env2Mix, dd2db(0x3F400000),
	SSN1K_CI_Env2Amp, dd2db(0x38800000),
	//SSN1K_CI_Env2DC, dd2db(0x3F800000),
	SSN1K_CI_Env2Atk, 0x00,
	SSN1K_CI_Env2Dec, 0x20,
	SSN1K_CI_Env2Sus, 0x40,
	SSN1K_CI_Env2Rel, 0x10,

	SSN1K_CI_Env3Mix, dd2db(0x3F600000),
	SSN1K_CI_Env3Amp, dd2db(0x447A0000),
	SSN1K_CI_Env3DC, dd2db(0x427A0000),
	SSN1K_CI_Env3Atk, 0x08,
	SSN1K_CI_Env3Dec, 0x14,
	SSN1K_CI_Env3Sus, 0x80,
	SSN1K_CI_Env3Rel, 0x10,


	SSN1K_CI_Osc1Amp, 0xE0,
	SSN1K_CI_Osc1Psw, 0x80,
	SSN1K_CI_Osc1Wav, SSN1K_WF_PSAW,

	SSN1K_CI_Osc2Mix, SSN1K_MM_ADD,
	SSN1K_CI_Osc2Amp, 0x30,
	SSN1K_CI_Osc2Psw, 0x60,
	SSN1K_CI_Osc2Tune, dd2db(0x414051EC),

	//SSN1K_CI_FltMix, SSN1K_MM_BPS,
	SSN1K_CI_FltRes, 0x60,
	SSN1K_CI_FltMode, SSN1K_FM_LP,
	0xFF,

	/********************************
	* pls
	********************************/
	SSN1K_CI_SynthMix, SSN1K_MM_OVR,
	SSN1K_CI_SynthAmp, 0x80,
	SSN1K_CI_SynthBal, 0x80,
	SSN1K_CI_Tune, dd2db(0x31C00000),

	SSN1K_CI_Env1Amp, dd2db(0x3F800000),
	SSN1K_CI_Env1Atk, 0x08,
	SSN1K_CI_Env1Dec, 0x10,
	SSN1K_CI_Env1Sus, 0x40,
	SSN1K_CI_Env1Rel, 0x20,

	//SSN1K_CI_Env2Mix, dd2db(0),	//dd2db(0x3F400000),
	SSN1K_CI_Env2Amp, dd2db(0x38800000),
	//SSN1K_CI_Env2DC, dd2db(0x3F800000),
	SSN1K_CI_Env2Atk, 0x01,
	SSN1K_CI_Env2Dec, 0x10,
	SSN1K_CI_Env2Sus, 0x40,
	SSN1K_CI_Env2Rel, 0x20,

	SSN1K_CI_Env3Mix, dd2db(0x3F400000),
	SSN1K_CI_Env3Amp, dd2db(0x447A0000),
	SSN1K_CI_Env3DC, dd2db(0x447A0000),
	SSN1K_CI_Env3Atk, 0x08,
	SSN1K_CI_Env3Dec, 0x10,
	SSN1K_CI_Env3Sus, 0xC0,
	SSN1K_CI_Env3Rel, 0x02,


	SSN1K_CI_Osc1Amp, 0x60,
	SSN1K_CI_Osc1Psw, 0x80,
	SSN1K_CI_Osc1Wav, SSN1K_WF_PLS,

	SSN1K_CI_Osc2Mix, SSN1K_MM_BPS,
	SSN1K_CI_Osc2Amp, 0x40,
	SSN1K_CI_Osc2Psw, 0x80,
	SSN1K_CI_Osc2Tune, dd2db(0x414051EC),

	//SSN1K_CI_FltMix, SSN1K_MM_BPS,
	SSN1K_CI_FltRes, 0x10,
	SSN1K_CI_FltMode, SSN1K_FM_LP,
	0xFF
};

/// <summary>synth configuration</summary>
/// <remarks>
/// - number of synths
/// - synth configurations: voice count
/// </remarks>
unsigned char synthConfig[] = { 0x08, 0x03, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01 };

#endif