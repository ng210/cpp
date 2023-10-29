#ifndef __SEQ_CTRL_H
#define __SEQ_CTRL_H

#include "win/ctrl.h"
//#include "win/buttonctrl.h"
//#include "win/comboboxctrl.h"
//#include "win/staticctrl.h"
#include "player/src/frames.h"

NS_FW_BASE_USE
NS_FW_WIN_USE
using namespace PLAYER;

namespace SYNTH_APP {

    typedef enum DrawType {
        DrawBars,
        DrawLines
    } DrawType;

    typedef void(DRAW_METHOD)(HDC, RECT*, int x, int y);

    typedef struct SequenceCtrlConfig {
        int backgroundColor;
        int gridColor;
        int lineColor;
        int lineWidth;
        DrawType type;
    } SequenceCtrlConfig;

    class SequenceCtrl : public Ctrl
    {
        static WndClass WndClass_;
        static SequenceCtrlConfig DefaultConfig_;

        PROP(Frames*, frames);
        PROP(FRAMES_GET_CHANNEL_ID*, getChannelId);

        PROP(HBRUSH, backgroundBrush);
        PROP(HBRUSH, gridBrush);
        PROP(HBRUSH, lineBrush);
        PROP(SequenceCtrlConfig, config);
        DRAW_METHOD drawMethod_;

    public:
        SequenceCtrl();
        ~SequenceCtrl();
        void create(Window* parent, char* name);

        void update();

        LRESULT onCreate();
        LRESULT onPaint();
        
        void insertCommand(int time, int type, byte* command);
        void removeCommand(int time, int type, byte* command);


        static MOUSEEVENTPROC onLeftUpProc;
        static MOUSEEVENTPROC onRightUpProc;

    };
}

#endif