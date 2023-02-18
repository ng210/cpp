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
		Knob
	} PotCtrlType;


	class PotCtrl : public Ctrl {
		static WndClass wndClass_;
		static HFONT font_;
		int size_;
		SIZE labelSize_, levelSize_;
		bool isCapturing_;
		void updateClientRect(bool resize = true);
		float angle_;
		HBITMAP bitmapVPot1_;
	protected: PROP_R(PotCtrlType, type);
	protected: PROP_R(PotBase*, pot);
	protected: PROP(bool, showLabel);
	protected: PROP(bool, showValue);
	protected: TCHAR label_[32];
	protected: TCHAR valueText_[32];
	protected:
		void drawHPotmeter(HDC hdc, float value);
		void drawVPotmeter(HDC hdc, float value);
		void drawKnob(HDC hdc, float value);
	public:
		PotCtrl();
		~PotCtrl();

		void create(Window* parent, char* name);

		void setColors(DWORD background, DWORD foreground, DWORD frame, DWORD text);
		void label(TCHAR* lbl);
		void setSize(int size);
		void type(PotCtrlType t);
		void pot(PotBase* pot);

		LRESULT onLeftDown(POINT& pos, WPARAM state);
		LRESULT onLeftUp(POINT& pos, WPARAM state);
		LRESULT onRightUp(POINT& pos, WPARAM state);
		LRESULT onMouseMove(POINT& pos, POINT& delta, WPARAM state);
		LRESULT onPaint();

		static HBRUSH frameBrush_, backgroundBrush_, foregroundBrush_;
		static COLORREF textColor_;
	};

}

#endif