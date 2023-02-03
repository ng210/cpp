#ifndef __DRUMS_H
#define __DRUMS_H

#include "collection/array.h"
#include "synth/src/module/module.h"
#include "synth/src/elem/dahr.h"
#include "synth/src/elem/adsr.h"
#include "synth/src/elem/lfo.h"
#include "synth/src/elem/osc.h"
#include "synth/src/elem/flt.h"

NS_FW_BASE_USE
namespace SYNTH {

    typedef struct GenericDrumCtrls {
        DahrCtrls dahr1, dahr2, dahr3, dahr4;
        FltCtrls flt1, flt2;
    } GenericDrumCtrls;

    class GenericDrum : public Module {
        //GenericDrumCtrls controls_;
    protected: PROP(int, attribute1);
    protected: float runFilter(Flt* flt, float cut, float input);
    public:
        double noise;
        // osc
        int freCount;
        float fre[6];
        float amp[6];
        double timers[6];
        // envelope
        int envCount;
        Dahr envelopes[4];
        // Filter
        Flt filters[2];

        GenericDrum();
        virtual ~GenericDrum();

        void assignControls(PotBase* controls);
        //void connectInput(int id, float* buffer);
        //PotBase* getControl(int id);
        //float* getInput(int id);
        //float* getOutput(int id);
        void initialize(byte** pData);
        bool isActive();
        void setProgram(int prgId);
        void setGate(byte velocity);
        void render(float* buffer, int start, int end);
    };

    class OpenHihat : public GenericDrum {
    public:
        void setProgram(int prgId);
    };

    typedef struct DrumCtrls {
        GenericDrumCtrls bd, sd, oh, ch, lt, mt, ht, cp;
    } DrumCtrls;

    typedef enum DrumsNotes {
        drBD = 1,
        drSN = 2,
        drOH = 3,
        drCH = 4,
        drLT = 5,
        drMT = 6,
        drHT = 7,
        drCP = 8
    } DrumsNotes;

    typedef enum DrumCtrlId {
        bdTone = ((size_t)&((DrumCtrls*)0)->bd.dahr3.amp) / sizeof(PotBase),
        bdTune = ((size_t)&((DrumCtrls*)0)->bd.dahr2.dc) / sizeof(PotBase),
        bdDecay = ((size_t)&((DrumCtrls*)0)->bd.dahr1.rel) / sizeof(PotBase),
        sdDecay = ((size_t) & ((DrumCtrls*)0)->sd.dahr1.rel) / sizeof(PotBase),
        sdTune = ((size_t) & ((DrumCtrls*)0)->sd.dahr3.dc) / sizeof(PotBase),
        sdSnappy = ((size_t) & ((DrumCtrls*)0)->sd.dahr4.amp) / sizeof(PotBase),
        sdTone = ((size_t) & ((DrumCtrls*)0)->sd.flt1.cut) / sizeof(PotBase),
        hhTone = ((size_t) & ((DrumCtrls*)0)->oh.flt1.cut) / sizeof(PotBase),
        ohDecay = ((size_t) & ((DrumCtrls*)0)->oh.dahr1.hld) / sizeof(PotBase)
    } DrumCtrlId;

    typedef enum DrumPrgId {
        prgBD808,
        prgBD909,
        prgBD707,
        prgSD808,
        prgSD909,
        prgSDa0a,
        prgHH808,
        prgHHa0a,
        prgLT808,
        prgMT808,
        prgHT808,
        prgCP808
    } DrumPrgId;


    class Drums : public Module {
    protected: DrumCtrls controls_;
    protected: PROP_R(GenericDrum, bassDrum);
    protected: PROP_R(GenericDrum, snareDrum);
    protected: PROP_R(GenericDrum, closedHihat);
    protected: PROP_R(OpenHihat, openHihat);
    protected: PROP_R(GenericDrum, lowTom);
    protected: PROP_R(GenericDrum, midTom);
    protected: PROP_R(GenericDrum, highTom);
    protected: PROP_R(GenericDrum, clap);
             //void renderGenericDrum(GenericDrum& drum, float* buffer, int start, int end);
             void renderHihat(GenericDrum* hihat, float* buffer, int start, int end);
             void renderBassDrum(float* buffer, int start, int end);
             void renderSnareDrum(float* buffer, int start, int end);
             void renderClosedHihat(float* buffer, int start, int end);
             void renderOpenHihat(float* buffer, int start, int end);
             void renderLowTom(float* buffer, int start, int end);
             void renderMidTom(float* buffer, int start, int end);
             void renderHighTom(float* buffer, int start, int end);
             void renderClap(float* buffer, int start, int end);
    public:
        Drums();
        virtual ~Drums();

        //void connectInput(int id, float* buffer);
        //void initialize(byte** pData);
        float* getOutput(int id);
        void setControl(int id, S value);
        void run(int start, int end);
        void setNote(byte note, byte velocity);
    };
}

#endif
