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

    typedef struct GenericDrumCtrlsPots {
        PotF gdFre[6];
        PotF8 gdAmp[6];
    } GenericDrumCtrlsPots;

    typedef struct GenericDrumCtrls {
        Pot type;
        DahrCtrls dahr[4];
        FltCtrls flt[2];
        GenericDrumCtrlsPots pots;
    } GenericDrumCtrls;

    typedef enum GenericDrumCtrlIds {
        gdType,
        gdDahr1Amp, gdDahr1Del, gdDahr1Atk, gdDahr1Hld, gdDahr1Rel,
        gdDahr2Amp, gdDahr2Del, gdDahr2Atk, gdDahr2Hld, gdDahr2Rel,
        gdDahr3Amp, gdDahr3Del, gdDahr3Atk, gdDahr3Hld, gdDahr3Rel,
        gdDahr4Amp, gdDahr4Del, gdDahr4Atk, gdDahr4Hld, gdDahr4Rel,

        gdFlt1Cut, gdFlt1Res, gdFlt1Mode,
        gdFlt2Cut, gdFlt2Res, gdFlt2Mode,

        gdFreq1, gdFreq2, gdFreq3, gdFreq4, gdFreq5, gdFreq6,
        gdAmp1, gdAmp2, gdAmp3, gdAmp4, gdAmp5, gdAmp6
    } GenericDrumCtrlIds;

    typedef enum DrumTypes {
        BassDrumType = 0,
        SnareDrumType,
        HihatType,
        ClapType,
        DefaultDrumType
    } DrumTypes;

    #define GenericDrumCtrlCount sizeof(GenericDrumCtrls) / sizeof(PotBase)

    class GenericDrum;
    typedef void (GenericDrum::*RENDER)(float* buffer, int start, int end);

    class GenericDrum : public Module {
        void renderDefault(float* buffer, int start, int end);
        void renderHihat(float* buffer, int start, int end);
        void renderClap(float* buffer, int start, int end);
    protected: PROP(int, attribute1);
    protected: float runFilter(Flt* flt, float cut, float input);
    public:
        GenericDrumCtrls controls;
        double noise;
        // osc
        double timers[6];
        // envelope
        Dahr envelopes[4];
        // Filter
        Flt filters[2];

        GenericDrum();
        virtual ~GenericDrum();
        RENDER render;

        bool isActive();
        void setGate(byte velocity);

        static int typeSetter(void*, DrumTypes, void* = NULL);

        static int programSetter(void* obj, int ix, void* = NULL);
    };

    //class Hihat : public GenericDrum {
    //protected: PROP(bool, isOpen);
    //public:
    //    Hihat();
    //    void program(int prgId);
    //    void render(float* buffer, int start, int end);
    //};
    //class MidTom : public GenericDrum {
    //public:
    //    void program(int prgId);
    //};
    //class HighTom : public GenericDrum {
    //public:
    //    void program(int prgId);
    //};

    //typedef struct DrumCtrls {
    //    GenericDrumCtrls bd, sd, hh, lt, mt, ht, cp;
    //} DrumCtrls;

    typedef enum DrumsNotes {
        drBD = 1,
        drSD = 2,
        drCH = 3,
        drOH = 4,
        drLT = 5,
        drMT = 6,
        drHT = 7,
        drCP = 8
    } DrumsNotes;

    #define DrumsCount 8

    class Drums : public Module {
        static Soundbank* defaultSoundbank_;
    protected: GenericDrum drums_[DrumsCount];
        //void renderGenericDrum(GenericDrum& drum, float* buffer, int start, int end);
        //void renderBassDrum(float* buffer, int start, int end);
        //void renderSnareDrum(float* buffer, int start, int end);
        //void renderClosedHihat(float* buffer, int start, int end);
        //void renderOpenHihat(float* buffer, int start, int end);
        //void renderLowTom(float* buffer, int start, int end);
        //void renderMidTom(float* buffer, int start, int end);
        //void renderHighTom(float* buffer, int start, int end);
        //void renderClap(float* buffer, int start, int end);
    public:
        GenericDrum* drums;

        Drums();
        virtual ~Drums();
        void initializeFromStream(byte** pData);

        //void soundbank(Soundbank* sb);
        float* getOutput(int id);
        void setControl(int id, S value);
        void run(int start, int end);
        int drumId2note(int id);
        void setNote(byte note, byte velocity);
        Soundbank* createSoundbank();
        

        Soundbank* getDefaultSoundbank();

        static void prepare();
        static void cleanUp();

        static int soundbankSetter(void*, Soundbank*, void* = NULL);
        static int programSetter(void*, int, void* = NULL);
    };
}

#endif
