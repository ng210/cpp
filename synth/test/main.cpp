#define _USE_MATH_DEFINES
#include <math.h>
#include "test.h"
#include "console/consoleapp.h"
#include "soundlib/src/soundplayer.h"
#include "soundlib/src/wavfmt.h"
#include "player/src/player-lib.h"
#include "player/src/player-adapter.h"
#include "synth/src/device/synth-adapter.h"
#include "synth/src/device/synth-device.h"
#include "synth/src/device/bass-device.h"
#include "synth/src/device/drums-device.h"
#include "synth/src/device/mixer-device.h"
#include "synth/src/device/distort-device.h"
#include "synth/src/device/stereo-delay-device.h"
#include "synth/src/xmloader.h"

NS_FW_BASE_USE
using namespace SYNTH;
using namespace PLAYER;

#define SAMPLING_RATE 48000
#define LAST_TICK 128

var isSaveToWave_ = false;
var framePerRow_ = 0;
var totalFrames_ = 0;
Array patternOrder_(sizeof(int), 256);
Array patterns_(sizeof(int), 64);

typedef char* (ROWPROCESSOR)(PArray* tokens, Sequence* sequence, int& delta);

PlayerAdapter playerAdapter_;
SynthAdapter synthAdapter_;


void saveToBuffer(Drums* drums, short*& buffer, int length) {
    var output = drums->getOutput(0);
    while (length > 0) {
        var toWrite = length > SAMPLE_BUFFER_SIZE ? SAMPLE_BUFFER_SIZE : length;
        drums->run(0, toWrite);
        for (var j = 0; j < toWrite; j++) {
            *buffer++ = (short)(32767.0f * output[j]);
        }
        length -= toWrite;
    }
}

char* synthRowProcessor(PArray* tokens, Sequence* sequence, int& delta) {
    const char* notes[12] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "H"};
    char* txt = NULL;
    char* ptr = NULL;
    var firstNote = true;
    for (var i = 0; i < tokens->length(); i++) {
        var token = (char*)tokens->get(i);
        var note = 0, velocity = 0;
        for (var j = 0; j < arraysize(notes); j++) {
            var ptr = token;
            var noteToken = notes[j];
            // check note
            var len = fmw::strlen(noteToken);
            if (fmw::strncmp(token, noteToken, len) == 0) {
                // check octave
                var oct = -1;
                parseInt(token + len, oct);
                // calculate note (1 = C#0)
                note = j + 12 * oct;
                break;
            }
        }

        if (note != 0) {
            // read velocity
            var vel = (char*)tokens->get(i + 1);
            var p = parseInt(vel, velocity);
            if (p != vel) {
                velocity = velocity * 255 / 10;
                i++;
            }
            else {
                velocity = 255;
            }

            if (firstNote) {
                sequence->writeDelta(delta);
                firstNote = false;
            }

            // add note
            var command = sequence->device()->makeCommand(ModuleCommands::CmdSetNote, note, velocity);
            if (ptr == NULL) {
                ptr = MALLOC(char, 1024);
                ptr[0] = '\0';
            }
            str_format_s(ptr, 255, " %s(%d): %0.2f  ", token, note, velocity / 255.0f);
            ptr += fmw::strlen(ptr);
            sequence->writeStream(command);
            delta = 0;
            DEL_(command);
        }
    }
    if (!firstNote) sequence->writeEOF();
    return txt;
}

char* drumsRowProcessor(PArray* tokens, Sequence* sequence, int& delta) {
    char* txt = NULL;
    char* ptr;
    var firstNote = true;
    for (var i = 0; i < tokens->length(); i++) {
        var token = (char*)tokens->get(i);
        var note = 0, velocity = 0;
        if (fmw::strncmp(token, "BD", 2) == 0) {
            note = drBD;
        }
        else if (fmw::strncmp(token, "SD", 2) == 0) {
            note = drSD;
        }
        else if (fmw::strncmp(token, "OH", 2) == 0) {
            note = drOH;
        }
        else if (fmw::strncmp(token, "CH", 2) == 0) {
            note = drCH;
        }
        else if (fmw::strncmp(token, "LT", 2) == 0) {
            note = drLT;
        }
        else if (fmw::strncmp(token, "MT", 2) == 0) {
            note = drMT;
        }
        else if (fmw::strncmp(token, "HT", 2) == 0) {
            note = drHT;
        }
        else if (fmw::strncmp(token, "CP", 2) == 0) {
            note = drCP;
        }
        if (note != 0) {
            // read velocity
            var vel = (char*)tokens->get(i + 1);
            var p = parseInt(vel, velocity);
            if (p != vel) {
                velocity = velocity * 255 / 10;
                i++;
            }
            else {
                velocity = 255;
            }

            if (firstNote) {
                sequence->writeDelta(delta);
                firstNote = false;
            }

            // add drum
            var command = sequence->device()->makeCommand(ModuleCommands::CmdSetNote, note, velocity);
            if (txt == NULL) {
                txt = ptr = MALLOC(char, 1024);
                ptr[0] = '\0';
            }
            str_format_s(ptr, 255, " %s: %0.2f  ", token, velocity/255.0f);
            ptr += fmw::strlen(ptr);
            sequence->writeStream(command);
            delta = 0;
            DEL_(command);
        }
    }
    if (!firstNote) sequence->writeEOF();
    return txt;
}

//Stream* soundBank_ = NULL;
//Stream* getSoundBank() {
//    if (soundBank_ == NULL) {
//        int count = 0;
//        var bass = NEW_(Stream);
//        {
//            count++;
//            bass->writeByte(32);
//            bass->writeByte(SynthCtrlId::amAdsrAmp)->writeFloat(1.0f);
//            //bass->writeByte(SynthCtrlId::amAdsrDc)->writeFloat(0.0f);
//            bass->writeByte(SynthCtrlId::amAdsrAtk)->writeByte(22);
//            bass->writeByte(SynthCtrlId::amAdsrDec)->writeByte(68);
//            bass->writeByte(SynthCtrlId::amAdsrSus)->writeByte(80);
//            bass->writeByte(SynthCtrlId::amAdsrRel)->writeByte(82);         // 5
//
//            //bass->writeByte(SynthCtrlId::fmAdsrAmp)->writeFloat(0.0f);
//            ////bass->writeByte(SynthCtrlId::fmAdsrDc)->writeFloat(0.0f);
//            //bass->writeByte(SynthCtrlId::fmAdsrAtk)->writeByte(0);
//            //bass->writeByte(SynthCtrlId::fmAdsrDec)->writeByte(0);
//            //bass->writeByte(SynthCtrlId::fmAdsrSus)->writeByte(0);
//            //bass->writeByte(SynthCtrlId::fmAdsrRel)->writeByte(0);        // 5
//
//            bass->writeByte(SynthCtrlId::pmAdsrAmp)->writeFloat(0.2f);
//            //bass->writeByte(SynthCtrlId::pmAdsrDc)->writeFloat(0.6f);
//            bass->writeByte(SynthCtrlId::pmAdsrAtk)->writeByte(40);
//            bass->writeByte(SynthCtrlId::pmAdsrDec)->writeByte(144);
//            bass->writeByte(SynthCtrlId::pmAdsrSus)->writeByte(128);
//            bass->writeByte(SynthCtrlId::pmAdsrRel)->writeByte(200);       // 5
//
//            bass->writeByte(SynthCtrlId::ftAdsrAmp)->writeFloat(0.5f);
//            bass->writeByte(SynthCtrlId::ftAdsrAtk)->writeByte(122);
//            bass->writeByte(SynthCtrlId::ftAdsrDec)->writeByte(122);
//            bass->writeByte(SynthCtrlId::ftAdsrSus)->writeByte(100);
//            bass->writeByte(SynthCtrlId::ftAdsrRel)->writeByte(222);       // 5
//
//            bass->writeByte(SynthCtrlId::amLfoAmp)->writeFloat(0.0f);
//            bass->writeByte(SynthCtrlId::amLfoFre)->writeFloat(2.0f);
//            bass->writeByte(SynthCtrlId::fmLfoAmp)->writeFloat(0.0f);
//            bass->writeByte(SynthCtrlId::fmLfoFre)->writeFloat(3.8f);    // 4
//
//            bass->writeByte(SynthCtrlId::osc1Amp)->writeByte(160);
//            bass->writeByte(SynthCtrlId::osc1Fre)->writeFloat(0.0f);
//            bass->writeByte(SynthCtrlId::osc1Tune)->writeByte(0);
//            bass->writeByte(SynthCtrlId::osc1Psw)->writeByte(120);
//            bass->writeByte(SynthCtrlId::osc1Wave)->writeByte(WfPulse);  // 5
//
//            bass->writeByte(SynthCtrlId::osc2Amp)->writeByte(120);
//            bass->writeByte(SynthCtrlId::osc2Fre)->writeFloat(0.2f);
//            bass->writeByte(SynthCtrlId::osc2Tune)->writeByte(12);
//            bass->writeByte(SynthCtrlId::osc2Psw)->writeByte(100);
//            bass->writeByte(SynthCtrlId::osc2Wave)->writeByte(WfPulse);    // 5
//
//            bass->writeByte(SynthCtrlId::flt1Cut)->writeByte(  0);
//            bass->writeByte(SynthCtrlId::flt1Res)->writeByte(160);
//            bass->writeByte(SynthCtrlId::flt1Mode)->writeByte(FmLowPass);  // 3
//        }
//        var chords = NEW_(Stream);
//        {
//            count++;
//            chords->writeByte(32);
//            chords->writeByte(SynthCtrlId::amAdsrAmp)->writeFloat(1.0f);
//            //chords->writeByte(SynthCtrlId::amAdsrDc)->writeFloat(0.0f);
//            chords->writeByte(SynthCtrlId::amAdsrAtk)->writeByte(62);
//            chords->writeByte(SynthCtrlId::amAdsrDec)->writeByte(128);
//            chords->writeByte(SynthCtrlId::amAdsrSus)->writeByte(120);
//            chords->writeByte(SynthCtrlId::amAdsrRel)->writeByte(162);       // 5
//
//            //chords->writeByte(SynthCtrlId::fmAdsrAmp)->writeFloat(0.0f);
//            //chords->writeByte(SynthCtrlId::fmAdsrDc)->writeFloat(0.0f);
//            //chords->writeByte(SynthCtrlId::fmAdsrAtk)->writeByte(0);
//            //chords->writeByte(SynthCtrlId::fmAdsrDec)->writeByte(0);
//            //chords->writeByte(SynthCtrlId::fmAdsrSus)->writeByte(0);
//            //chords->writeByte(SynthCtrlId::fmAdsrRel)->writeByte(0);        // 5
//
//            chords->writeByte(SynthCtrlId::pmAdsrAmp)->writeFloat(0.2f);
//            //chords->writeByte(SynthCtrlId::pmAdsrDc)->writeFloat(0.45f);
//            chords->writeByte(SynthCtrlId::pmAdsrAtk)->writeByte(22);
//            chords->writeByte(SynthCtrlId::pmAdsrDec)->writeByte(84);
//            chords->writeByte(SynthCtrlId::pmAdsrSus)->writeByte(128);
//            chords->writeByte(SynthCtrlId::pmAdsrRel)->writeByte(80);       // 5
//
//            chords->writeByte(SynthCtrlId::ftAdsrAmp)->writeFloat(0.4f);
//            chords->writeByte(SynthCtrlId::ftAdsrAtk)->writeByte(122);
//            chords->writeByte(SynthCtrlId::ftAdsrDec)->writeByte(222);
//            chords->writeByte(SynthCtrlId::ftAdsrSus)->writeByte(160);
//            chords->writeByte(SynthCtrlId::ftAdsrRel)->writeByte(122);       // 5
//
//            chords->writeByte(SynthCtrlId::amLfoAmp)->writeFloat(0.1f);
//            chords->writeByte(SynthCtrlId::amLfoFre)->writeFloat(4.1f);
//            chords->writeByte(SynthCtrlId::fmLfoAmp)->writeFloat(1.4f);
//            chords->writeByte(SynthCtrlId::fmLfoFre)->writeFloat(4.8f);    // 4
//
//            chords->writeByte(SynthCtrlId::osc1Amp)->writeByte(160);
//            chords->writeByte(SynthCtrlId::osc1Fre)->writeFloat(0.0f);
//            chords->writeByte(SynthCtrlId::osc1Tune)->writeByte(0);
//            chords->writeByte(SynthCtrlId::osc1Psw)->writeByte(60);
//            chords->writeByte(SynthCtrlId::osc1Wave)->writeByte(WfSaw);  // 5
//
//            chords->writeByte(SynthCtrlId::osc2Amp)->writeByte(100);
//            chords->writeByte(SynthCtrlId::osc2Fre)->writeFloat(0.4f);
//            chords->writeByte(SynthCtrlId::osc2Tune)->writeByte(12);
//            chords->writeByte(SynthCtrlId::osc2Psw)->writeByte(255);
//            chords->writeByte(SynthCtrlId::osc2Wave)->writeByte(WfSaw);    // 5
//
//            chords->writeByte(SynthCtrlId::flt1Cut)->writeByte(20);
//            chords->writeByte(SynthCtrlId::flt1Res)->writeByte( 4);
//            chords->writeByte(SynthCtrlId::flt1Mode)->writeByte(FmBandPass);  // 3
//        }
//        var mono = NEW_(Stream);
//        {
//            count++;
//            mono->writeByte(28);
//            mono->writeByte(SynthCtrlId::amAdsrAmp)->writeFloat(1.0f);
//            mono->writeByte(SynthCtrlId::amAdsrAtk)->writeByte(2);
//            mono->writeByte(SynthCtrlId::amAdsrDec)->writeByte(28);
//            mono->writeByte(SynthCtrlId::amAdsrSus)->writeByte(80);
//            mono->writeByte(SynthCtrlId::amAdsrRel)->writeByte(32);       // 5
//
//            mono->writeByte(SynthCtrlId::pmAdsrAmp)->writeFloat(0.2f);
//            mono->writeByte(SynthCtrlId::pmAdsrAtk)->writeByte(20);
//            mono->writeByte(SynthCtrlId::pmAdsrDec)->writeByte(64);
//            mono->writeByte(SynthCtrlId::pmAdsrSus)->writeByte(128);
//            mono->writeByte(SynthCtrlId::pmAdsrRel)->writeByte(40);       // 5
//
//            mono->writeByte(SynthCtrlId::ftAdsrAmp)->writeFloat(1.0f);
//            mono->writeByte(SynthCtrlId::ftAdsrAtk)->writeByte(2);
//            mono->writeByte(SynthCtrlId::ftAdsrDec)->writeByte(22);
//            mono->writeByte(SynthCtrlId::ftAdsrSus)->writeByte(80);
//            mono->writeByte(SynthCtrlId::ftAdsrRel)->writeByte(32);       // 5
//
//            mono->writeByte(SynthCtrlId::osc1Amp)->writeByte(160);
//            mono->writeByte(SynthCtrlId::osc1Fre)->writeFloat(0.0f);
//            mono->writeByte(SynthCtrlId::osc1Tune)->writeByte(0);
//            mono->writeByte(SynthCtrlId::osc1Psw)->writeByte(0);
//            mono->writeByte(SynthCtrlId::osc1Wave)->writeByte(WfSaw);  // 5
//
//            mono->writeByte(SynthCtrlId::osc2Amp)->writeByte(120);
//            mono->writeByte(SynthCtrlId::osc2Fre)->writeFloat(0.1f);
//            mono->writeByte(SynthCtrlId::osc2Tune)->writeByte(12);
//            mono->writeByte(SynthCtrlId::osc2Psw)->writeByte(10);
//            mono->writeByte(SynthCtrlId::osc2Wave)->writeByte(WfSaw);    // 5
//
//            mono->writeByte(SynthCtrlId::flt1Cut)->writeByte(120);
//            mono->writeByte(SynthCtrlId::flt1Res)->writeByte(0);
//            mono->writeByte(SynthCtrlId::flt1Mode)->writeByte(FmLowPass);  // 3
//        }
//        var piano = NEW_(Stream);
//        {
//            count++;
//            piano->writeByte(28);
//            piano->writeByte(SynthCtrlId::amAdsrAmp)->writeFloat(1.0f);
//            piano->writeByte(SynthCtrlId::amAdsrAtk)->writeByte(  0);
//            piano->writeByte(SynthCtrlId::amAdsrDec)->writeByte( 30);
//            piano->writeByte(SynthCtrlId::amAdsrSus)->writeByte( 80);
//            piano->writeByte(SynthCtrlId::amAdsrRel)->writeByte( 20);       // 5
//
//            piano->writeByte(SynthCtrlId::pmAdsrAmp)->writeFloat(0.3f);
//            piano->writeByte(SynthCtrlId::pmAdsrAtk)->writeByte(  0);
//            piano->writeByte(SynthCtrlId::pmAdsrDec)->writeByte( 24);
//            piano->writeByte(SynthCtrlId::pmAdsrSus)->writeByte(128);
//            piano->writeByte(SynthCtrlId::pmAdsrRel)->writeByte(120);       // 5
//
//            piano->writeByte(SynthCtrlId::ftAdsrAmp)->writeFloat(1.0f);
//            piano->writeByte(SynthCtrlId::ftAdsrAtk)->writeByte(  2);
//            piano->writeByte(SynthCtrlId::ftAdsrDec)->writeByte( 32);
//            piano->writeByte(SynthCtrlId::ftAdsrSus)->writeByte( 80);
//            piano->writeByte(SynthCtrlId::ftAdsrRel)->writeByte( 42);       // 5
//
//            piano->writeByte(SynthCtrlId::osc1Amp)->writeByte( 100);
//            piano->writeByte(SynthCtrlId::osc1Fre)->writeFloat(0.0f);
//            piano->writeByte(SynthCtrlId::osc1Tune)->writeByte(  0);
//            piano->writeByte(SynthCtrlId::osc1Psw)->writeByte(   0);
//            piano->writeByte(SynthCtrlId::osc1Wave)->writeByte(WfTriangle);  // 5
//
//            piano->writeByte(SynthCtrlId::osc2Amp)->writeByte(  80);
//            piano->writeByte(SynthCtrlId::osc2Fre)->writeFloat(0.1f);
//            piano->writeByte(SynthCtrlId::osc2Tune)->writeByte(  0);
//            piano->writeByte(SynthCtrlId::osc2Psw)->writeByte(   0);
//            piano->writeByte(SynthCtrlId::osc2Wave)->writeByte(WfTriangle);    // 5
//
//            piano->writeByte(SynthCtrlId::flt1Cut)->writeByte(10);
//            piano->writeByte(SynthCtrlId::flt1Res)->writeByte(80);
//            piano->writeByte(SynthCtrlId::flt1Mode)->writeByte(FmLowPass|FmBandPass);  // 3
//        }
//
//        word offset = 1 + count * 16;
//        soundBank_ = NEW_(Stream, 1024);
//        soundBank_->writeByte(count);
//        soundBank_->writeString("bassline.....");
//        soundBank_->writeWord(offset); offset += (word)bass->length();
//        soundBank_->writeString("kick.........");
//        soundBank_->writeWord(offset); offset += (word)chords->length();
//        soundBank_->writeString("mono.........");
//        soundBank_->writeWord(offset); offset += (word)mono->length();
//        soundBank_->writeString("piano........");
//        soundBank_->writeWord(offset); offset += (word)piano->length();
//
//        soundBank_->writeStream(bass);
//        soundBank_->writeStream(chords);
//        soundBank_->writeStream(mono);
//        soundBank_->writeStream(piano);        
//
//        DEL_(bass);
//        DEL_(chords);
//        DEL_(mono);
//        DEL_(piano);
//    }
//
//    return soundBank_;
//}
//
//Stream* bassSoundBank_ = NULL;
//Stream* getBassSoundBank() {
//    if (bassSoundBank_ == NULL) {
//        int count = 0;
//        var bass = NEW_(Stream);
//        {
//            bass->writeByte(28);
//            bass->writeByte(BassCtrlId::bAmAdsrAmp)->writeFloat(1.0f);
//            bass->writeByte(BassCtrlId::bAmAdsrAtk)->writeByte(12);
//            bass->writeByte(BassCtrlId::bAmAdsrDec)->writeByte(60);
//            bass->writeByte(BassCtrlId::bAmAdsrSus)->writeByte(160);
//            bass->writeByte(BassCtrlId::bAmAdsrRel)->writeByte(100);         // 5
//
//            bass->writeByte(BassCtrlId::bPmAdsrAmp)->writeFloat(0.0f);
//            bass->writeByte(BassCtrlId::bPmAdsrAtk)->writeByte(20);
//            bass->writeByte(BassCtrlId::bPmAdsrDec)->writeByte(144);
//            bass->writeByte(BassCtrlId::bPmAdsrSus)->writeByte(128);
//            bass->writeByte(BassCtrlId::bPmAdsrRel)->writeByte(100);       // 5
//
//            bass->writeByte(BassCtrlId::bFtAdsrAmp)->writeFloat(0.2f);
//            bass->writeByte(BassCtrlId::bFtAdsrAtk)->writeByte( 2);
//            bass->writeByte(BassCtrlId::bFtAdsrDec)->writeByte(60);
//            bass->writeByte(BassCtrlId::bFtAdsrSus)->writeByte(40);
//            bass->writeByte(BassCtrlId::bFtAdsrRel)->writeByte(80);       // 5
//
//            bass->writeByte(BassCtrlId::bOsc1Amp)->writeByte(120);
//            bass->writeByte(BassCtrlId::bOsc1Fre)->writeFloat(0.0f);
//            bass->writeByte(BassCtrlId::bOsc1Tune)->writeByte(0);
//            bass->writeByte(BassCtrlId::bOsc1Psw)->writeByte(120);
//            bass->writeByte(BassCtrlId::bOsc1Wave)->writeByte(WfSaw);  // 5
//
//            bass->writeByte(BassCtrlId::bOsc2Amp)->writeByte(60);
//            bass->writeByte(BassCtrlId::bOsc2Fre)->writeFloat(0.4f);
//            bass->writeByte(BassCtrlId::bOsc2Tune)->writeByte(12);
//            bass->writeByte(BassCtrlId::bOsc2Psw)->writeByte(100);
//            bass->writeByte(BassCtrlId::bOsc2Wave)->writeByte(WfPulse); // 5
//
//            bass->writeByte(BassCtrlId::bFlt1Cut)->writeByte(10);
//            bass->writeByte(BassCtrlId::bFlt1Res)->writeByte(80);
//            bass->writeByte(BassCtrlId::bFlt1Mode)->writeByte(FmLowPass);  // 3
//            count++;
//        };
//
//        var bass2 = NEW_(Stream);
//        {
//            bass2->writeByte(28);
//            bass2->writeByte(BassCtrlId::bAmAdsrAmp)->writeFloat(1.0f);
//            bass2->writeByte(BassCtrlId::bAmAdsrAtk)->writeByte(250);
//            bass2->writeByte(BassCtrlId::bAmAdsrDec)->writeByte(240);
//            bass2->writeByte(BassCtrlId::bAmAdsrSus)->writeByte(220);
//            bass2->writeByte(BassCtrlId::bAmAdsrRel)->writeByte(240);         // 5
//
//            bass2->writeByte(BassCtrlId::bPmAdsrAmp)->writeFloat(0.2f);
//            bass2->writeByte(BassCtrlId::bPmAdsrAtk)->writeByte(120);
//            bass2->writeByte(BassCtrlId::bPmAdsrDec)->writeByte(244);
//            bass2->writeByte(BassCtrlId::bPmAdsrSus)->writeByte(88);
//            bass2->writeByte(BassCtrlId::bPmAdsrRel)->writeByte(200);       // 5
//
//            bass2->writeByte(BassCtrlId::bFtAdsrAmp)->writeFloat(0.2f);
//            bass2->writeByte(BassCtrlId::bFtAdsrAtk)->writeByte(250);
//            bass2->writeByte(BassCtrlId::bFtAdsrDec)->writeByte(200);
//            bass2->writeByte(BassCtrlId::bFtAdsrSus)->writeByte(120);
//            bass2->writeByte(BassCtrlId::bFtAdsrRel)->writeByte(240);       // 5
//
//            bass2->writeByte(BassCtrlId::bOsc1Amp)->writeByte(100);
//            bass2->writeByte(BassCtrlId::bOsc1Fre)->writeFloat(0.0f);
//            bass2->writeByte(BassCtrlId::bOsc1Tune)->writeByte(0);
//            bass2->writeByte(BassCtrlId::bOsc1Psw)->writeByte(120);
//            bass2->writeByte(BassCtrlId::bOsc1Wave)->writeByte(WfSaw | WfPulse | WfSinus);  // 5
//
//            bass2->writeByte(BassCtrlId::bOsc2Amp)->writeByte(20);
//            bass2->writeByte(BassCtrlId::bOsc2Fre)->writeFloat(3487.04f);
//            bass2->writeByte(BassCtrlId::bOsc2Tune)->writeByte(0);
//            bass2->writeByte(BassCtrlId::bOsc2Psw)->writeByte(120);
//            bass2->writeByte(BassCtrlId::bOsc2Wave)->writeByte(WfNoise); // 5
//
//            bass2->writeByte(BassCtrlId::bFlt1Cut)->writeByte(4);
//            bass2->writeByte(BassCtrlId::bFlt1Res)->writeByte(100);
//            bass2->writeByte(BassCtrlId::bFlt1Mode)->writeByte(FmLowPass);  // 3
//            count++;
//        };
//
//        word offset = 1 + count * 16;
//        bassSoundBank_ = NEW_(Stream, 4096);
//        bassSoundBank_->writeByte(count);
//        bassSoundBank_->writeString("bass.........");
//        bassSoundBank_->writeWord(offset); offset += (word)bass->length();
//        bassSoundBank_->writeString("bass2........");
//        bassSoundBank_->writeWord(offset); offset += (word)bass2->length();
//
//        bassSoundBank_->writeStream(bass);
//        bassSoundBank_->writeStream(bass2);
//        DEL_(bass);
//        DEL_(bass2);
//    }
//    return bassSoundBank_;
//}

