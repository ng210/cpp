#ifndef __DRUMS_H
#define __DRUMS_H

#include "collection/array.h"
#include "synth/src/module/generic-drum.h"

NS_FW_BASE_USE
namespace SYNTH {

    #define DrumsCount 8
    typedef struct DrumsValues_ {
        GenericDrumValues drums[DrumsCount];
    } DrumsValues;

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

    class Drums : public Module {
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
        ~Drums();

        Value* getValues();  // ???
        float* getOutput(int id);
        bool isActive();
        void run(int start, int end);
        int drumId2note(int id);
        void setNote(byte note, byte velocity);
    };
}

#endif
