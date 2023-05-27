#ifndef __POT_CTRL_H
#define __POT_CTRL_H

#include "win/ctrl.h"
#include "synth/src/elem/pot.h"

#define SYNTH_UI synth_ui

NS_FW_BASE_USE
NS_FW_WIN_USE
using namespace SYNTH;

namespace SYNTH_UI {

	typedef enum PotCtrlType {
		HPotmeter,
		VPotmeter,
		Knob,
		Number
	} PotCtrlType;


	class PotCtrl : public Ctrl {
		static WndClass wndClass_;
		static HFONT smallFont_;
		static HFONT mediumFont_;
		static HFONT largeFont_;
		int size_;
		int fontSize_;
		SIZE labelSize_, levelSize_;
		bool isCapturing_;
		void updateClientRect(bool resize = true);
		float angle_;
		HFONT font_;
		//Functor1<void*, S> oldSet_;
		//HBITMAP bitmapVPot1_;
	protected: PROP_R(PotCtrlType, type);
	protected: PROP_R(PotBase*, pot);
	protected: PROP(bool, showLabel);
	protected: PROP(bool, showValue);
	protected: TCHAR label_[32];
	protected: TCHAR valueText_[32];
	protected: PROP(int, mouseSpeed1);
	protected: PROP(int, mouseSpeed2);
	protected:
		void drawHPotmeter(HDC hdc, float value);
		void drawVPotmeter(HDC hdc, float value);
		void drawKnob(HDC hdc, int& x, int& y, float value);
		void drawNumber(HDC hdc, float value);
	public:
		PotCtrl();
		~PotCtrl();

		void create(Window* parent, char* name);

		void setColors(DWORD background, DWORD foreground, DWORD frame, DWORD text);
		void setColors(DWORD* colors);
		void setColors(HBRUSH background, HBRUSH foreground, HBRUSH frame, HPEN pen, COLORREF text);
		void label(TCHAR* lbl);
		void setSize(int size);
		void type(PotCtrlType t);
		void pot(PotBase* pot);

		static MOUSEEVENTPROC onLeftDownProc, onLeftUpProc, onRightUpProc;
		static MOUSEMOVEEVENTPROC onMouseMoveProc;
		LRESULT onPaint();
		LRESULT onEraseBkgnd(HDC hDC);

		HBRUSH frameBrush_, backgroundBrush_, foregroundBrush_;
		HPEN pen_;
		COLORREF textColor_;

		static SetterFunc setter;
		static SetterFunc setterF8;
	};

}

#endif