void parseFile(char* file, Player* player, Map* rowProcessorMap) {
    // file structure
    // - tempo
    //   - keyword: TEMPO
    //   - bpm, frame per row
    // - devices
    //   - keyword: DEVICES
    //   - devices ids
    // - master pattern (pattern order)
    //   - keyword: MASTER
    //   - pattern ids
    // - patterns
    //   - keyword: PATTERN
    //   - rows (notes, commands)
    var cons = getConsole();
    var stream = NEW_(Stream, 16384);
    stream->readBytesFromFile(file, 16384, 0);
    cons->printf(" * %d bytes read.\n", stream->length());
    stream->reset();
    PArray devices(16);

#pragma region HEADER
    // read tempo
    var row = stream->readRow();
    var tokens = PArray::str_split(row, " ");
    var bpm = 120.0;
    framePerRow_ = 4;
    totalFrames_ = 0;
    if (fmw::strncmp((char*)tokens->get(0), "TEMPO", 5) == 0)
    {
        parseDouble((char*)tokens->get(1), bpm);
        parseInt((char*)tokens->get(2), framePerRow_);
        cons->printf(" * Tempo: %0#.2f bpm, %d frames per row\n", bpm, framePerRow_);
        var framePerSecond = bpm * framePerRow_ / 60.0;
        player->refreshRate((float)framePerSecond);
    }
    ARRAY_FOREACH(tokens, FREE(value));
    DEL_(tokens);
    FREE(row);

    // read devices
    row = stream->readRow();
    tokens = PArray::str_split(row, " ");
    Bass bass(&synthAdapter_.samplingRate);
    Synth synth(&synthAdapter_.samplingRate);
    if (fmw::strncmp((char*)tokens->get(0), "DEVICES", 7) == 0)
    {
        for (var i = 1; i < tokens->length(); i++) {
            var devIdToken = (char*)tokens->get(i);
            var deviceType = 0;
            char* deviceName = "Drums";
            if (fmw::strncmp(devIdToken, deviceName, 5) == 0) {
                deviceType = SynthDevices::DeviceDrums;
            }
            else {
                deviceName = "Bass";
                if (fmw::strncmp(devIdToken, "Bass", 4) == 0) {
                    deviceType = SynthDevices::DeviceBass;
                }
                else {
                    deviceName = "Synth";
                    if (fmw::strncmp(devIdToken, "Synth", 5) == 0) {
                        deviceType = SynthDevices::DeviceSynth;
                    }
                }
            }

            cons->printf(" * Device %s(%d) added.\n", deviceName, deviceType);
            var device = player->addDevice(player->getAdapter(synthAdapter_.getInfo()->id), deviceType, NULL);
            devices.push(device);
        }
    }
    ARRAY_FOREACH(tokens, FREE(value));
    DEL_(tokens);
    FREE(row);

    // read master
    row = stream->readRow();
    tokens = PArray::str_split(row, " ");
    if (fmw::strncmp((char*)tokens->get(0), "MASTER", 6) == 0)
    {
        cons->printf(" * Pattern order:");
        for (var i = 1; i < tokens->length(); i++) {
            var patternToken = (char*)tokens->get(i);
            var patternId = -1;
            var p = parseInt(patternToken, patternId);
            if (p != patternToken) {
                patternOrder_.push(&patternId);
                cons->printf("%02X ", patternId);
            }
        }
    }
    cons->printf("\n");
    ARRAY_FOREACH(tokens, FREE(value));
    DEL_(tokens);
    FREE(row);
#pragma endregion

#pragma region PATTERNS
    var delta = 0;
    var totalDelta = 0;
    int patternLength[256];
    Sequence* sequence = NULL;
    var seqId = 0;
    Device* device = NULL;
    while ((row = stream->readRow()) != NULL) {
        tokens = PArray::str_split(row, " ");
        if (fmw::strncmp(row, "PATTERN", 7) == 0) {
            if (sequence != NULL) {
                sequence->writeDelta(delta)->writeEOS();
                patternLength[seqId] = totalDelta;
                seqId++;
                player->addSequence(sequence);
                cons->printf(" * Sequence added with %d frames.\n\n", totalDelta);
            }
            cons->printf(" * Pattern %d\n", seqId + 1);
            var deviceId = -1;
            parseInt((char*)tokens->get(1), deviceId);
            device = (Device*)devices.get(deviceId-1);
            if (device == NULL) {
                cons->printf("Device %d not found!", deviceId);
                break;
            }
            sequence = NEW_(Sequence, device);
            sequence->writeHeader();
            delta = 0;
            totalDelta = 0;
            ARRAY_FOREACH(tokens, FREE(value));
            DEL_(tokens);
            FREE(row);
            continue;
        }

        var rowProcessor = *(ROWPROCESSOR**)rowProcessorMap->get(device->type());
        var txt = rowProcessor(tokens, sequence, delta);
        if (txt != NULL) {
            cons->printf("%4d  %s\n", totalDelta, txt);
            FREE(txt);
        }
        delta++;
        totalDelta++;
        ARRAY_FOREACH(tokens, FREE(value));
        DEL_(tokens);
        FREE(row);
    }

    if (sequence != NULL) {
        sequence->writeDelta(delta)->writeEOS();
        patternLength[seqId] = totalDelta;
        player->addSequence(sequence);
        cons->printf(" * Sequence added with %d frames.\n\n", totalDelta);
    }
    //FREE(row);
    DEL_(stream);
#pragma endregion

    // create master sequence
    var masterSequence = NEW_(Sequence, player->masterDevice());
    masterSequence->writeHeader();
    delta = 0;
    totalDelta = 0;
    for (var i = 0; i < patternOrder_.length(); i++) {
        var seqId = *(int*)patternOrder_.get(i);
        masterSequence->writeDelta(delta)->writeCommand(PlayerCommands::CmdAssign)->writeByte(1)->writeByte(seqId)->writeByte(1)->writeByte(0)->writeEOF();
        delta = patternLength[seqId - 1];
        totalDelta += delta;
    }
    masterSequence->writeDelta(delta)->writeEOS();
    player->sequences().insert(0, masterSequence);
    if (totalFrames_ == 0) totalFrames_ = totalDelta;
    var info = masterSequence->print();
    cons->printf("%s", info);
    FREE(info);

    // assign master sequence
    player->assignChannel(0, (Sequence*)player->sequences().get(0), 0, 0);
}

void simpleSynthCallback(short* buffer, int sampleCount, void* context) {
    //((Synth*)context)->run(buffer, 0, sampleCount);
    //for (var i = 0; i < sampleCount; i++) {
    //    SynthTest::buffer_[i] = 0.0f;
    //}
    var synth = (Module*)context;
    synth->run(0, sampleCount);
    for (var i = 0; i < sampleCount; i++) {
        buffer[i] = (short)(0.6f * synth->getOutput(0)[i] * 32768.0f);
    }
}

#pragma region SynthTest
class SynthTest : public Test {
    //Player* player_;
    //Device* device_;

    Stream* createBinaryData();
    void createTestSequence(PlayerDevice* device);
    //Stream* createSynthInitData();
public:
    static float buffer_[4096];
    float samplingRate;

    SynthTest();
    void testEnvelopes();
    void testFilterSound();
    void testGenerateSoundSimple();
    void testGenerateSoundMixer();
    void testCreatePlayerWithDevices();
    void testCreateDrums();
    void testLoadBinary();
    void testCreateBinary();
    void testLoadXm();
    void testDrumLoop();

    void runAll(int&, int&);
};

float SynthTest::buffer_[4096];

SynthTest::SynthTest() {
    //device_ = NULL;
    //player_ = NULL;
    samplingRate = SAMPLING_RATE;
}

void SynthTest::createTestSequence(PlayerDevice* device) {
    var player = device->player();
    var sequence1 = NEW_(Sequence, device);
    { // sequence #1
        sequence1->writeByte(PlayerDevices::DevicePlayer);
        // Frame #1 - 8
        sequence1->writeDelta(0);
        sequence1->writeCommand(PlayerCommands::CmdAssign)->writeByte(1)->writeByte(1)->writeByte(3)->writeByte(0);
        sequence1->writeEOF();
        // Frame #2 - 3
        sequence1->writeDelta(LAST_TICK);
        sequence1->writeEOS();
        player->addSequence(sequence1);
    }
    // 2nd sequence
    var sequence2 = NEW_(Sequence, NULL);
    {
        sequence2->writeByte(SynthDevices::DeviceSynth);
        // Frame #1 - 11
        sequence2->writeDelta(0);
        sequence2->writeByte(ModuleCommands::CmdSetNote)->writeByte(SynthPitch::pC4)->writeByte(180);
        sequence2->writeByte(ModuleCommands::CmdSetNote)->writeByte(SynthPitch::pDs4)->writeByte(80);
        sequence2->writeByte(ModuleCommands::CmdSetNote)->writeByte(SynthPitch::pG4)->writeByte(160);
        sequence2->writeByte(ModuleCommands::CmdSetNote)->writeByte(SynthPitch::pC2)->writeByte(120);
        sequence2->writeEOF();
        // Frame #2 - 11
        sequence2->writeDelta(24);
        sequence2->writeByte(ModuleCommands::CmdSetNote)->writeByte(SynthPitch::pC4)->writeByte(0);
        sequence2->writeByte(ModuleCommands::CmdSetNote)->writeByte(SynthPitch::pDs4)->writeByte(0);
        sequence2->writeByte(ModuleCommands::CmdSetNote)->writeByte(SynthPitch::pG4)->writeByte(0);
        sequence2->writeByte(ModuleCommands::CmdSetNote)->writeByte(SynthPitch::pC2)->writeByte(0);
        sequence2->writeEOF();
        // Frame #3 - 11
        sequence2->writeDelta(40);
        sequence2->writeByte(ModuleCommands::CmdSetNote)->writeByte(SynthPitch::pC4)->writeByte(180);
        sequence2->writeByte(ModuleCommands::CmdSetNote)->writeByte(SynthPitch::pDs4)->writeByte(80);
        sequence2->writeByte(ModuleCommands::CmdSetNote)->writeByte(SynthPitch::pG4)->writeByte(160);
        sequence2->writeByte(ModuleCommands::CmdSetNote)->writeByte(SynthPitch::pC2)->writeByte(120);
        sequence2->writeEOF();
        // Frame #4 - 11
        sequence2->writeDelta(24);
        sequence2->writeByte(ModuleCommands::CmdSetNote)->writeByte(SynthPitch::pC4)->writeByte(0);
        sequence2->writeByte(ModuleCommands::CmdSetNote)->writeByte(SynthPitch::pDs4)->writeByte(0);
        sequence2->writeByte(ModuleCommands::CmdSetNote)->writeByte(SynthPitch::pG4)->writeByte(0);
        sequence2->writeByte(ModuleCommands::CmdSetNote)->writeByte(SynthPitch::pC2)->writeByte(0);
        sequence2->writeEOF();
        // Frame #5 - 11
        sequence2->writeDelta(40);
        sequence2->writeByte(ModuleCommands::CmdSetNote)->writeByte(SynthPitch::pAs3)->writeByte(180);
        sequence2->writeByte(ModuleCommands::CmdSetNote)->writeByte(SynthPitch::pC4)->writeByte(80);
        sequence2->writeByte(ModuleCommands::CmdSetNote)->writeByte(SynthPitch::pF4)->writeByte(160);
        sequence2->writeByte(ModuleCommands::CmdSetNote)->writeByte(SynthPitch::pAs1)->writeByte(120);
        sequence2->writeEOF();
        // Frame #6 - 11
        sequence2->writeDelta(80);
        sequence2->writeByte(ModuleCommands::CmdSetNote)->writeByte(SynthPitch::pAs3)->writeByte(0);
        sequence2->writeByte(ModuleCommands::CmdSetNote)->writeByte(SynthPitch::pC4)->writeByte(0);
        sequence2->writeByte(ModuleCommands::CmdSetNote)->writeByte(SynthPitch::pF4)->writeByte(0);
        sequence2->writeByte(ModuleCommands::CmdSetNote)->writeByte(SynthPitch::pAs1)->writeByte(0);
        sequence2->writeEOF();

        // Frame #3 - 3
        sequence2->writeDelta(48);
        sequence2->writeEOS();
        player->addSequence(sequence2);
    }
}

