#ifndef __DRUMS_H
#define __DRUMS_H

#include "synth/src/module/generic-drum.h"

NS_FW_BASE_USE
namespace SYNTH {

    #define DRUMS_COUNT 8
    typedef struct DrumsStageValues_ {
        Value bnkId;
        Value prgId;
        Value pan;
        Value send[4];
    } DrumsStageValues;

    typedef struct DrumsValues_ {
        DrumsStageValues drums[DRUMS_COUNT];
    } DrumsValues;
    
    typedef struct DrumStage_ {
        float* buffer[2];
        Module* effect;
    } DrumStage;

    typedef enum DrumsNotes {
        drBD = 1,
        drSD = 3,
        drCH = 5,
        drOH = 6,
        drLT = 8,
        drMT = 10,
        drHT = 12,
        drCP = 14
    } DrumsNotes;

    class Drums : public Module {
    protected: DrumStage stages_[4];

    protected: GenericDrum drums_[DRUMS_COUNT];
        //void renderGenericDrum(GenericDrum& drum, float* buffer, int start, int end);
        //void renderBassDrum(float* buffer, int start, int end);
        //void renderSnareDrum(float* buffer, int start, int end);
        //void renderClosedHihat(float* buffer, int start, int end);
        //void renderOpenHihat(float* buffer, int start, int end);
        //void renderLowTom(float* buffer, int start, int end);
        //void renderMidTom(float* buffer, int start, int end);
        //void renderHighTom(float* buffer, int start, int end);
        //void renderClap(float* buffer, int start, int end);
    protected: DrumsValues values_;
    public:
        inline GenericDrum* drum() { return drums_; };

        Drums();
        ~Drums();

        Value* getValues();

        void connectEffect(Module* effect);
        bool isActive();
        void run(int start, int end, BufferWriteModes mode);

        void setNote(byte note, byte velocity);
    };
}

#endif
