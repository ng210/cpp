#ifndef __ENV_H
#define __ENV_H

#include "mdl.h"

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

    typedef struct EnvCtrls_ : MdlCtrls {
        PotF  dc;
        PotF8 atk;
        PotF8 dec;
        PotF8 sus;
        PotF8 rel;
    } EnvCtrls;

    #define EnvCtrlCount (sizeof(EnvCtrls) / sizeof(Pot))

    class Env : public Mdl {
    private: PROP_R(EnvCtrls*, controls);
    private: byte gate;
    private: float velocity;
    private: PROP_R(int, ticks);
    private: double timer;
    private: double rate;
    private: PROP_R(EnvPhase, phase);
    public:
        Env();

        void assignControls(Pot* controls);
        void setFromStream(byte* stream);
        float run(void* params);

        void setGate(byte velocity);
        inline bool isActive() { return phase_ < EnvPhase::Idle; }
    };
}
#endif