//Stream* SynthTest::createSynthInitData() {
//    var data = NEW_(Stream);
//    //data->write
//    return data;
//}
//
//Stream* SynthTest::createBinary() {
//    var data = NEW_(Stream);
//    createSequences();
//    var sequence1 = (Sequence*)playerDevice_->sequences().get(0);
//    var sequence2 = (Sequence*)playerDevice_->sequences().get(1);
//
//    #pragma region Datablocks
//    var deviceTypeList = NEW_(Stream);
//    deviceTypeList->writeByte(1);                   // 1 device type
//    deviceTypeList->writeByte(1 + 1 + 1 + 1 + 2);   // length of init data block
//    deviceTypeList->writeByte(DeviceSynth);         // 01 type
//    deviceTypeList->writeByte(6);                   // 02 voice count
//    deviceTypeList->writeByte(1);                   // 03 data block id of sound bank
//    deviceTypeList->writeByte(3);                   // 04 default program
//    deviceTypeList->writeWord(SamplingRate);
//    var soundBank = getSoundBank();
//
//    data->writeWord(3);                             // data block count
//    data->writeDword(deviceTypeList->length());     // length of device type list block
//    data->writeDword(1 + 2 * 2 + sequence1->length() + sequence2->length());    // length of sequence list
//    //data->writeDword(synthInit->length());    // length of data block 1
//    data->writeDword(soundBank->length());          // length of data block 2
//    data->writeFloat(25.0f);                        // fps = 25.0
//    data->writeByte(1);                         // 1 channel
//    // device type list
//    data->writeStream(deviceTypeList);
//    DEL_(deviceTypeList);
//    // sequence list
//    data->writeByte(2);                             // 2 sequences
//    data->writeWord((word)sequence1->length());
//    data->writeWord((word)sequence2->length());
//    // 1st sequence
//    data->writeStream(sequence1);
//    data->writeStream(sequence2);
//    DEL_(sequence1);
//    DEL_(sequence2);
//    // data block #1: init data for SynthDevice
//    // data block #2: sound bank
//    //data->writeStream(synthInit);
//    data->writeStream(soundBank);
//    DEL_(soundBank);
//    #pragma endregion
//
//    return data;
//
//}

//void simpleSoundCallback(short* buffer, int count, void* p) {
//    var elems = (Elem**)p;
//    var flt = (Flt*)elems[0];
//    var osc1 = (Osc*)elems[1];
//    var osc2 = (Osc*)elems[2];
//    var osc3 = (Osc*)elems[3];
//
//    float oscParams[] = { 1.0f, 0.0f, 0.0f };
//    for (var i = 0; i < count; i++) {
//        var smp = osc1->run((float*)&oscParams) + osc2->run((float*)&oscParams) + osc3->run((float*)&oscParams);
//        smp = flt->run(smp);
//        buffer[i] = (short)(smp * 32768.0f);
//    }
//}

void SynthTest::testFilterSound() {
    const float freq = 0.01f * samplingRate - 1.0f;
    const float cut = freq / samplingRate;

#pragma region Prepare_Oscillators
    OscCtrls oscCtrls[3];
    Osc osc[3];
    float fre[3] = { 0.05f * samplingRate, 110.0f, 220.0f };
    Pot note;
    note.value.b = pNone;
    for (var i = 0; i < 3; i++) {
        osc[i].assignControls((PotBase*) & oscCtrls[i]);
        osc[i].setNoteControl(&note);
        osc[i].controls()->amp.value.f = 0.3f / (i + 1);
        osc[i].controls()->fre.value.f = fre[i];
        osc[i].controls()->psw.value.f = 0.5f;
        osc[i].controls()->wave.value.b = WfPulse;
    }

    osc[0].controls()->wave.value.b = WfNoise;
#pragma endregion

#pragma region prepare filters
    const int maxPoles = 6;
    FltCtrls fltControls;
    Flt flts[10];    
    for (var i = 0; i < maxPoles; i++) {
        flts[i].createStages(i+1);
        flts[i].assignControls((PotBase*)&fltControls);
    }
#pragma endregion

#pragma region Tests
    // poles: 1-6
    // filter modes: all, low, band, high
    // cutoff: 0.01, 0.1, 0.5
    // resonance: 0.0, 0.5, 1.0
    byte filterMode[] = { FmAllPass, /*FmLowPass, FmBandPass, FmHighPass */};
    char* filterModeLabel[] = { "AllPass", /*"LowPass", "BandPass", "HighPass" */ };
    float resonance[] = { 0.0f, 0.5f, 0.9f };

    const int duration = 2000;

    Elem* elems[] = { NULL, &osc[0], &osc[1], &osc[2] };
    for (var fmi = 0; fmi < arraysize(filterMode); fmi++) {
        var fm = filterMode[fmi];
        cons->printf("%s\n", filterModeLabel[fmi]);
        for (var pi = 0; pi < maxPoles; pi++) {
            Flt& flt = flts[pi];
            cons->printf(" - %d poles\n", pi + 1);
            for (var ri = 0; ri < arraysize(resonance); ri++) {
                var r = resonance[ri];
                cons->printf("    %.2f resonance\n", r);
                flt.controls()->res.value.f = r;
                // 2.0 * PI * cutoff / samplingRate;
                var df = (2.0 * M_PI * 0.5f);
                flt.controls()->cut.value.f = (float)df;
                flt.controls()->cut.step.f = -(float)(df / (duration * samplingRate / 1000.0f));
                //flt.controls()->mode.value.b = fm;
                elems[0] = &flt;
                SoundPlayer::start((int)samplingRate, 1, 
                    [](short* buffer, int count, void* p) {
                        var elems = (Elem**)p;
                        var flt = (Flt*)elems[0];
                        var osc1 = (Osc*)elems[1];
                        var osc2 = (Osc*)elems[2];
                        var osc3 = (Osc*)elems[3];

                        float oscParams[] = { 1.0f, 0.0f, 0.0f };
                        var f = flt->controls()->cut.value.f;
                        var q = flt->controls()->res.value.f;
                        var st = flt->stage(0);
                        for (var i = 0; i < count; i++) {
                            var smp = osc1->run((float*)&oscParams) + osc2->run((float*)&oscParams) + osc3->run((float*)&oscParams);
                            // filter
                            flt->update(f);
                            smp = flt->run(smp);

                            //var fb = q + q / (1.0f - f);
                            //st->ai_[0] = st->ai_[0] + f * (smp - st->ai_[0] + fb * (st->ai_[0] - st->ai_[1]));
                            //smp = st->ai_[1] = st->ai_[1] + f * (st->ai_[0] - st->ai_[1]);
                            
                            //var b0 = f * f;
                            //st->bi_[0] = st->bi_[2] = b0; st->bi_[1] = 2 * b0;
                            //st->ai_[0] = b0 + 1 - q; st->ai_[2] = b0 + 1 + q;
                            //st->ai_[1] = 2.0f * (b0 - 1);

                            //st->ui_[0] = smp;
                            //smp = (st->bi_[0] * st->ui_[0] + st->bi_[1] * st->ui_[1] + st->bi_[2] * st->ui_[2] - st->ai_[1] * st->lp_[0] - st->ai_[2] * st->lp_[1]) / st->ai_[0];
                            //st->ui_[2] = st->ui_[1]; st->ui_[1] = st->ui_[0];
                            //st->lp_[1] = st->lp_[0]; st->lp_[0] = smp;

                            flt->controls()->cut.value.f += flt->controls()->cut.step.f;
                            buffer[i] = (short)(smp * 32768.0f);
                        }
                    }, &elems);
                Sleep(duration);
                SoundPlayer::stop();
                Sleep(10);
                break;
            }
        }
    }

    //flts[i].update(cut);
    //fltControls.cut.value.b = 0;
    //fltControls.res.value.f = 0.0f;
    //fltControls.mod.value.f = 0.0f;
    //
    //void* fltModes[] = {
    //    "AllPass", (void*)FmAllPass,
    //    "LowPass", (void*)FmLowPass,
    //    "BandPass", (void*)FmBandPass,
    //    "HighPass", (void*)FmHighPass
    //};
    //Elem* elems[] = { NULL, &osc1, &osc2, &osc3 };
    //for (var m = 0; m < ARRAYSIZE(fltModes);) {
    //    var txt = fltModes[m++];
    //    fltControls.mode.value.b = *(byte*)&fltModes[m++];
    //    for (var i = minPole; i < maxPoles; i++) {
    //        LOG(" - %d pole %s\n", i+1, txt);
    //        elems[0] = flts[i];
    //        SoundPlayer::start((int)samplingRate, 1, simpleSoundCallback, &elems);
    //        var toggle = i % 2;
    //    osc1Controls.amp.value.f = 0.5f * (float)toggle;
    //    osc2Controls.amp.value.f = 0.5f * (float)(1 - toggle);
    //        Sleep(600);
    //        SoundPlayer::stop();
    //        Sleep(100);
    //    }
    //}
#pragma endregion

    //var buffer = MALLOC(short, samplingRate);
    //var wave = NEW_(WaveFmt, "noise.wav", (int)samplingRate, 2, 16);
    //elems[0] = flts[2];
    //for (var i=0; i<8; i++) {
    //    simpleSoundCallback(buffer, 8192, &elems);
    //    wave->write((byte*)buffer, 2*8192);
    //}
    //wave->close();
    //FREE(buffer);
    //DEL_(wave);
}

void SynthTest::testEnvelopes() {
    Env::initialize();
    // 20 secs buffer
    var buffer = MALLOC(short, 20*samplingRate);
    Adsr adsr;
    AdsrCtrls adsrCtrls;
    byte adsrMinSettings[] = { DF(1.0f), DF(0.0f), DB(0), DB(0), DB(128), DB(0) };
    byte adsrMaxSettings[] = { DF(1.0f), DF(0.0f), DB(255), DB(255), DB(128), DB(255) };
    Dahr dahr;
    DahrCtrls dahrCtrls;
    byte dahrMinSettings[] = { DF(1.0f), DF(0.0f), DB(0), DB(0), DB(0), DB(0) };
    byte dahrMaxSettings[] = { DF(1.0f), DF(0.0f), DB(255), DB(255), DB(255), DB(255) };

    adsr.assignControls((PotBase*)&adsrCtrls);
    dahr.assignControls((PotBase*)&dahrCtrls);

    byte* s = NULL;
    WaveFmt* wave = NULL;
    var si = 0;
    int noteOff = 0;

#pragma region ADSR
    // save ADSR with min settings: 1ms Atk, 1ms Dec, 100ms Sus, 1ms Rel
    s = adsrMinSettings; adsr.setFromStream(s);
    noteOff = (int)ceil(1 / Env::attackRates[adsr.controls()->atk.value.b] + 1 / Env::attackRates[adsr.controls()->dec.value.b] + 0.100 * samplingRate);
    wave = NEW_(WaveFmt, "adsr-min.wav", (int)samplingRate, 1, 16);
    adsr.setGate(255);
    while (si < 20*samplingRate) {
        var smp = adsr.run();
        if (!adsr.isActive()) break;
        buffer[si++] = (short)(smp * 32767.0f);
        if (si == noteOff) adsr.setGate(0);
    }
    wave->write((byte*)buffer, 2 * si);
    wave->close();
    DEL_(wave);
    assert("Should create minimal ADSR", si == 4815);   // != 5+5+4800+5 = 4815

    // save ADSR with max settings
    si = 0;
    s = adsrMaxSettings; adsr.setFromStream(s);
    noteOff = (int)ceil(1 / Env::attackRates[adsr.controls()->atk.value.b] + 1 / Env::attackRates[adsr.controls()->dec.value.b] + 0.100 * samplingRate);
    wave = NEW_(WaveFmt, "adsr-max.wav", (int)samplingRate, 1, 16);
    adsr.setGate(255);
    while (si < 20 * samplingRate) {
        var smp = adsr.run();
        if (!adsr.isActive()) break;
        buffer[si++] = (short)(smp * 32767.0f);
        if (si == noteOff) adsr.setGate(0);
    }
    wave->write((byte*)buffer, 2 * si);
    wave->close();
    DEL_(wave);
    assert("Should create maximal ADSR", si == 772801); // != 192000+192000+4800+384000 = 772800

    // save ADSR with 0.8 sustain
    si = 0;
    adsr.controls()->sus.value.f = 0.8f;
    noteOff = (int)ceil(1 / Env::attackRates[adsr.controls()->atk.value.b] + 1 / Env::attackRates[adsr.controls()->dec.value.b] + 0.100 * samplingRate);
    wave = NEW_(WaveFmt, "adsr-max08.wav", (int)samplingRate, 1, 16);
    adsr.setGate(255);
    while (si < 20*samplingRate) {
        var smp = adsr.run();
        if (!adsr.isActive()) break;
        buffer[si++] = (short)(smp * 32767.0f);
        if (si == noteOff) adsr.setGate(0);
    }
    wave->write((byte*)buffer, 2 * si);
    wave->close();
    DEL_(wave);
    assert("Should create maximal ADSR", si == 772801); // != 772801
#pragma endregion

#pragma region DAHR
    // save DAHR with min settings
    si = 0;
    s = dahrMinSettings; dahr.setFromStream(s);
    wave = NEW_(WaveFmt, "dahr-min.wav", (int)samplingRate, 1, 16);
    dahr.setGate(255);
    while (si < 20 * samplingRate) {
        var smp = dahr.run();
        if (!dahr.isActive()) break;
        buffer[si++] = (short)(smp * 32767.0f);
    }
    wave->write((byte*)buffer, 2 * si);
    wave->close();
    DEL_(wave);
    assert("Should create minimal DAHR", si == 19); // != 4+5+5+5 = 96
   
    // save DAHR with max settings
    si = 0;
    s = dahrMaxSettings; dahr.setFromStream(s);
    wave = NEW_(WaveFmt, "dahr-max.wav", (int)samplingRate, 1, 16);
    dahr.setGate(255);
    while (si < 20 * samplingRate) {
        var smp = dahr.run();
        if (!dahr.isActive()) break;
        buffer[si++] = (short)(smp * 32767.0f);
    }
    wave->write((byte*)buffer, 2 * si);
    wave->close();
    DEL_(wave);
    assert("Should create maximal DAHR", si == 960000); // != 192000+192000+192000+384000
#pragma endregion

    FREE(buffer);
}

void SynthTest::testGenerateSoundSimple() {
    const int note1 = SynthPitch::pD5;
    const int note2 = SynthPitch::pFs4;
    const int note3 = SynthPitch::pA4;
    const int note4 = SynthPitch::pD2;
    //Mdl::createFrequencyTable();
    synthAdapter_.prepare();
    var synth = NEW_(Synth, &samplingRate, 4);
    synth->setProgram(3);

    if (SoundPlayer::start((int)samplingRate, 1, simpleSynthCallback, synth) == 0) {
        for (var i = 0; i < 2; i++) {
            synth->setNote(note1, 124);
            synth->setNote(note2, 84);
            synth->setNote(note3, 114);
            synth->setNote(note4, 74);
            Sleep(1200);
            synth->setNote(note1, 0);
            synth->setNote(note2, 0);
            synth->setNote(note3, 0);
            synth->setNote(note4, 0);
            Sleep(800);
        }
        Sleep(1000);
        SoundPlayer::stop();
    }

    assert("Should create sound", synth->isActive());
    DEL_(synth);
    synthAdapter_.cleanUp();
}

