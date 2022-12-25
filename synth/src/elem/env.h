#ifndef __ENV_H
#define __ENV_H

#include "elem.h"

NS_FW_BASE_USE
namespace SYNTH {

    typedef enum class EnvPhase {
        Up = 1,
        Atk = 2,
        Dec = 3,
        Sus = 4,
        Down = 5,
        Rel = 6,
        Idle = 7
    } EnvPhase;

    typedef struct EnvCtrls_ : ElemCtrls {
        PotF dc;
        Pot atk;
        Pot dec;
        PotF8 sus;
        Pot rel;
    } EnvCtrls;

    #define EnvCtrlCount (sizeof(EnvCtrls) / sizeof(Pot))

    class Env : public Elem {
    private: PROP_R(EnvCtrls*, controls);
    private: byte gate;
    private: float velocity;
    private: PROP_R(int, ticks);
    private: double timer;
    private: double rate;
    private: PROP_R(EnvPhase, phase);
    public:
        Env();

        void assignControls(PotBase* controls);
        void setFromStream(byte* stream);
        float run(Arg params = (void*)NULL);

        void setGate(byte velocity);
        inline bool isActive() { return phase_ < EnvPhase::Idle; }

        static void initialize(float samplingRate);
    };
}
#endif