#ifndef __INPUT_CTRL_H
#define __INPUT_CTRL_H

#include "win/ctrl.h"
#include "player/src/input.h"

NS_FW_BASE_USE
NS_FW_WIN_USE
using namespace PLAYER;
namespace PLAYER {
	typedef enum InputCtrlType {
		HPotmeter,
		VPotmeter,
		Knob,
		Number
	} InputCtrlType;

	class InputCtrl : public Ctrl {
		static char* windowClassName_;
		static ATOM windowClass_;

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
		HBRUSH frameBrush_, backgroundBrush_, foregroundBrush_;
		HPEN pen_;
		COLORREF textColor_;

	protected: PROP_R(InputCtrlType, type);
	protected: PROP_R(Input*, input);
	protected: PROP(bool, showLabel);
	protected: PROP(bool, showValue);
	protected: TCHAR label_[32];
	protected: TCHAR valueText_[32];
	protected: PROP(int, mouseSpeed1);
	protected: PROP(int, mouseSpeed2);
	protected: PROP(int, mouseSpeed3);
	protected: PROP(int, mouseCounter);
	protected:
		void drawHPotmeter(HDC hdc, float value);
		void drawVPotmeter(HDC hdc, float value);
		void drawKnob(HDC hdc, int& x, int& y, float value);
		void drawNumber(HDC hdc, int& x, int& y, float value);
	public:
		InputCtrl();
		~InputCtrl();
		
		static void cleanup();

		char* const registerWindowClass();
		void create(Window* parent, char* name/*, LONG style = 0, DWORD exStyle = 0 */ );

		void setColors(DWORD* colors);
		void label(TCHAR* lbl);
		void setSize(int size);
		void type(InputCtrlType t);
		void input(Input* input);

		LRESULT onCreated();
		LRESULT onPaint();
		LRESULT onEraseBkgnd(HDC hDC);

		static int setter(void*, Value, void* = NULL);

		static MOUSEMOVEEVENTPROC onMouseMoveProc;
		static MOUSEEVENTPROC onLeftDownProc, onLeftUpProc, onRightUpProc;
	};

}

#endif