void SynthTest::testGenerateSoundMixer() {
    const int note1 = SynthPitch::pFs3;
    const int note2 = SynthPitch::pA3;
    const int note3 = SynthPitch::pD4;
    const int note4 = SynthPitch::pD1;

    synthAdapter_.prepare();
    #pragma region Devices
    var synth = NEW_(Synth, &samplingRate, 1);
    synth->voiceCount(9);
    synth->setProgram(3);

    var bass = NEW_(Bass, &samplingRate, 1);
    bass->setProgram(0);

    var clip1 = NEW_(Distort, &samplingRate);
    var mixer = NEW_(Mixer8x4);
        mixer->channelCount(2);
    #pragma endregion

    // channel 1
    mixer->connectInput(mixer->getChannel(0), synth);
    mixer->getControl(MxCh1gain)->value.f = 1.0f;
    mixer->getControl(MxCh1pan)->value.f = 0.2f;
    mixer->getControl(MxCh1amp)->value.f = 0.3f;

    // channel 2
    var ch2 = mixer->getChannel(1);
    mixer->connectInput(ch2, bass);
    mixer->getControl(MxCh2gain)->value.f = 0.2f;
    mixer->getControl(MxCh2pan)->value.f = 0.7f;
    mixer->getControl(MxCh2amp)->value.f = 0.6f;
        mixer->getControl(MxCh2st1gain)->value.f = 0.5f;
        mixer->connectEffect(ch2, clip1);
        clip1->controls.amp.value.f = 3.5f;
        clip1->controls.lvl.value.f = 0.8f;
        clip1->controls.cut.value.b = 10;
        clip1->controls.res.value.f = 0.4f;
        clip1->controls.mode.value.b = FmLowPass || FmBandPass;
        clip1->updateFilter();


    if (SoundPlayer::start((int)samplingRate, 2, Mixer8x4::fillSoundBuffer, mixer) == 0) {
        for (var i = 0; i < 2; i++) {
            synth->setNote(note1, 224);
            synth->setNote(note2, 124);
            synth->setNote(note3, 184);
            bass->setNote(note4, 184);
            Sleep(2400);
            synth->setNote(note1, 0);
            synth->setNote(note2, 0);
            synth->setNote(note3, 0);
            bass->setNote(note4, 0);
            Sleep(1200);
        }
        Sleep(1000);
        SoundPlayer::stop();
    }

    assert("Should create sound", synth->isActive());

    DEL_(synth);
    DEL_(bass);
    DEL_(clip1);
    DEL_(mixer);

    synthAdapter_.cleanUp();
}

void SynthTest::testCreatePlayerWithDevices() {
    var device = PlayerDevice::create(NULL);
    var player = device->player();
    var mainDevice = player->masterDevice();
    var adapter = player->addAdapter(NEW_(SynthAdapter));
    SynthDevice* synthDevices[4];
    int channelCount = arraysize(synthDevices);
    synthDevices[0] = (SynthDevice*)player->addDevice(adapter, SynthDevices::DeviceSynth);
    synthDevices[1] = (SynthDevice*)player->addDevice(adapter, SynthDevices::DeviceSynth);
    synthDevices[1]->synth()->voiceCount(6);
    synthDevices[2] = (SynthDevice*)player->addDevice(adapter, SynthDevices::DeviceSynth);
    synthDevices[3] = (SynthDevice*)player->addDevice(adapter, SynthDevices::DeviceSynth);
    synthDevices[3]->synth()->voiceCount(6);
    var mixerDevice = (MixerDevice*)player->addDevice(adapter, SynthDevices::DeviceMixer);
    assert("Should have 6 devices", player->devices().length() == 6);
    var mixer = (Mixer8x4*)mixerDevice->object();
    mixer->channelCount(channelCount);
    
    float amp[] = { 0.8f, 0.4f, 0.5f, 0.4f};
    float pan[] = { 0.5f, 0.6f, 0.2f, 0.8f };
    for (var i = 0; i < channelCount; i++) {
        var ch = mixer->getChannel(i);
        mixer->connectInput(ch, synthDevices[i]->synth());
        ch->controls->gain.value.f = 1.0f;
        ch->controls->amp.value.f = amp[i];
        ch->controls->pan.value.f = pan[i];
        synthDevices[i]->setProgram(i);
    }
    //mixer->getChannel(4)->controls->amp.value.f = 0.4f;

    if (SoundPlayer::start((int)samplingRate, 2, Mixer8x4::fillSoundBuffer, mixer) == 0) {
        for (var i = 0; i < 4; i++) {
            synthDevices[0]->synth()->setNote(pC2, 80);     // 1.1
            synthDevices[1]->synth()->setNote(pC4, 60);
            synthDevices[1]->synth()->setNote(pE4, 50);
            synthDevices[1]->synth()->setNote(pG4, 40);
            synthDevices[2]->synth()->setNote(pC2, 80);
            synthDevices[3]->synth()->setNote(pC4, 180);
            Sleep(100);

            synthDevices[2]->synth()->setNote(pC2,  0);     // 1.2
            synthDevices[3]->synth()->setNote(pC4,  0);
            Sleep(100);

            synthDevices[0]->synth()->setNote(pC2, 0);      // 2.1
            synthDevices[2]->synth()->setNote(pC2, 80);
            Sleep(100);

            synthDevices[2]->synth()->setNote(pC2, 0);      // 2.2
            Sleep(100);

            synthDevices[0]->synth()->setNote(pC2, 60);     // 3.1
            synthDevices[1]->synth()->setNote(pC4, 0);
            synthDevices[1]->synth()->setNote(pE4, 0);
            synthDevices[1]->synth()->setNote(pG4, 0);
            synthDevices[2]->synth()->setNote(pE2, 80);
            synthDevices[3]->synth()->setNote(pG3, 200);
            Sleep(100);

            synthDevices[2]->synth()->setNote(pE2, 0);      // 3.2
            synthDevices[3]->synth()->setNote(pG3, 0);
            Sleep(100);

            synthDevices[0]->synth()->setNote(pC2, 0);      // 4.1
            synthDevices[2]->synth()->setNote(pC3, 80);
            synthDevices[3]->synth()->setNote(pC4, 180);
            Sleep(100);

            synthDevices[2]->synth()->setNote(pC3, 0);      // 4.2
            synthDevices[3]->synth()->setNote(pC4, 0);
            Sleep(100);

            synthDevices[0]->synth()->setNote(pAs1, 0);     // 5.1
            synthDevices[2]->synth()->setNote(pC2, 80);
            Sleep(100);

            synthDevices[2]->synth()->setNote(pC2, 0);      // 5.2
            Sleep(100);

            synthDevices[0]->synth()->setNote(pAs1, 80);    // 6.1
            synthDevices[2]->synth()->setNote(pC2, 80);
            synthDevices[3]->synth()->setNote(pG3, 200);
            Sleep(100);

            synthDevices[2]->synth()->setNote(pC2, 0);      // 6.2
            synthDevices[3]->synth()->setNote(pG3, 0);
            Sleep(100);

            synthDevices[0]->synth()->setNote(pG1, 80);     // 7.1
            synthDevices[2]->synth()->setNote(pE2, 80);
            synthDevices[3]->synth()->setNote(pE4, 170);
            Sleep(100);

            synthDevices[2]->synth()->setNote(pE2, 0);      // 7.2
            Sleep(100);

            synthDevices[0]->synth()->setNote(pG1, 0);      // 8.1
            synthDevices[2]->synth()->setNote(pC3, 80);
            synthDevices[3]->synth()->setNote(pE4, 0);
            Sleep(100);

            synthDevices[2]->synth()->setNote(pC3, 0);      // 8.2
            Sleep(100);
        }
        SoundPlayer::stop();
    }

    DEL_(device);
    Player::cleanUp();
}

void SynthTest::testCreateDrums() {
    var device = PlayerDevice::create(NULL);
    var player = device->player();
    var mainDevice = player->masterDevice();
    var adapter = player->addAdapter(NEW_(SynthAdapter));
    var drumsDevice = (DrumsDevice*)player->addDevice(adapter, SynthDevices::DeviceDrums);
    var drums = drumsDevice->drums();

    GenericDrum& bd = drums->drums[0];
    assert("Should have a kick drum", bd.controls.fre[0].value.f != 0.0f);

    drums->drums[0].setProgram(1);
    drums->drums[1].setProgram(2);
    drums->drums[2].setProgram(3);
    drums->drums[3].setProgram(5);
    drums->drums[4].setProgram(7);
    drums->drums[5].setProgram(8);
    drums->drums[6].setProgram(9);
    drums->drums[7].setProgram(10);

    drums->drums[0].getControl(gdDahr1Amp)->value.f = 0.8f;
    //drums->drums[1].getControl(gdDahr1Amp)->value.f = 0.0f;
    drums->drums[2].getControl(gdDahr1Amp)->value.f = 0.6f;
    drums->drums[3].getControl(gdDahr1Amp)->value.f = 0.6f;
    drums->drums[3].getControl(gdDahr1Rel)->value.b = 24;
    drums->drums[4].getControl(gdDahr1Amp)->value.f = 1.0f;
    drums->drums[5].getControl(gdDahr1Amp)->value.f = 0.8f;
    drums->drums[6].getControl(gdDahr1Amp)->value.f = 0.9f;

    if (isSaveToWave_) {
        var wave = NEW_(WaveFmt, "drums.wav", (int)samplingRate, 1, 16);
        var length = 2 * (int)(8.0f * 1.0f * samplingRate);
        var buffer = MALLOC(short, length);
        memset(buffer, 0, length);
        wave->write((byte*)buffer, SAMPLE_BUFFER_SIZE);
        //var output = drums->getOutput(0);
        var count = 0;
        int frame = length / 8 / 8;
        var p = buffer;
        do {
            var p = buffer;

            drums->setNote(drBD, 240);
            drums->setNote(drCH, 200);
            saveToBuffer(drums, p, frame);

            drums->setNote(drOH, 200);
            drums->setNote(drLT, 120);
            saveToBuffer(drums, p, frame);

            drums->setNote(drBD, 240);
            drums->setNote(drCH, 200);
            drums->setNote(drCP, 80);
            saveToBuffer(drums, p, frame);

            drums->setNote(drOH, 200);
            drums->setNote(drHT, 120);
            saveToBuffer(drums, p, frame);

            drums->setNote(drBD, 240);
            drums->setNote(drCH, 200);
            saveToBuffer(drums, p, frame);

            drums->setNote(drOH, 200);
            drums->setNote(drLT, 120);
            saveToBuffer(drums, p, frame);

            drums->setNote(drBD, 240);
            drums->setNote(drCH, 200);
            drums->setNote(drCP, 80);
            saveToBuffer(drums, p, frame);

            drums->setNote(drOH, 200);
            drums->setNote(drHT, 120);
            saveToBuffer(drums, p, frame >> 1);

            drums->setNote(drMT, 120);
            saveToBuffer(drums, p, frame >> 1);

            wave->write((byte*)buffer, 2 * (int)(p - buffer));
        } while (count++ < 7);
        wave->close();
        DEL_(wave);
        FREE(buffer);
    }
    else {
        var count = 0;
        if (SoundPlayer::start((int)samplingRate, 1, simpleSynthCallback, drums) == 0) {
            var frame = SAMPLE_BUFFER_SIZE;
            do {
                drums->setNote(drBD, 200);
                drums->setNote(drCH, 100);
                Sleep(240);

                drums->setNote(drCH, 200);
                drums->setNote(drLT, 100);
                Sleep(240);

                drums->setNote(drBD, 200);
                drums->setNote(drCH, 100);
                drums->setNote(drCP, 180);
                Sleep(240);

                drums->setNote(drCH, 200);
                drums->setNote(drHT, 100);
                Sleep(240);

                drums->setNote(drBD, 200);
                drums->setNote(drCH, 100);
                Sleep(240);

                drums->setNote(drCH, 200);
                drums->setNote(drLT, 100);
                Sleep(240);

                drums->setNote(drBD, 200);
                drums->setNote(drCH, 100);
                drums->setNote(drCP, 180);
                Sleep(240);

                drums->setNote(drCH, 200);
                drums->setNote(drHT, 100);
                Sleep(120);

                drums->setNote(drMT, 100);
                Sleep(120);

            } while (++count < 4);
            Sleep(240);
            SoundPlayer::stop();
        }
    }

    DEL_(device);
    Player::cleanUp();
}

Stream* SynthTest::createBinaryData() {
    var data = NEW_(Stream);
    PlayerDevice playerDevice(NULL, &playerAdapter_);
    SynthDevice synthDevice(&synthAdapter_);
    BassDevice bassDevice(&synthAdapter_);

    var initData = NEW_(Stream);
    initData->writeFloat(32.0f);
    // 5 channels
    initData->writeByte(5);

    #pragma region Sequences
    var steps = 64;
    //#region MASTER sequence - 12
    // k kbh kbhc kbhcm kbhcm kbhm kbhm
    var sequence1 = NEW_(Sequence, &playerDevice);
    {
        //    0 1 2 3 4 5 6 7 8 9 A B C D E F 
        // dr x x x x x x x x         x x x x    // 0|8   12|4
        // bs     x x x x x x     x x x x        // 2|6   10|4
        // hh     x x x x x x x x                // 2|8
        // cd         x x x x x x x x            // 4|8
        // mn             x x x x     x x x x    // 6|4   12|2
        sequence1->writeHeader();

        sequence1->writeDelta(0);           // 0
        sequence1->writeCommand(PlayerCommands::CmdAssign)->writeByte(1)->writeByte(1)->writeByte(1)->writeByte(2);
        sequence1->writeEOF();

        sequence1->writeDelta(2*steps);     // 2
        sequence1->writeCommand(PlayerCommands::CmdAssign)->writeByte(2)->writeByte(2)->writeByte(2)->writeByte(6);
        sequence1->writeCommand(PlayerCommands::CmdAssign)->writeByte(3)->writeByte(3)->writeByte(1)->writeByte(8);
        sequence1->writeEOF();

        sequence1->writeDelta(2*steps);     // 4
        sequence1->writeCommand(PlayerCommands::CmdAssign)->writeByte(4)->writeByte(4)->writeByte(3)->writeByte(4);
        sequence1->writeEOF();

        sequence1->writeDelta(2*steps);     // 6
        sequence1->writeCommand(PlayerCommands::CmdAssign)->writeByte(5)->writeByte(5)->writeByte(4)->writeByte(4);
        sequence1->writeEOF();

        sequence1->writeDelta(4*steps);     // 10
        sequence1->writeCommand(PlayerCommands::CmdAssign)->writeByte(2)->writeByte(2)->writeByte(2)->writeByte(4);
        sequence1->writeEOF();

        sequence1->writeDelta(2 * steps);   // 12
        sequence1->writeCommand(PlayerCommands::CmdAssign)->writeByte(1)->writeByte(1)->writeByte(1)->writeByte(1);
        sequence1->writeCommand(PlayerCommands::CmdAssign)->writeByte(5)->writeByte(5)->writeByte(4)->writeByte(4);
        sequence1->writeEOF();

        sequence1->writeDelta(5*steps);
        sequence1->writeEOS();
    }
    //#region Sequence #2 - 256st "drums"
    var sequence2 = NEW_(Sequence, &synthDevice);
    {
        sequence2->writeHeader();
        sequence2->writeDelta( 0)->writeCommand(CmdSetNote)->writeByte(drBD)->writeByte(208)->writeEOF();    //  0  0
        sequence2->writeDelta(16)->writeCommand(CmdSetNote)->writeByte(drSD)->writeByte(255)->writeEOF();    //  4 16
        sequence2->writeDelta( 8)->writeCommand(CmdSetNote)->writeByte(drBD)->writeByte(208)->writeEOF();    //  6  8
        sequence2->writeDelta(16)->writeCommand(CmdSetNote)->writeByte(drBD)->writeByte(208)->writeEOF();    // 10 16
        sequence2->writeDelta( 8)->writeCommand(CmdSetNote)->writeByte(drSD)->writeByte(255)->writeEOF();    // 12  8
        sequence2->writeDelta( 8)->writeCommand(CmdSetNote)->writeByte(drBD)->writeByte(120)->writeEOF();    // 14  8
        sequence2->writeDelta( 8)->writeCommand(CmdSetNote)->writeByte(drBD)->writeByte(208)->writeEOF();    // 16  8
        sequence2->writeDelta(16)->writeCommand(CmdSetNote)->writeByte(drSD)->writeByte(255)->writeEOF();    // 20 16
        sequence2->writeDelta(24)->writeCommand(CmdSetNote)->writeByte(drBD)->writeByte(208)->writeEOF();    // 26 24
        sequence2->writeDelta( 8)->writeCommand(CmdSetNote)->writeByte(drSD)->writeByte(255)->writeEOF();    // 28  8
        sequence2->writeDelta(16)->writeCommand(CmdSetNote)->writeByte(drBD)->writeByte(208)->writeEOF();    // 32 16
        sequence2->writeDelta(16)->writeCommand(CmdSetNote)->writeByte(drSD)->writeByte(255)->writeEOF();    // 36 16
        sequence2->writeDelta( 8)->writeCommand(CmdSetNote)->writeByte(drBD)->writeByte(208)->writeEOF();    // 38  8
        sequence2->writeDelta(16)->writeCommand(CmdSetNote)->writeByte(drBD)->writeByte(208)->writeEOF();    // 42 16
        sequence2->writeDelta( 8)->writeCommand(CmdSetNote)->writeByte(drSD)->writeByte(255)->writeEOF();    // 44  8
        sequence2->writeDelta(16)->writeCommand(CmdSetNote)->writeByte(drBD)->writeByte(208)->writeEOF();    // 48 16
        sequence2->writeDelta(16)->writeCommand(CmdSetNote)->writeByte(drSD)->writeByte(255)->writeEOF();    // 52 16
        sequence2->writeDelta(24)->writeCommand(CmdSetNote)->writeByte(drBD)->writeByte(208)->writeEOF();    // 58 24
        sequence2->writeDelta( 8)->writeCommand(CmdSetNote)->writeByte(drSD)->writeByte(255)->writeEOF();    // 60  8
        sequence2->writeDelta( 8)->writeCommand(CmdSetNote)->writeByte(drSD)->writeByte(255)->writeEOF();    // 62  8
        sequence2->writeDelta( 4)->writeCommand(CmdSetNote)->writeByte(drSD)->writeByte(255)->writeEOF();    // 63  4

        sequence2->writeDelta( 4)->writeEOS();
    }
    //#region Sequence #3 - 64st "bass"
    var sequence3 = NEW_(Sequence, &bassDevice);
    {
        sequence3->writeHeader();
        sequence3->writeDelta(0);
        sequence3->writeCommand(CmdSetProgram)->writeByte(0);
        sequence3->writeCommand(CmdSetNote)->writeByte(pD1)->writeByte(128);
        sequence3->writeEOF();
        sequence3->writeDelta(4);
        sequence3->writeCommand(CmdSetNote)->writeByte(pD1)->writeByte(0);
        sequence3->writeEOF();

        sequence3->writeDelta(4);
        sequence3->writeCommand(CmdSetNote)->writeByte(pD2)->writeByte(128);
        sequence3->writeEOF();
        sequence3->writeDelta(4);
        sequence3->writeCommand(CmdSetNote)->writeByte(pD2)->writeByte(0);
        sequence3->writeEOF();
    
        sequence3->writeDelta(4);
        sequence3->writeCommand(CmdSetNote)->writeByte(pF1)->writeByte(128);
        sequence3->writeEOF();
        sequence3->writeDelta(4);
        sequence3->writeCommand(CmdSetNote)->writeByte(pF1)->writeByte(0);
        sequence3->writeEOF();

        sequence3->writeDelta(4);
        sequence3->writeCommand(CmdSetNote)->writeByte(pF2)->writeByte(128);
        sequence3->writeEOF();
        sequence3->writeDelta(4);
        sequence3->writeCommand(CmdSetNote)->writeByte(pF2)->writeByte(0);
        sequence3->writeEOF();

        sequence3->writeDelta(4);
        sequence3->writeCommand(CmdSetNote)->writeByte(pC1)->writeByte(128);
        sequence3->writeEOF();
        sequence3->writeDelta(4);
        sequence3->writeCommand(CmdSetNote)->writeByte(pC1)->writeByte(0);
        sequence3->writeEOF();

        sequence3->writeDelta(4);
        sequence3->writeCommand(CmdSetNote)->writeByte(pC2)->writeByte(128);
        sequence3->writeEOF();
        sequence3->writeDelta(4);
        sequence3->writeCommand(CmdSetNote)->writeByte(pC2)->writeByte(0);
        sequence3->writeEOF();

        sequence3->writeDelta(4);
        sequence3->writeCommand(CmdSetNote)->writeByte(pG1)->writeByte(128);
        sequence3->writeEOF();
        sequence3->writeDelta(4);
        sequence3->writeCommand(CmdSetNote)->writeByte(pG1)->writeByte(0);
        sequence3->writeEOF();

        sequence3->writeDelta(4);
        sequence3->writeCommand(CmdSetNote)->writeByte(pF2)->writeByte(128);
        sequence3->writeEOF();
        sequence3->writeDelta(4);
        sequence3->writeCommand(CmdSetNote)->writeByte(pF2)->writeByte(0);
        sequence3->writeEOF();

        sequence3->writeDelta(4);
        sequence3->writeEOS();
    }
    //#region Sequence #4 - 64st "hihat"
    var sequence4 = NEW_(Sequence, &synthDevice);
    {
        sequence4->writeHeader();
        // 0               1               2               3
        // 0...1...2...3...0...1...2...3...0...1...2...3...0...1...2...3...
        // c.......oooo....c.......ooo...c.....c...ooo.c...c.......ooo.c...
        // 01      74      41      73    31    51  33  11  31      73  11  3
        // #0.0
        sequence4->writeDelta(0)->writeCommand(CmdSetNote)->writeByte(drCH)->writeByte(180)->writeEOF();
        sequence4->writeDelta(8)->writeCommand(CmdSetNote)->writeByte(drOH)->writeByte(140)->writeEOF();
        sequence4->writeDelta(8)->writeCommand(CmdSetNote)->writeByte(drCH)->writeByte(180)->writeEOF();
        sequence4->writeDelta(8)->writeCommand(CmdSetNote)->writeByte(drOH)->writeByte(140)->writeEOF();
        sequence4->writeDelta(5)->writeCommand(CmdSetNote)->writeByte(drCH)->writeByte(160)->writeEOF();
        sequence4->writeDelta(7)->writeCommand(CmdSetNote)->writeByte(drCH)->writeByte(180)->writeEOF();
        sequence4->writeDelta(4)->writeCommand(CmdSetNote)->writeByte(drOH)->writeByte(140)->writeEOF();
        sequence4->writeDelta(4)->writeCommand(CmdSetNote)->writeByte(drCH)->writeByte(160)->writeEOF();
        sequence4->writeDelta(4)->writeCommand(CmdSetNote)->writeByte(drCH)->writeByte(180)->writeEOF();
        sequence4->writeDelta(8)->writeCommand(CmdSetNote)->writeByte(drOH)->writeByte(140)->writeEOF();
        sequence4->writeDelta(4)->writeCommand(CmdSetNote)->writeByte(drCH)->writeByte(170)->writeEOF();
        sequence4->writeDelta(4);
        
        sequence4->writeEOS();
    }
    //#region Sequence #5 - 128st "chords"
    var sequence5 = NEW_(Sequence, &synthDevice);
    {
        sequence5->writeHeader();
        // Frame #1.1
        sequence5->writeDelta(0);
        sequence5->writeCommand(CmdSetNote)->writeByte(pD4)->writeByte(128);
        sequence5->writeCommand(CmdSetNote)->writeByte(pF3)->writeByte(128);
        sequence5->writeCommand(CmdSetNote)->writeByte(pA3)->writeByte(128);
        sequence5->writeEOF();
        sequence5->writeDelta(8);
        sequence5->writeCommand(CmdSetNote)->writeByte(pD4)->writeByte(0);
        sequence5->writeCommand(CmdSetNote)->writeByte(pF3)->writeByte(0);
        sequence5->writeCommand(CmdSetNote)->writeByte(pA3)->writeByte(0);
        sequence5->writeEOF();
        sequence5->writeDelta(24);
        sequence5->writeCommand(CmdSetNote)->writeByte(pC4)->writeByte(128);
        sequence5->writeCommand(CmdSetNote)->writeByte(pF3)->writeByte(128);
        sequence5->writeCommand(CmdSetNote)->writeByte(pA3)->writeByte(128);
        sequence5->writeEOF();
        sequence5->writeDelta(8);
        sequence5->writeCommand(CmdSetNote)->writeByte(pC4)->writeByte(0);
        sequence5->writeCommand(CmdSetNote)->writeByte(pF3)->writeByte(0);
        sequence5->writeCommand(CmdSetNote)->writeByte(pA3)->writeByte(0);
        sequence5->writeEOF();
        sequence5->writeDelta(24);
        sequence5->writeCommand(CmdSetNote)->writeByte(pC4)->writeByte(128);
        sequence5->writeCommand(CmdSetNote)->writeByte(pE3)->writeByte(128);
        sequence5->writeCommand(CmdSetNote)->writeByte(pG3)->writeByte(128);
        sequence5->writeEOF();
        sequence5->writeDelta(8);
        sequence5->writeCommand(CmdSetNote)->writeByte(pC4)->writeByte(0);
        sequence5->writeCommand(CmdSetNote)->writeByte(pE3)->writeByte(0);
        sequence5->writeCommand(CmdSetNote)->writeByte(pG3)->writeByte(0);
        sequence5->writeEOF();
        sequence5->writeDelta(24);
        sequence5->writeCommand(CmdSetNote)->writeByte(pD4)->writeByte(128);
        sequence5->writeCommand(CmdSetNote)->writeByte(pG3)->writeByte(128);
        sequence5->writeCommand(CmdSetNote)->writeByte(pH3)->writeByte(128);
        sequence5->writeEOF();
        sequence5->writeDelta(8);
        sequence5->writeCommand(CmdSetNote)->writeByte(pD4)->writeByte(0);
        sequence5->writeCommand(CmdSetNote)->writeByte(pG3)->writeByte(0);
        sequence5->writeCommand(CmdSetNote)->writeByte(pH3)->writeByte(0);
        sequence5->writeEOF();
        sequence5->writeDelta(24);
        sequence5->writeEOS();
    }
    //#region Sequence #6 - 64st "mono"
    var sequence6 = NEW_(Sequence, &synthDevice);
    {
        sequence6->writeHeader();
        // #1
        sequence6->writeDelta(0)->writeCommand(CmdSetNote)->writeByte(pD2)->writeByte(144)->writeEOF();
        sequence6->writeDelta(2)->writeCommand(CmdSetNote)->writeByte(pD2)->writeByte(0)->writeEOF();

        sequence6->writeDelta(2)->writeCommand(CmdSetNote)->writeByte(pD3)->writeByte(144)->writeEOF();
        sequence6->writeDelta(2)->writeCommand(CmdSetNote)->writeByte(pD3)->writeByte(0)->writeEOF();

        sequence6->writeDelta(2)->writeCommand(CmdSetNote)->writeByte(pD2)->writeByte(144)->writeEOF();
        sequence6->writeDelta(2)->writeCommand(CmdSetNote)->writeByte(pD2)->writeByte(0)->writeEOF();

        sequence6->writeDelta(2)->writeCommand(CmdSetNote)->writeByte(pD2)->writeByte(144)->writeEOF();
        sequence6->writeDelta(2)->writeCommand(CmdSetNote)->writeByte(pD2)->writeByte(0)->writeEOF();

        // #2
        sequence6->writeDelta(2)->writeCommand(CmdSetNote)->writeByte(pF3)->writeByte(144)->writeEOF();
        sequence6->writeDelta(2)->writeCommand(CmdSetNote)->writeByte(pF3)->writeByte(0)->writeEOF();

        sequence6->writeDelta(2)->writeCommand(CmdSetNote)->writeByte(pF2)->writeByte(144)->writeEOF();
        sequence6->writeDelta(2)->writeCommand(CmdSetNote)->writeByte(pF2)->writeByte(0)->writeEOF();

        sequence6->writeDelta(2)->writeCommand(CmdSetNote)->writeByte(pF2)->writeByte(144)->writeEOF();
        sequence6->writeDelta(2)->writeCommand(CmdSetNote)->writeByte(pF2)->writeByte(0)->writeEOF();

        sequence6->writeDelta(2)->writeCommand(CmdSetNote)->writeByte(pF3)->writeByte(144)->writeEOF();
        sequence6->writeDelta(2)->writeCommand(CmdSetNote)->writeByte(pF3)->writeByte(0)->writeEOF();

        // #3
        sequence6->writeDelta(2)->writeCommand(CmdSetNote)->writeByte(pC3)->writeByte(144)->writeEOF();
        sequence6->writeDelta(2)->writeCommand(CmdSetNote)->writeByte(pC3)->writeByte(0)->writeEOF();

        sequence6->writeDelta(2)->writeCommand(CmdSetNote)->writeByte(pC2)->writeByte(144)->writeEOF();
        sequence6->writeDelta(2)->writeCommand(CmdSetNote)->writeByte(pC2)->writeByte(0)->writeEOF();

        sequence6->writeDelta(2)->writeCommand(CmdSetNote)->writeByte(pC2)->writeByte(144)->writeEOF();
        sequence6->writeDelta(2)->writeCommand(CmdSetNote)->writeByte(pC2)->writeByte(0)->writeEOF();

        sequence6->writeDelta(2)->writeCommand(CmdSetNote)->writeByte(pC3)->writeByte(144)->writeEOF();
        sequence6->writeDelta(2)->writeCommand(CmdSetNote)->writeByte(pC3)->writeByte(0)->writeEOF();


        // #3
        sequence6->writeDelta(2)->writeCommand(CmdSetNote)->writeByte(pG3)->writeByte(144)->writeEOF();
        sequence6->writeDelta(2)->writeCommand(CmdSetNote)->writeByte(pG3)->writeByte(0)->writeEOF();

        sequence6->writeDelta(2)->writeCommand(CmdSetNote)->writeByte(pG2)->writeByte(144)->writeEOF();
        sequence6->writeDelta(2)->writeCommand(CmdSetNote)->writeByte(pG2)->writeByte(0)->writeEOF();

        sequence6->writeDelta(2)->writeCommand(CmdSetNote)->writeByte(pG2)->writeByte(144)->writeEOF();
        sequence6->writeDelta(2)->writeCommand(CmdSetNote)->writeByte(pG2)->writeByte(0)->writeEOF();

        sequence6->writeDelta(2)->writeCommand(CmdSetNote)->writeByte(pG3)->writeByte(144)->writeEOF();
        sequence6->writeDelta(2)->writeCommand(CmdSetNote)->writeByte(pG3)->writeByte(0)->writeEOF();

        sequence6->writeDelta(2)->writeEOS();
    }
    #pragma endregion

    var adapterList = NEW_(Stream);
    // 2 adapters
    adapterList->writeByte(2);
    // 1st adapter: PlayerAdapter
    {
        adapterList->writeByte(playerAdapter_.getInfo()->id);
        // no init data
        // no devices
        adapterList->writeByte(0);
    }
    // 2nd adapter: SynthAdapter
    {
        adapterList->writeByte(synthAdapter_.getInfo()->id);
        adapterList->writeWord(48000);
        // 9 devices
        adapterList->writeByte(9);
        adapterList->writeByte(SynthDevices::DeviceDrums);  //dev #1: drums

        #pragma region Synths
        adapterList->writeByte(SynthDevices::DeviceBass);  //dev #2: bass
            adapterList->writeByte(1);      // voice count
            adapterList->writeByte(255);    // data block id for sound bank (invalid id => use default soundbank)
            adapterList->writeByte(0);      // program

        adapterList->writeByte(SynthDevices::DeviceSynth);  //dev #3; chords
            adapterList->writeByte(6);      // voice count
            adapterList->writeByte(255);    // data block id for sound bank (invalid id => use default soundbank)
            adapterList->writeByte(1);      // program

        adapterList->writeByte(SynthDevices::DeviceSynth);  //dev #4: mono
            adapterList->writeByte(1);  // voice count
            adapterList->writeByte(255);    // data block id for sound bank (invalid id => use default soundbank)
            adapterList->writeByte(2);  // program
        #pragma endregion

        #pragma region Effects
        adapterList->writeByte(SynthDevices::DeviceDistort);  //dev #5
            adapterList->writeFloat(2.1f);      // amp
            adapterList->writeByte(220);        // lvl
            adapterList->writeByte(50);         // cut
            adapterList->writeByte(100);        // res
            adapterList->writeByte(FmLowPass || FmBandPass /*|| FmHighPass */ );    // mode

        adapterList->writeByte(SynthDevices::DeviceStereoDelay);  //dev #6
            adapterList->writeByte(160);        // feedbackL
            adapterList->writeFloat(120.0f);    // delayL
            adapterList->writeByte(140);        // feedbackR
            adapterList->writeFloat(240.0f);    // delayR
            adapterList->writeByte(120);        // mixL
            adapterList->writeByte(120);        // mixR
            adapterList->writeByte(80);         // cut
            adapterList->writeByte(10);         // res
            adapterList->writeByte(FmLowPass);  // mode


        adapterList->writeByte(SynthDevices::DeviceStereoDelay);  //dev #7
            adapterList->writeByte(140);        // feedbackL
            adapterList->writeFloat(64.0f);    // delayL
            adapterList->writeByte(140);        // feedbackR
            adapterList->writeFloat( 96.0f);    // delayR
            adapterList->writeByte(120);        // mixL
            adapterList->writeByte(220);        // mixR
            adapterList->writeByte(80);         // cut
            adapterList->writeByte(10);         // res
            adapterList->writeByte(FmLowPass);  // mode


        adapterList->writeByte(SynthDevices::DeviceStereoDelay);  //dev #8
            adapterList->writeByte( 40);        // feedbackl
            adapterList->writeFloat( 11.5f);    // delayL
            adapterList->writeByte(120);        // feedbackR
            adapterList->writeFloat(380.1f);    // delayR
            adapterList->writeByte(200);        // mixL
            adapterList->writeByte(180);        // mixR
            adapterList->writeByte(80);         // cut
            adapterList->writeByte(10);         // res
            adapterList->writeByte(FmLowPass);  // mode
        #pragma endregion

        #pragma region Mixer
        adapterList->writeByte(SynthDevices::DeviceMixer);  //dev #9
            adapterList->writeByte(4);
            
        // channel #1
            adapterList->writeByte(240);    // gain#1 - drums
            adapterList->writeByte( 50);    // pan#1
            adapterList->writeByte(120);    // amp#1
            adapterList->writeByte(  0);    // stage count

        // channel #2
            adapterList->writeByte(60);     // gain#2 - bass
            adapterList->writeByte( 60);    // pan#2
            adapterList->writeByte( 60);    // amp#2
            adapterList->writeByte(1);      // stage count
                adapterList->writeByte(80); // stage gain

        // channel #3
            adapterList->writeByte(100);    // gain#4 - chords
            adapterList->writeByte( 30);    // pan#4
            adapterList->writeByte( 80);    // amp#4
            adapterList->writeByte(2);      // stage count
                adapterList->writeByte(120);  // stage gain
                adapterList->writeByte( 80);  // stage gain

        // channel #4                                            
            adapterList->writeByte(160);    // gain#3 - mono
            adapterList->writeByte( 40);    // pan#5
            adapterList->writeByte( 70);    // amp#5
            adapterList->writeByte(1);      // stage count
                adapterList->writeByte(180);  // stage gain
                
        adapterList->writeByte(1);      // channel1 input: drums
        adapterList->writeByte(2);      // channel2 input: bass
            adapterList->writeByte(5);  // channel2.stage1 input: clip
        adapterList->writeByte(3);      // channel3 input:chords
            adapterList->writeByte(6);  // channel3.stage1 input: delay
            adapterList->writeByte(7);  // channel3.stage2 input: delay
        adapterList->writeByte(4);      // channel4 input
            adapterList->writeByte(8);  // channel5.stage1 input: delay
        #pragma endregion
    }

    #pragma region CreateStream
    var sequenceList = NEW_(Stream);
    sequenceList->writeByte(6);
    sequenceList->writeWord((word)sequence1->length());
    sequenceList->writeWord((word)sequence2->length());
    sequenceList->writeWord((word)sequence3->length());
    sequenceList->writeWord((word)sequence4->length());
    sequenceList->writeWord((word)sequence5->length());
    sequenceList->writeWord((word)sequence6->length());
    sequenceList->writeStream(sequence1);
    sequenceList->writeStream(sequence2);
    sequenceList->writeStream(sequence3);
    sequenceList->writeStream(sequence4);
    sequenceList->writeStream(sequence5);
    sequenceList->writeStream(sequence6);
    DEL_(sequence1);
    DEL_(sequence2);
    DEL_(sequence3);
    DEL_(sequence4);
    DEL_(sequence5);
    DEL_(sequence6);

    Bass bass(&synthAdapter_.samplingRate);
    Synth synth(&synthAdapter_.samplingRate);
    //var soundBank = synth.getDefaultSoundbank()->toStream();
    //var bassSoundBank = bass.getDefaultSoundbank()->toStream();

    // header
    // 3+2 data blocks
    data->writeWord(3);
    data->writeDword(initData->length());
    data->writeDword(adapterList->length());
    data->writeDword(sequenceList->length());
    //data->writeDword(soundBank->length());
    //data->writeDword(bassSoundBank->length());

    data->writeStream(initData);
    DEL_(initData);
    data->writeStream(adapterList);
    DEL_(adapterList);
    data->writeStream(sequenceList);
    DEL_(sequenceList);
    //data->writeStream(soundBank);
    //data->writeStream(bassSoundBank);
    #pragma endregion

    return data;
}

void SynthTest::testCreateBinary() {
    Player::addAdapter(NEW_(SynthAdapter));
    var bin = createBinaryData();
    var data = bin->data();
    var device = PlayerDevice::create(&data);
    var player = device->player();
    var stream = player->save();

    var cons = getConsole();
    cons->printf("Expected\n");
    cons->dump(bin->data(), bin->length(), 16);
    cons->printf("---\nReceived\n");
    cons->dump(stream->data(), stream->length(), 16);
    // compare bin and stream;
    assert("Should be same long", bin->length() == stream->length());
    var isMatch = true;
    stream->reset();
    bin->reset();
    cons->printf("Binary comparison\n");
    for (var i = 0; i < bin->length(); i++) {
        var ca = bin->readByte();
        var cb = stream->readByte();
        if (ca != cb) {
            cons->printf(" - %04X: %02X %02X\n", i, ca, cb);
            isMatch = false;
            break;
        }
    }
    assert("Should be binary identical", isMatch);

    bin->extract();
    DEL_(stream);
    DEL_(bin);
    DEL_(device);
    Player::cleanUp();
}

void SynthTest::testLoadBinary() {
    Player::addAdapter(NEW_(SynthAdapter));
    var bin = createBinaryData();
    var data = bin->extract();
    DEL_(bin);
    var device = PlayerDevice::create(&data);
    var player = device->player();
    player->assignChannel(0, (Sequence*)player->sequences().get(0), 0, 0);
    assert("Should have 32.0 fps", player->refreshRate() == 32.0f);
    assert("Should have 10 devices", player->devices().length() == 10);
    assert("Should have PlayerDevice as #1", ((Device*)player->devices().get(0))->type() == PlayerDevices::DevicePlayer);
    assert("Should have correct devices",
        ((Device*)player->devices().get(1))->type() ==SynthDevices::DeviceDrums &&
        ((Device*)player->devices().get(2))->type() == SynthDevices::DeviceBass &&
        ((Device*)player->devices().get(3))->type() == SynthDevices::DeviceSynth &&
        ((Device*)player->devices().get(4))->type() == SynthDevices::DeviceSynth &&
        ((Device*)player->devices().get(5))->type() == SynthDevices::DeviceDistort &&
        ((Device*)player->devices().get(6))->type() == SynthDevices::DeviceStereoDelay &&
        ((Device*)player->devices().get(7))->type() == SynthDevices::DeviceStereoDelay &&
        ((Device*)player->devices().get(8))->type() == SynthDevices::DeviceStereoDelay &&
        ((Device*)player->devices().get(9))->type() == SynthDevices::DeviceMixer);
    assert("Should have 6 sequences", player->sequences().length() == 6);
    assert("Should have no data-blocks", player->dataBlocks().length() == 0);
    Key ix = 0;
    var mixer = (Mixer8x4*)((Device*)player->devices().search(NULL, ix,
        [](COLLECTION_ARGUMENTS) { return ((Device*)value)->type() == SynthDevices::DeviceMixer ? 0 : 1; }))->object();

    if (isSaveToWave_) {
        var wave = NEW_(WaveFmt, "test-sequence01.wav", (int)samplingRate, 2, 16);
        var length = 2 * 8 * (int)samplingRate; // 8 secs stereo buffer
        var buffer = MALLOC(short, length);
        // write preroll
        wave->write((byte*)buffer, SAMPLE_BUFFER_SIZE);
        var count = length >> 1;
        var samplePerFrame = samplingRate / player->refreshRate();
        float error = samplePerFrame - (int)samplePerFrame;
        float errorCounter = 0.0f;
        var p = buffer;
        memset(p, 0, 2 * length);
        var cons = getConsole();
        var totalFrames = 0;
        while (device->isActive()) {
            cons->showCursor(false);
            cons->printf("% 5d", totalFrames++);
            cons->movexy(-5, 0);
            // step 1 frame
            player->run(1);
            var sampleCount = (int)samplePerFrame;
            errorCounter += error;
            if (errorCounter > 1.0f) {
                errorCounter -= 1.0f;
                sampleCount++;
            }

            // fill frame buffer
            if (count < sampleCount) {
                sampleCount = count;
            }

            count -= sampleCount;
            while (sampleCount > 0) {
                var toWrite = sampleCount > SAMPLE_BUFFER_SIZE ? SAMPLE_BUFFER_SIZE : sampleCount;
                Mixer8x4::fillSoundBuffer(p, toWrite, mixer);
                p += 2 * toWrite;
                sampleCount -= toWrite;
            }
            if (count == 0) {
                // flush buffer
                wave->write((byte*)buffer, 2 * length);
                count = length >> 1;
                p = buffer;
                memset(p, 0, 2 * length);
            }
        }
        if (count > 0) {
            // flush buffer
            wave->write((byte*)buffer, 2 * length - 4*count);
        }
        wave->close();
        DEL_(wave);
        FREE(buffer);
    }
    else {
        var count = 0;
        if (SoundPlayer::start((int)samplingRate, 2, Mixer8x4::fillSoundBuffer, mixer) == 0) {
            player->useThread();
            player->start();
            while (device->isActive()) {
                Sleep(3);
            }
            SoundPlayer::stop();
        }
    }
    assert("Should run to the end", true);

    DEL_(device);
    Player::cleanUp();
}

//Stream* addDevice(Player* player, InstrumentInfo* instrumentInfo) {
//    return ;
//}
//
//Device* addEffectToChannel(Player* player, MixerChannel* channel) {
//    return NULL;
//}

void SynthTest::testLoadXm() {
    var device = PlayerDevice::create();
    var player = device->player();
    var xmLoader = NEW_(XmLoader, player);
    // load xm
    // - add sequences
    // - create master sequence
    // - add synth/drum devices
    // - add mixer
    // extern
    // - add effects
    // - add soundbanks
    //xmLoader->load("test.xm", addDevice, addEffectToChannel);
    xmLoader->load("test.xm");

    DEL_(xmLoader);
    DEL_(device);
    Player::cleanUp();
}

void SynthTest::testDrumLoop() {
    char* file = "drums1.pat";
    char* output = "drums1.wav";
    // read pattern file
    if (File::exists(file)) {

        #pragma region Player
        Env::initialize();
        Flt::initialize();
        var device = PlayerDevice::create();
        var player = device->player();
        var synthAdapter = (SynthAdapter*)Player::addAdapter(NEW_(SynthAdapter));

        byte* bp = NULL;
        var drumsDevice = (DrumsDevice*)player->addDevice(synthAdapter, SynthDevices::DeviceDrums);
        #pragma region Effects
        var distortDevice = (DistortDevice*)player->addDevice(synthAdapter, SynthDevices::DeviceDistort, NULL);
        var distort = (Module*)distortDevice->object();
        distort->setControl(distAmp, 1.2f); distort->setControl(distLvl, 0.6f);
        distort->setControl(distCut, 20); distort->setControl(distRes, 0.6f);
        distort->setControl(distMode, FmBandPass);
        var stereoDelayDevice = (StereoDelayDevice*)player->addDevice(synthAdapter, SynthDevices::DeviceStereoDelay, NULL);
        var stereoDelay = (Module*)stereoDelayDevice->object();
        stereoDelay->setControl(stdlFeedbackLeft, 0.5f); stereoDelay->setControl(stdlFeedbackRight, 0.4f);
        stereoDelay->setControl(stdlDelayLeft, 141.6f); stereoDelay->setControl(stdlDelayRight, 134.6f);
        stereoDelay->setControl(stdlMixLeft, 0.1f); stereoDelay->setControl(stdlMixRight, 0.2f);
        stereoDelay->setControl(stdlCut, 40); stereoDelay->setControl(stdlRes, 0.2f);
        stereoDelay->setControl(stdlMode, FmBandPass | FmHighPass);
        #pragma endregion

        #pragma region Mixer
        var mixer = (Mixer8x4*)(player->addDevice(synthAdapter, SynthDevices::DeviceMixer)->object());
        mixer->channelCount(1);
        var ch1 = mixer->getChannel(0);
        mixer->connectInput(ch1, drumsDevice->drums());
        mixer->connectEffect(ch1, (Module*)distortDevice->object());
        mixer->connectEffect(ch1, (Module*)stereoDelayDevice->object());
        ch1->controls->amp.value = 0.8f;
        ch1->controls->gain.value = 0.5f;
        ch1->controls->pan.value = 0.5f;
        ch1->controls->stages[0].gain.value = 0.1f;
        ch1->controls->stages[1].gain.value = 0.2f;

        var channel = NEW_(Channel, "drums");
        player->channels().push(channel);
        #pragma endregion

        #pragma region Parse
        Map deviceRowProcessorMap(sizeof(int), sizeof(ROWPROCESSOR*), Map::hashingInt, Map::compareInt);
        deviceRowProcessorMap.hasRefKey(false);
        var rowProcessor = &synthRowProcessor;
        deviceRowProcessorMap.add(SynthDevices::DeviceBass, &rowProcessor);
        deviceRowProcessorMap.add(SynthDevices::DeviceSynth, &rowProcessor);
        rowProcessor = &drumsRowProcessor;
        deviceRowProcessorMap.add(SynthDevices::DeviceDrums, &rowProcessor);
        parseFile(file, player, &deviceRowProcessorMap);
        player->refreshRate();
        #pragma endregion
        
        #pragma region Playback
        var drums = drumsDevice->drums();
        drums->drums[0].setProgram(1);
        //drums->drums[0].setControl(gdDahr1Rel, 140);
        //drums->drums[0].setControl(gdDahr2Rel, 180);
        drums->drums[1].setProgram(2);
        //drums->drums[1].setControl(gdDahr1Hld, 26);
        //drums->drums[1].setControl(gdDahr1Amp,  60/255.0f);
        drums->drums[1].setControl(gdDahr1Hld,  12);
        drums->drums[1].setControl(gdDahr1Rel,  24);
        //drums->drums[1].setControl(gdDahr3Amp, 250 / 255.0f);
        //drums->drums[1].setControl(gdDahr3Rel,  8);
        drums->drums[1].setControl(gdFlt1Cut,   40);
        //drums->drums[1].setControl(gdAmp3, 150/255.0f);
        drums->drums[2].setProgram(3);
        drums->drums[3].setProgram(5);
        drums->drums[3].setControl(gdDahr1Hld, 50);
        drums->drums[4].setProgram(7);
        drums->drums[5].setProgram(8);
        drums->drums[6].setProgram(9);
        //drums->setControl(bdTone, 1.0f);
        //drums->setControl(bdTune, 41.0f);
        //drums->setControl(bdDecay, 80);
        //drums->snareDrum().setProgram(DrumPrgId::prgSD808);
        //drums->setControl(sdSnappy, 0.2f);
        //drums->setControl(sdTune, 40.0f);
        //drums->setControl(sdTone, 80);
        //drums->closedHihat().setProgram(DrumPrgId::prgCH808);
        //drums->openHihat().setProgram(DrumPrgId::prgOH808);
        //drums->setControl(hhTone, 220);
        //drums->setControl(ohDecay, 160);

        int ix = 0;
        if (!isSaveToWave_) {

            if (SoundPlayer::start((int)samplingRate, 2, Mixer8x4::fillSoundBuffer, mixer) == 0) {
                player->useThread();
                player->start();
                player->masterChannel()->loopCount(128);

                var counter = 0;
                var toggle = false;
                var patternTicks = 8 * framePerRow_;
                var device = player->masterDevice();

                while (device->isActive()) {
                    if (player->counter() > counter) {
                        counter += patternTicks;
                        toggle = !toggle;
                        //if (toggle) {
                        //    drums->bassDrum().setProgram(prgBD707);
                        //    drums->snareDrum().setProgram(prgSDa0a);
                        //    drums->closedHihat().setProgram(prgHHa0a);
                        //    drums->openHihat().setProgram(prgHHa0a);
                        //}
                        //else {
                        //    drums->bassDrum().setProgram(prgBD808);
                        //    drums->snareDrum().setProgram(prgSD808);
                        //    drums->closedHihat().setProgram(prgHH808);
                        //    drums->openHihat().setProgram(prgHH808);
                        //}
                    }
                    //Sleep(3);
                }


                player->stop();
                assert("Should run to the end", true);
                SoundPlayer::stop();
            }
        }
        else {
            var samplesPerFrame = synthAdapter->samplingRate / player->refreshRate();
            int bufferSizeInSamples = 16384;
            int bufferSize = 2 * sizeof(short) * bufferSizeInSamples;
            var buffer = MALLOC(short, bufferSize);
            var offset = buffer;
            var wave = NEW_(WaveFmt, output, (int)samplingRate, 2, 16);
            var frameCounter = 0.0f;
            var frameCount = 0;
            int totalCount = 0;
            bool isStopped = false;
            var patternTicks = 16 * framePerRow_;
            var toggle = false;
            do {
                int remainingSamples = bufferSizeInSamples;
                memset(buffer, 0, bufferSize);
                var offset = buffer;
                while (remainingSamples > 0) {
                    var count = remainingSamples > SAMPLE_BUFFER_SIZE ? SAMPLE_BUFFER_SIZE : remainingSamples;
                    if (frameCounter <= 0) {
                        if ((player->counter() % patternTicks) == 0) {
                            //if (toggle) {
                            //    drums->bassDrum().setProgram(prgBD808);
                            //    drums->snareDrum().setProgram(prgSD808);
                            //    drums->closedHihat().setProgram(prgHH808);
                            //    drums->openHihat().setProgram(prgHH808);
                            //}
                            //else {
                            //    drums->bassDrum().setProgram(prgBD707);
                            //    drums->snareDrum().setProgram(prgSDa0a);
                            //    drums->closedHihat().setProgram(prgHHa0a);
                            //    drums->openHihat().setProgram(prgHHa0a);
                            //}

                            toggle = !toggle;
                        }
                        player->run(1);
                        frameCount++;
                        frameCounter += samplesPerFrame;
                        if (!player->masterChannel()->isActive() || frameCount > totalFrames_) {
                            isStopped = true;
                            break;
                        }
                    }
                    if (frameCounter < count) {
                        count = (int)ceil(frameCounter);
                    }
                    Mixer8x4::fillSoundBuffer(offset, count, mixer);
                    offset += 2 * count;
                    totalCount += count;
                    cons->showCursor(false);
                    cons->printf("% 5d / % 5d", frameCount, totalFrames_);
                    cons->movexy(-13, 0);
                    remainingSamples -= count;
                    frameCounter -= count;
                }
                wave->write((byte*)buffer, (int)(2 * (offset - buffer)));
            } while (!isStopped);
            cons->printf("\n%d samples saved\n", totalCount);
            wave->close();
            DEL_(wave);
            FREE(buffer);
        }
        #pragma endregion        
        
        //DEL_(bassSoundBank_);
        //DEL_(soundBank_);
        DEL_(device);
        Player::cleanUp();
    } else {
        cons->printf("File could not be accessed!");
    }
    assert("Drums!", true);
}

void SynthTest::runAll(int& totalPassed, int& totalFailed) {
    totalPassed += totalPassed_;
    totalFailed += totalFailed_;
}
#pragma endregion

void createMoonWalzer()
{
    // a small song with a bass, strings, piano and drums
    var synthAdapter = (SynthAdapter*)Player::addAdapter(NEW_(SynthAdapter));
    PlayerDevice playerDevice(NULL, &playerAdapter_);
    SynthDevice pianoDevice(synthAdapter);
    SynthDevice padDevice(synthAdapter);
    BassDevice bassDevice(synthAdapter);
    DrumsDevice drumsDevice(synthAdapter);

    var data = NEW_(Stream);

    var initData = NEW_(Stream);
    initData->writeFloat(7.4f);
    // 7 channels
    initData->writeByte(7);

#pragma region Sequences
    // MASTER
    var steps = 24;
    var seqCount = 0;

    //        00 00 00 00 00 11 11 11 11 11 22 22 22 
    //        01 23 45 67 89 01 23 45 67 89 01 23 45 
    //  bass: 11 11 11 11 11 11 11 11 11 11 11 11    0/24
    //  pad1:    33 33 33    33 33 33    33 33       2/6 10/6 18/4
    //  pad2:          44 44 44 44       44 44 44    6/8 18/6
    // piano:    22 22 22    22 22 22 2              2/6 10/7
    // drums:       55 55 66 55 55 55 66             4/4 8/2 10/6 16/2
    // bass2: 77 77 77 77 77 77 77 77 77 77 77 77 77 0/28
    //  0 +0 bass/24 bass2/28
    //  2 +2 pad1/6 piano/6
    //  4 +2 drums/4
    //  6 +2 pad2/8
    //  8 +2 drums/2
    // 10 +2 pad1/6 drums/6 piano/7
    // 16 +6 drums/2
    // 18 +2 pad1/4 pad2/4
    // 24 +6 end

    var totalFrameCount = 0;

    var masterSeq = NEW_(Sequence, &playerDevice);
    {
        masterSeq->writeHeader();

        masterSeq->writeDelta(0);
        masterSeq->writeCommand(PlayerCommands::CmdAssign)->writeByte(6)->writeByte(7)->writeByte(5)->writeByte(0);    // bass2
        masterSeq->writeEOF();

        masterSeq->writeDelta(2 * steps);
        masterSeq->writeCommand(PlayerCommands::CmdAssign)->writeByte(1)->writeByte(1)->writeByte(1)->writeByte(12);    // bass
        masterSeq->writeEOF();
        totalFrameCount += 2 * steps;

        masterSeq->writeDelta(2 * steps);
        masterSeq->writeCommand(PlayerCommands::CmdAssign)->writeByte(2)->writeByte(2)->writeByte(2)->writeByte(6);     // piano
        masterSeq->writeCommand(PlayerCommands::CmdAssign)->writeByte(3)->writeByte(3)->writeByte(3)->writeByte(3);     // pad1
        masterSeq->writeEOF();
        totalFrameCount += 2 * steps;

        masterSeq->writeDelta(2 * steps);
        masterSeq->writeCommand(PlayerCommands::CmdAssign)->writeByte(5)->writeByte(5)->writeByte(4)->writeByte(4);     // drums
        masterSeq->writeEOF();
        totalFrameCount += 2 * steps;

        masterSeq->writeDelta(2 * steps);
        masterSeq->writeCommand(PlayerCommands::CmdAssign)->writeByte(4)->writeByte(4)->writeByte(3)->writeByte(4);     // pad2
        masterSeq->writeEOF();
        totalFrameCount += 2 * steps;

        masterSeq->writeDelta(2 * steps);
        masterSeq->writeCommand(PlayerCommands::CmdAssign)->writeByte(5)->writeByte(6)->writeByte(4)->writeByte(2);     // drums
        masterSeq->writeEOF();
        totalFrameCount += 2 * steps;

        masterSeq->writeDelta(2 * steps);
        masterSeq->writeCommand(PlayerCommands::CmdAssign)->writeByte(2)->writeByte(2)->writeByte(2)->writeByte(7);     // piano
        masterSeq->writeCommand(PlayerCommands::CmdAssign)->writeByte(3)->writeByte(3)->writeByte(3)->writeByte(3);     // pad1
        masterSeq->writeCommand(PlayerCommands::CmdAssign)->writeByte(5)->writeByte(5)->writeByte(4)->writeByte(6);     // drums
        masterSeq->writeEOF();
        totalFrameCount += 2 * steps;

        masterSeq->writeDelta(6 * steps);
        masterSeq->writeCommand(PlayerCommands::CmdAssign)->writeByte(5)->writeByte(6)->writeByte(4)->writeByte(2);     // drums
        masterSeq->writeEOF();
        totalFrameCount += 6 * steps;

        masterSeq->writeDelta(2 * steps);
        masterSeq->writeCommand(PlayerCommands::CmdAssign)->writeByte(3)->writeByte(3)->writeByte(3)->writeByte(2);     // pad1
        masterSeq->writeCommand(PlayerCommands::CmdAssign)->writeByte(4)->writeByte(4)->writeByte(3)->writeByte(3);     // pad2
        masterSeq->writeEOF();
        totalFrameCount += 2 * steps;

        masterSeq->writeDelta(2 * steps);
        masterSeq->writeCommand(PlayerCommands::CmdAssign)->writeByte(6)->writeByte(7)->writeByte(5)->writeByte(0);    // bass2
        masterSeq->writeEOF();
        totalFrameCount += 2 * steps;

        masterSeq->writeDelta(8 * steps);
        masterSeq->writeEOS();
        totalFrameCount += 8 * steps;
        seqCount++;
    }

    // BASS
    var bassSeq = NEW_(Sequence, &bassDevice);      // 48
    {
        bassSeq->writeHeader();

        bassSeq->writeDelta(0); bassSeq->writeCommand(CmdSetNote)->writeByte(pC2)->writeByte(140); bassSeq->writeEOF(); //  0
        bassSeq->writeDelta(4); bassSeq->writeCommand(CmdSetNote)->writeByte(pC3)->writeByte(160); bassSeq->writeEOF(); //  4
        bassSeq->writeDelta(4); bassSeq->writeCommand(CmdSetNote)->writeByte(pC2)->writeByte(  0); bassSeq->writeEOF(); //  8
        bassSeq->writeDelta(4); bassSeq->writeCommand(CmdSetNote)->writeByte(pC1)->writeByte(180);
                                bassSeq->writeCommand(CmdSetNote)->writeByte(pC3)->writeByte(  0); bassSeq->writeEOF(); // 12
        bassSeq->writeDelta(2); bassSeq->writeCommand(CmdSetNote)->writeByte(pC1)->writeByte(  0); bassSeq->writeEOF(); // 14
        bassSeq->writeDelta(2); bassSeq->writeCommand(CmdSetNote)->writeByte(pC2)->writeByte(100); bassSeq->writeEOF(); // 16
        bassSeq->writeDelta(4); bassSeq->writeCommand(CmdSetNote)->writeByte(pC2)->writeByte(  0);
                                bassSeq->writeCommand(CmdSetNote)->writeByte(pC3)->writeByte(120); bassSeq->writeEOF(); // 20
        bassSeq->writeDelta(2); bassSeq->writeCommand(CmdSetNote)->writeByte(pC3)->writeByte(  0); bassSeq->writeEOF(); // 22
        bassSeq->writeDelta(2); bassSeq->writeCommand(CmdSetNote)->writeByte(pH1)->writeByte(140);
                                bassSeq->writeCommand(CmdSetNote)->writeByte(pC3)->writeByte(  0); bassSeq->writeEOF(); // 24
        bassSeq->writeDelta(4); bassSeq->writeCommand(CmdSetNote)->writeByte(pH2)->writeByte(160); bassSeq->writeEOF(); // 28
        bassSeq->writeDelta(4); bassSeq->writeCommand(CmdSetNote)->writeByte(pH1)->writeByte(  0); bassSeq->writeEOF(); // 32
        bassSeq->writeDelta(4); bassSeq->writeCommand(CmdSetNote)->writeByte(pH0)->writeByte(180);
                                bassSeq->writeCommand(CmdSetNote)->writeByte(pH2)->writeByte(  0); bassSeq->writeEOF(); // 36
        bassSeq->writeDelta(4); bassSeq->writeCommand(CmdSetNote)->writeByte(pH0)->writeByte(  0);
                                bassSeq->writeCommand(CmdSetNote)->writeByte(pH1)->writeByte(140); bassSeq->writeEOF(); // 40
        bassSeq->writeDelta(4); bassSeq->writeCommand(CmdSetNote)->writeByte(pH1)->writeByte(  0); bassSeq->writeEOF(); // 44

        bassSeq->writeDelta(4);
        bassSeq->writeEOS();
        seqCount++;
    }

    // PIANO
    var pianoSeq = NEW_(Sequence, &pianoDevice);    // 24
    {
        pianoSeq->writeHeader();

        pianoSeq->writeDelta(0); pianoSeq->writeCommand(CmdSetNote)->writeByte(pC4)->writeByte(160); pianoSeq->writeEOF();
        pianoSeq->writeDelta(1); pianoSeq->writeCommand(CmdSetNote)->writeByte(pC4)->writeByte(  0); pianoSeq->writeEOF();

        pianoSeq->writeDelta(1); pianoSeq->writeCommand(CmdSetNote)->writeByte(pD4)->writeByte( 80); pianoSeq->writeEOF();
        pianoSeq->writeDelta(1); pianoSeq->writeCommand(CmdSetNote)->writeByte(pD4)->writeByte(  0); pianoSeq->writeEOF();

        pianoSeq->writeDelta(1); pianoSeq->writeCommand(CmdSetNote)->writeByte(pE4)->writeByte(120); pianoSeq->writeEOF();
        pianoSeq->writeDelta(1); pianoSeq->writeCommand(CmdSetNote)->writeByte(pE4)->writeByte(  0); pianoSeq->writeEOF();

        pianoSeq->writeDelta(1); pianoSeq->writeCommand(CmdSetNote)->writeByte(pFs4)->writeByte(140); pianoSeq->writeEOF();
        pianoSeq->writeDelta(1); pianoSeq->writeCommand(CmdSetNote)->writeByte(pFs4)->writeByte(  0); pianoSeq->writeEOF();

        pianoSeq->writeDelta(1); pianoSeq->writeCommand(CmdSetNote)->writeByte(pGs4)->writeByte(160); pianoSeq->writeEOF();
        pianoSeq->writeDelta(1); pianoSeq->writeCommand(CmdSetNote)->writeByte(pGs4)->writeByte(0); pianoSeq->writeEOF();

        pianoSeq->writeDelta(1); pianoSeq->writeCommand(CmdSetNote)->writeByte(pAs4)->writeByte(80); pianoSeq->writeEOF();
        pianoSeq->writeDelta(1); pianoSeq->writeCommand(CmdSetNote)->writeByte(pAs4)->writeByte(0); pianoSeq->writeEOF();

        pianoSeq->writeDelta(1); pianoSeq->writeCommand(CmdSetNote)->writeByte(pC5)->writeByte(120); pianoSeq->writeEOF();
        pianoSeq->writeDelta(1); pianoSeq->writeCommand(CmdSetNote)->writeByte(pC5)->writeByte(0); pianoSeq->writeEOF();

        pianoSeq->writeDelta(1); pianoSeq->writeCommand(CmdSetNote)->writeByte(pAs4)->writeByte(140); pianoSeq->writeEOF();
        pianoSeq->writeDelta(1); pianoSeq->writeCommand(CmdSetNote)->writeByte(pAs4)->writeByte(0); pianoSeq->writeEOF();

        pianoSeq->writeDelta(1); pianoSeq->writeCommand(CmdSetNote)->writeByte(pGs4)->writeByte(160); pianoSeq->writeEOF();
        pianoSeq->writeDelta(1); pianoSeq->writeCommand(CmdSetNote)->writeByte(pGs4)->writeByte(0); pianoSeq->writeEOF();

        pianoSeq->writeDelta(1); pianoSeq->writeCommand(CmdSetNote)->writeByte(pFs4)->writeByte(140); pianoSeq->writeEOF();
        pianoSeq->writeDelta(1); pianoSeq->writeCommand(CmdSetNote)->writeByte(pFs4)->writeByte(0); pianoSeq->writeEOF();

        pianoSeq->writeDelta(1); pianoSeq->writeCommand(CmdSetNote)->writeByte(pE4)->writeByte(120); pianoSeq->writeEOF();
        pianoSeq->writeDelta(1); pianoSeq->writeCommand(CmdSetNote)->writeByte(pE4)->writeByte(0); pianoSeq->writeEOF();

        pianoSeq->writeDelta(1); pianoSeq->writeCommand(CmdSetNote)->writeByte(pD4)->writeByte(80); pianoSeq->writeEOF();
        pianoSeq->writeDelta(1); pianoSeq->writeCommand(CmdSetNote)->writeByte(pD4)->writeByte(0); pianoSeq->writeEOF();

        pianoSeq->writeDelta(1);
        pianoSeq->writeEOS();
        seqCount++;
    }

    // PAD1
    var pad1Seq = NEW_(Sequence, &padDevice);        // 48
    {
        pad1Seq->writeHeader();

        pad1Seq->writeDelta(0);
        pad1Seq->writeCommand(CmdSetNote)->writeByte(pC4)->writeByte(120);
        pad1Seq->writeCommand(CmdSetNote)->writeByte(pE4)->writeByte(140);
        pad1Seq->writeCommand(CmdSetNote)->writeByte(pGs4)->writeByte(100);
        pad1Seq->writeEOF();

        pad1Seq->writeDelta(12);
        pad1Seq->writeCommand(CmdSetNote)->writeByte(pC4)->writeByte(0);
        pad1Seq->writeCommand(CmdSetNote)->writeByte(pE4)->writeByte(0);
        pad1Seq->writeCommand(CmdSetNote)->writeByte(pGs4)->writeByte(0);
        pad1Seq->writeEOF();

        pad1Seq->writeDelta(12);
        pad1Seq->writeCommand(CmdSetNote)->writeByte(pH3)->writeByte(120);
        pad1Seq->writeCommand(CmdSetNote)->writeByte(pDs4)->writeByte(140);
        pad1Seq->writeCommand(CmdSetNote)->writeByte(pG4)->writeByte(100);
        pad1Seq->writeEOF();

        pad1Seq->writeDelta(12);
        pad1Seq->writeCommand(CmdSetNote)->writeByte(pH3)->writeByte(0);
        pad1Seq->writeCommand(CmdSetNote)->writeByte(pDs4)->writeByte(0);
        pad1Seq->writeCommand(CmdSetNote)->writeByte(pG4)->writeByte(0);
        pad1Seq->writeEOF();

        pad1Seq->writeDelta(12);
        pad1Seq->writeEOS();
        seqCount++;
    }

    // PAD2
    var pad2Seq = NEW_(Sequence, &padDevice);        // 48
    {
        pad2Seq->writeHeader();

        pad2Seq->writeDelta(8);
        pad2Seq->writeCommand(CmdSetNote)->writeByte(pGs5)->writeByte(180);     //  8
        pad2Seq->writeEOF();

        pad2Seq->writeDelta(4);
        pad2Seq->writeCommand(CmdSetNote)->writeByte(pGs5)->writeByte(0);
        pad2Seq->writeCommand(CmdSetNote)->writeByte(pC6)->writeByte(130);       // 12
        pad2Seq->writeEOF();

        pad2Seq->writeDelta(8);
        pad2Seq->writeCommand(CmdSetNote)->writeByte(pC6)->writeByte(0);
        pad2Seq->writeEOF();


        pad2Seq->writeDelta(12);
        pad2Seq->writeCommand(CmdSetNote)->writeByte(pG5)->writeByte(180);      // 32
        pad2Seq->writeEOF();

        pad2Seq->writeDelta(4);
        pad2Seq->writeCommand(CmdSetNote)->writeByte(pG5)->writeByte(0);
        pad2Seq->writeCommand(CmdSetNote)->writeByte(pH5)->writeByte(130);       // 36
        pad2Seq->writeEOF();

        pad2Seq->writeDelta(8);
        pad2Seq->writeCommand(CmdSetNote)->writeByte(pH5)->writeByte(0);
        pad2Seq->writeEOF();

        pad2Seq->writeDelta(4);
        pad2Seq->writeEOS();
        seqCount++;
    }

    // DRUMS
    var drums1Seq = NEW_(Sequence, &drumsDevice);    // 24
    {
        drums1Seq->writeHeader();

        drums1Seq->writeDelta(0);
        drums1Seq->writeCommand(CmdSetNote)->writeByte(drBD)->writeByte(120);
        drums1Seq->writeCommand(CmdSetNote)->writeByte(drCH)->writeByte( 40);
        drums1Seq->writeEOF();

        drums1Seq->writeDelta(2);
        drums1Seq->writeCommand(CmdSetNote)->writeByte(drCH)->writeByte( 20);
        drums1Seq->writeEOF();

        drums1Seq->writeDelta(2);
        drums1Seq->writeCommand(CmdSetNote)->writeByte(drOH)->writeByte( 30);
        drums1Seq->writeEOF();

        drums1Seq->writeDelta(2);
        drums1Seq->writeCommand(CmdSetNote)->writeByte(drCH)->writeByte( 20);
        drums1Seq->writeEOF();

        drums1Seq->writeDelta(2);
        drums1Seq->writeCommand(CmdSetNote)->writeByte(drCP)->writeByte( 60);
        drums1Seq->writeCommand(CmdSetNote)->writeByte(drSD)->writeByte( 20);
        drums1Seq->writeCommand(CmdSetNote)->writeByte(drCH)->writeByte( 40);
        drums1Seq->writeEOF();

        drums1Seq->writeDelta(4);
        drums1Seq->writeCommand(CmdSetNote)->writeByte(drOH)->writeByte( 30);
        drums1Seq->writeEOF();

        drums1Seq->writeDelta(2);
        drums1Seq->writeCommand(CmdSetNote)->writeByte(drCH)->writeByte( 20);
        drums1Seq->writeEOF();

        drums1Seq->writeDelta(2);
        drums1Seq->writeCommand(CmdSetNote)->writeByte(drBD)->writeByte(100);
        drums1Seq->writeCommand(CmdSetNote)->writeByte(drCH)->writeByte( 40);
        drums1Seq->writeEOF();

        drums1Seq->writeDelta(2);
        drums1Seq->writeCommand(CmdSetNote)->writeByte(drCH)->writeByte( 20);
        drums1Seq->writeEOF();

        drums1Seq->writeDelta(2);
        drums1Seq->writeCommand(CmdSetNote)->writeByte(drOH)->writeByte( 30);
        drums1Seq->writeEOF();

        drums1Seq->writeDelta(2);
        drums1Seq->writeCommand(CmdSetNote)->writeByte(drCH)->writeByte( 20);
        drums1Seq->writeEOF();

        drums1Seq->writeDelta(2);
        drums1Seq->writeEOS();
        seqCount++;
    }

    var drums2Seq = NEW_(Sequence, &drumsDevice);    // 24
    {
        drums2Seq->writeHeader();

        drums2Seq->writeDelta(0);
        drums2Seq->writeCommand(CmdSetNote)->writeByte(drCH)->writeByte(40);
        drums2Seq->writeEOF();

        drums2Seq->writeDelta(4);
        drums2Seq->writeCommand(CmdSetNote)->writeByte(drCH)->writeByte(20);
        drums2Seq->writeEOF();

        drums2Seq->writeDelta(2);
        drums2Seq->writeCommand(CmdSetNote)->writeByte(drCH)->writeByte(20);
        drums2Seq->writeEOF();

        drums2Seq->writeDelta(2);
        drums2Seq->writeCommand(CmdSetNote)->writeByte(drSD)->writeByte(30);
        drums2Seq->writeCommand(CmdSetNote)->writeByte(drCH)->writeByte(40);
        drums2Seq->writeEOF();

        drums2Seq->writeDelta(4);
        drums2Seq->writeCommand(CmdSetNote)->writeByte(drCH)->writeByte(30);
        drums2Seq->writeEOF();

        drums2Seq->writeDelta(4);
        drums2Seq->writeCommand(CmdSetNote)->writeByte(drCH)->writeByte(40);
        drums2Seq->writeEOF();

        drums2Seq->writeDelta(2);
        drums2Seq->writeCommand(CmdSetNote)->writeByte(drCH)->writeByte(20);
        drums2Seq->writeEOF();

        drums2Seq->writeDelta(2);
        drums2Seq->writeCommand(CmdSetNote)->writeByte(drOH)->writeByte(30);
        drums2Seq->writeEOF();

        drums2Seq->writeDelta(2);
        drums2Seq->writeCommand(CmdSetNote)->writeByte(drCH)->writeByte(20);
        drums2Seq->writeEOF();

        drums2Seq->writeDelta(2);
        drums2Seq->writeEOS();
        seqCount++;
    }


    // BASS2
    var bass2Seq = NEW_(Sequence, &bassDevice);      // 48
    {
        bass2Seq->writeHeader();

        bass2Seq->writeDelta( 0);       bass2Seq->writeCommand(CmdSetNote)->writeByte(pF1)->writeByte(180);
                                        bass2Seq->writeCommand(CmdSetNote)->writeByte(pF3)->writeByte(100); bass2Seq->writeEOF();
        bass2Seq->writeDelta(4*steps);  bass2Seq->writeCommand(CmdSetNote)->writeByte(pF1)->writeByte(  0);
                                        bass2Seq->writeCommand(CmdSetNote)->writeByte(pF3)->writeByte(  0); bass2Seq->writeEOF();
        bass2Seq->writeDelta(1*steps);
        bass2Seq->writeEOS();
        seqCount++;
    }


#pragma endregion

#pragma region Devices
    var mixerId = 0;
    var adapterList = NEW_(Stream);
    // 2 adapters
    adapterList->writeByte(2);
    // 1st adapter: PlayerAdapter
    {
        adapterList->writeByte(playerAdapter_.getInfo()->id);
        // no init data
        // no devices
        adapterList->writeByte(0);
    }
    // 2nd adapter: SynthAdapter
    var drumsId = 0;
    {
        adapterList->writeByte(synthAdapter_.getInfo()->id);
        adapterList->writeWord(SAMPLING_RATE);

        var channelCount = 0;

        var devCount = adapterList->cursor();
        adapterList->writeByte(0);

        // Bass
        adapterList->writeByte(SynthDevices::DeviceBass);
        adapterList->writeByte(2);          // voice count
        adapterList->writeByte(255);        // data block id for sound bank 
        adapterList->writeByte(0);          // program
        var bassId = ++*devCount;
        channelCount++;

        // Piano
        adapterList->writeByte(SynthDevices::DeviceSynth);
        adapterList->writeByte(6);          // voice count
        adapterList->writeByte(0);          // data block id for sound bank 
        adapterList->writeByte(3);          // program
        var pianoId = ++*devCount;
        channelCount++;

        // Pad
        adapterList->writeByte(SynthDevices::DeviceSynth);
        adapterList->writeByte(8);          // voice count
        adapterList->writeByte(255);        // data block id for sound bank 
        adapterList->writeByte(1);          // program
        var padId = ++*devCount;
        channelCount++;

        // Drums
        adapterList->writeByte(SynthDevices::DeviceDrums);
        drumsId = ++ * devCount;
        channelCount++;

        // Bass2
        adapterList->writeByte(SynthDevices::DeviceBass);
        adapterList->writeByte(4);          // voice count
        adapterList->writeByte(255);        // data block id for sound bank 
        adapterList->writeByte(1);          // program
        var bass2Id = ++ * devCount;
        channelCount++;

        // Effects
        adapterList->writeByte(SynthDevices::DeviceDistort);
        adapterList->writeFloat(1.2f);      // amp
        adapterList->writeByte(250);        // lvl
        adapterList->writeByte(180);        // cut
        adapterList->writeByte( 20);        // res
        adapterList->writeByte(FmLowPass || FmBandPass /*|| FmHighPass */);    // mode
        var clip1Id = ++*devCount;

        adapterList->writeByte(SynthDevices::DeviceStereoDelay);
        adapterList->writeByte(10);         // feedbackL
        adapterList->writeFloat(20.0f);     // delayL
        adapterList->writeByte(0);          // feedbackR
        adapterList->writeFloat(10.0f);    // delayR
        adapterList->writeByte( 20);        // mixL
        adapterList->writeByte(250);        // mixR
        adapterList->writeByte(80);         // cut
        adapterList->writeByte(10);         // res
        adapterList->writeByte(FmLowPass);  // mode
        var delay1Id = ++*devCount;

        adapterList->writeByte(SynthDevices::DeviceStereoDelay);
        adapterList->writeByte(180);        // feedbackL
        adapterList->writeFloat(121.0f);    // delayL
        adapterList->writeByte(200);        // feedbackR
        adapterList->writeFloat(153.0f);    // delayR
        adapterList->writeByte(250);        // mixL
        adapterList->writeByte(250);        // mixR
        adapterList->writeByte(80);         // cut
        adapterList->writeByte(10);         // res
        adapterList->writeByte(FmLowPass);  // mode
        var delay2Id = ++*devCount;

        adapterList->writeByte(SynthDevices::DeviceStereoDelay);
        adapterList->writeByte(220);        // feedbackL
        adapterList->writeFloat(421.0f);    // delayL
        adapterList->writeByte(200);        // feedbackR
        adapterList->writeFloat(303.0f);    // delayR
        adapterList->writeByte(250);        // mixL
        adapterList->writeByte(240);        // mixR
        adapterList->writeByte(80);         // cut
        adapterList->writeByte(10);         // res
        adapterList->writeByte(FmLowPass);  // mode
        var delay3Id = ++ * devCount;

        adapterList->writeByte(SynthDevices::DeviceDistort);
        adapterList->writeFloat(2.8f);      // amp
        adapterList->writeByte(220);        // lvl
        adapterList->writeByte(100);        // cut
        adapterList->writeByte(160);        // res
        adapterList->writeByte(FmLowPass | FmBandPass /*|| FmHighPass */);
        var clip2Id = ++ * devCount;

        adapterList->writeByte(SynthDevices::DeviceDistort);
        adapterList->writeFloat(1.4f);      // amp
        adapterList->writeByte(240);        // lvl
        adapterList->writeByte(200);        // cut
        adapterList->writeByte(230);        // res
        adapterList->writeByte(FmLowPass || FmBandPass /*|| FmHighPass */);
        var clip3Id = ++ * devCount;

        adapterList->writeByte(SynthDevices::DeviceStereoDelay);
        adapterList->writeByte(180);        // feedbackL
        adapterList->writeFloat(231.0f);    // delayL
        adapterList->writeByte(200);        // feedbackR
        adapterList->writeFloat(263.0f);    // delayR
        adapterList->writeByte(250);        // mixL
        adapterList->writeByte(240);        // mixR
        adapterList->writeByte(80);         // cut
        adapterList->writeByte(10);         // res
        adapterList->writeByte(FmLowPass);  // mode
        var delay4Id = ++ * devCount;

        adapterList->writeByte(SynthDevices::DeviceStereoDelay);
        adapterList->writeByte(20);        // feedbackL
        adapterList->writeFloat(31.0f);    // delayL
        adapterList->writeByte(20);        // feedbackR
        adapterList->writeFloat(13.0f);    // delayR
        adapterList->writeByte(180);        // mixL
        adapterList->writeByte(160);        // mixR
        adapterList->writeByte(80);         // cut
        adapterList->writeByte(10);         // res
        adapterList->writeByte(FmLowPass);  // mode
        var delay5Id = ++ * devCount;

        adapterList->writeByte(SynthDevices::DeviceMixer);
        adapterList->writeByte(channelCount);
        mixerId = ++*devCount;

        // channel #1 - Bass
        adapterList->writeByte(  0);        // gain#1
        adapterList->writeByte( 50);        // pan#1
        adapterList->writeByte( 40);        // amp#1
        adapterList->writeByte(  2);        // stage count
            adapterList->writeByte( 40);    // stage#1 gain
            adapterList->writeByte(200);    // stage#2 gain

        // channel #2 - Piano
        adapterList->writeByte(100);        // gain#2
        adapterList->writeByte( 35);        // pan#2
        adapterList->writeByte( 60);        // amp#2
        adapterList->writeByte(  1);        // stage count
            adapterList->writeByte(100);    // stage gain

        // channel #3 - Pad
        adapterList->writeByte(100);        // gain#3
        adapterList->writeByte(50);         // pan#3
        adapterList->writeByte(20);         // amp#3
        adapterList->writeByte(1);          // stage count
            adapterList->writeByte( 80);    // stage gain

        // channel #4 - Drums
        adapterList->writeByte( 40);        // gain#4
        adapterList->writeByte( 60);        // pan#4
        adapterList->writeByte(  7);        // amp#4
        adapterList->writeByte(  2);        // stage count
            adapterList->writeByte( 20);    // stage gain
            adapterList->writeByte(200);    // stage gain

        // channel #5 - Bass2
        adapterList->writeByte(  0);        // gain#1
        adapterList->writeByte( 50);        // pan#1
        adapterList->writeByte( 60);        // amp#1
        adapterList->writeByte(  2);        // stage count
            adapterList->writeByte( 20);    // stage#1 gain
            adapterList->writeByte(200);    // stage#2 gain

        adapterList->writeByte(bassId);     // channel1.input
        adapterList->writeByte(clip1Id);    // channel1.stage1
        adapterList->writeByte(delay1Id);   // channel1.stage2

        adapterList->writeByte(pianoId);    // channel2.input
        adapterList->writeByte(delay2Id);   // channel2.stage1.input

        adapterList->writeByte(padId);      // channel3.input
        adapterList->writeByte(delay3Id);   // channel3.stage1.input

        adapterList->writeByte(drumsId);    // channel4.input
        adapterList->writeByte(clip2Id);    // channel4.stage1.input
        adapterList->writeByte(delay4Id);   // channel4.stage2.input

        adapterList->writeByte(bass2Id);    // channel5.input
        adapterList->writeByte(clip3Id);    // channel5.stage1
        adapterList->writeByte(delay5Id);   // channel5.stage2

    }
#pragma endregion

#pragma region CreateStream
    var sequenceList = NEW_(Stream);
    sequenceList->writeByte(seqCount);
    sequenceList->writeWord((word)masterSeq->length());
    sequenceList->writeWord((word)bassSeq->length());
    sequenceList->writeWord((word)pianoSeq->length());
    sequenceList->writeWord((word)pad1Seq->length());
    sequenceList->writeWord((word)pad2Seq->length());
    sequenceList->writeWord((word)drums1Seq->length());
    sequenceList->writeWord((word)drums2Seq->length());
    sequenceList->writeWord((word)bass2Seq->length());
    sequenceList->writeStream(masterSeq);
    sequenceList->writeStream(bassSeq);
    sequenceList->writeStream(pianoSeq);
    sequenceList->writeStream(pad1Seq);
    sequenceList->writeStream(pad2Seq);
    sequenceList->writeStream(drums1Seq);
    sequenceList->writeStream(drums2Seq);
    sequenceList->writeStream(bass2Seq);
    DEL_(masterSeq);
    DEL_(bassSeq);
    DEL_(pianoSeq);
    DEL_(pad1Seq);
    DEL_(pad2Seq);
    DEL_(drums1Seq);
    DEL_(drums2Seq);
    DEL_(bass2Seq);

    //var soundBank = ((Synth*)pianoDevice.object())->getDefaultSoundbank()->toStream();
    //var bassSoundBank = ((Bass*)bassDevice.object())->getDefaultSoundbank()->toStream();

    // header
    // 2+3 data blocks
    data->writeWord(3);
    data->writeDword(initData->length());
    data->writeDword(adapterList->length());
    data->writeDword(sequenceList->length());
    //data->writeDword(soundBank->length());
    //data->writeDword(bassSoundBank->length());

    data->writeStream(initData);
    DEL_(initData);
    data->writeStream(adapterList);
    DEL_(adapterList);
    data->writeStream(sequenceList);
    DEL_(sequenceList);
    //data->writeStream(soundBank);
    //data->writeStream(bassSoundBank);
#pragma endregion

    var ptr = data->extract();
    DEL_(data);
    var device = PlayerDevice::create(&ptr);
    var player = device->player();
    player->assignChannel(0, (Sequence*)player->sequences().get(0), 0, 0);

    Key ix = 0;
    var drums = ((DrumsDevice*)player->devices().get(drumsId))->drums();

    var mixer = (Mixer8x4*)((MixerDevice*)player->devices().get(mixerId))->object();
    //(Mixer8x4*)((Device*)player->devices().search(NULL, ix,
    //    [](COLLECTION_ARGUMENTS) { return ((Device*)value)->type() == SynthDevices::DeviceMixer ? 0 : 1; }))->object();

    if (!isSaveToWave_) {
        if (SoundPlayer::start((int)SAMPLING_RATE, 2, Mixer8x4::fillSoundBuffer, mixer) == 0) {
            player->useThread();
            player->start();
            while (device->isActive()) {
                Sleep(3);
            }
            SoundPlayer::stop();
        }
    }
    else {
        var cons = getConsole();
        var samplesPerFrame = SAMPLING_RATE / player->refreshRate();
        int bufferSizeInSamples = 16384;
        int bufferSize = 2 * sizeof(short) * bufferSizeInSamples;
        var buffer = MALLOC(short, bufferSize);
        var offset = buffer;
        var wave = NEW_(WaveFmt, "moon-walzer-s.wav", SAMPLING_RATE, 2, 16);
        var frameCounter = 0.0f;
        var frameCount = 0;
        int totalCount = 0;
        bool isStopped = false;
        var patternTicks = 16 * framePerRow_;
        var toggle = false;
        do {
            int remainingSamples = bufferSizeInSamples;
            memset(buffer, 0, bufferSize);
            var offset = buffer;
            while (remainingSamples > 0) {
                var count = remainingSamples > SAMPLE_BUFFER_SIZE ? SAMPLE_BUFFER_SIZE : remainingSamples;
                if (frameCounter <= 0) {
                    if (!player->run(1)) break;
                    frameCount++;
                    frameCounter += samplesPerFrame;
                }
                if (frameCounter <= count) {
                    count = (int)ceil(frameCounter);
                }
                Mixer8x4::fillSoundBuffer(offset, count, mixer);
                offset += 2 * count;
                totalCount += count;
                cons->showCursor(false);
                cons->printf("% 5d / % 5d", frameCount, totalFrameCount);
                cons->movexy(-13, 0);
                remainingSamples -= count;
                frameCounter -= count;
            }
            wave->write((byte*)buffer, (int)(2 * (offset - buffer)));
        } while (player->masterChannel()->isActive());
        cons->printf("\n%d samples saved\n", totalCount);

        wave->close();
        DEL_(wave);
        FREE(buffer);
    }

    DEL_(device);
    Player::cleanUp();
}

int _main(NS_FW_BASE::Map* args) {
#ifdef _DEBUG
    Debug::initialize(/*DEBUG_UNICODE | DEBUG_MEMORY*/);
#endif
/*
    var j = 10;
    for (var i = 199; i < 600;)
    {
        var isPrime = true;
        //printf("%d ", i);
        for (var k = 2; k < sqrt(i); k++) {
            if ((isPrime = i % k) == 0) {
                //printf("%d", k);
                break;
            }
        }
        if (isPrime) {
            printf("%d\n", i);
            i += j;
            j += 20;
        }
        else {
            i++;
        }
    }
*/

    LOG("Synth tests\n");
    var tests = NEW_(SynthTest);

    //tests->test("Test envelopes", (TestMethod) & (SynthTest::testEnvelopes));
    //tests->test("Generate Sound", (TestMethod) & (SynthTest::testGenerateSoundSimple));
    //tests->test("Generate Sound via Mixer", (TestMethod) & (SynthTest::testGenerateSoundMixer));
    //tests->test("Create player with devices", (TestMethod) & (SynthTest::testCreatePlayerWithDevices));
    //tests->test("Create drums", (TestMethod) & (SynthTest::testCreateDrums));
    //tests->test("Create binary", (TestMethod) & (SynthTest::testCreateBinary));
    //tests->test("Load from binary", (TestMethod) & (SynthTest::testLoadBinary));
    //tests->test("Load XM module", (TestMethod) & (SynthTest::testLoadXm));
    //tests->test("Filter comparison", (TestMethod) & (SynthTest::testFilterSound));
    tests->test("Test drum loop", (TestMethod) & (SynthTest::testDrumLoop));
    tests->displayFinalResults();

    //createMoonWalzer();

    DEL_(tests);

    return 0;
}

//Player* loadXm(char* path) {
//    Player::registerAdapter(SynthAdapter::Info);
//    var player = (Player*)Player::creator(NULL);
//    var adapter = player->addAdapter(SynthAdapter::Info.id, 1);
//    adapter->player(player);
//    var soundBank = getSoundBank();
//    var xmLoader = NEW_(XmLoader, player, soundBank);
//    var error = xmLoader->load(path);
//    if (error != 0) {
//        printf("Error loading xm (%d)", error);
//    }
////    else {
////#ifdef _DEBUG
////        for (var i = 0; i < xmLoader->patterns().length(); i++) {
////            var buffer = xmLoader->printPattern(i);
////            printf("Pattern #%d\n%s", (i+1), buffer);
////            FREE(buffer);
////        }
////#endif
////    }
//    player->sequences().forEach([](void* item, UINT32 ix, Collection* col, void* args) {
//        var seq = (Sequence*)item;
//        var text = seq->print();
//        printf("Seq #%d\n%s", ix+1, text);
//        FREE(text);
//        return 1;
//    });
//    DEL_(xmLoader);
//
//    ((SynthDevice*)adapter->getDevice(0))->synth()->setVoiceCount(2);
//    ((SynthDevice*)adapter->getDevice(3))->synth()->setVoiceCount(3);
//    player->masterChannel()->assign(0, (Sequence*)player->sequences().get(0));
//
//    return player;
//}
//
//int testLoadXm() {
//    const char* fileName = "min";
//    var fileNameWithExtension = str_format("%s.xm", fileName);
//    var player = loadXm(fileNameWithExtension);
//    FREE(fileNameWithExtension);
//    const float bpm = 1.5f * 127.0f / 3.75f;
//    player->setRefreshRate(bpm);
//#ifdef SAVE_TO_WAVE
//    saveToWave(str_format("%s.wav", fileName), player);
//#else
//    player->masterChannel()->loopCount(2);
//    player->masterChannel()->reset();
//    playback(player, 2);
//#endif
//    DEL_(player);
//    return 0;
//}