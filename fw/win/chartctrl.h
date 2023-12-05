#ifndef __CHART_CTRL_H
#define __CHART_CTRL_H

#include "win/ctrl.h"

NS_FW_BASE_USE
NS_FW_WIN_BEGIN

#define SelectionTimerEvent 0x5e1e

typedef bool (CHART_DATA_GETTER)(int x, int y1, int y2, POINT* p, Collection* dataSource);

typedef enum CHART_INSERT_MODE {
	ChartInsertModeFree
} CHART_INSERT_MODE;

typedef enum CHART_DRAG_MODE {
	ChartDragModeFree
} CHART_INTERACTION_MODE;

typedef enum CHART_DRAW_MODE {
	ChartDrawModeLine
} CHART_DRAW_MODE;

typedef enum DRAGGING_STATE {
	DRAGGING_STARTS,
	DRAGGING_ACTIVE,
	DRAGGING_INACTIVE
} DRAGGING_STATE;

typedef struct ChartSettings {
	POINT unit1;
	POINT unit2;
	bool showGrid;
	dword backgroundColor;
	dword foregroundColor;
	dword gridColor;
	//dword grid2Color;
	dword lineColor;
	dword textColor;
	CHART_INSERT_MODE insertMode;
	CHART_DRAG_MODE dragMode;
	CHART_DRAW_MODE drawMode;
	int lineSizeX;
	int lineSizeY;
	int pageSizeX;
	int pageSizeY;
	int dragSpeed;
	float scaleSpeed;
} ChartSettings;

class ChartCtrl : public Ctrl {
	static WndClass WndClass_;
	static ChartSettings defaultSettings_;

protected: PROP_R(Collection*, dataSource);
		 CHART_DATA_GETTER* dataGetter_;
protected:
	ChartSettings settings_;

	RECT chartSize_;

	float scaleX_;
	float scaleY_;
	float scaleRangeX_[2];
	float scaleRangeY_[2];
	int stepX_, stepX2_;
	int stepY_, stepY2_;
	int scrollPosX_;
	int scrollPosY_;
	int offsetX_, offsetX2_;
	int offsetY_, offsetY2_;
	RECT originalVirtualSize_;

	POINT clientPos_;
	int keys_;

	POINT cursor_;
	POINT highLightPos_;
	bool isHighLightVisible_;
	POINT selectionPos1_;
	POINT selectionPos2_;
	RECT selectionRect_;
	bool hasSelection_;

	HDC hDC_;
	HBITMAP hBitmap_;
	HDC hSelectionDC_;
	HBITMAP hSelectionBitmap_;
	HBRUSH backgroundBrush_;
	HBRUSH foregroundBrush_;
	HPEN grid1Pen_;
	HPEN grid2Pen_;

	DRAGGING_STATE draggingState_;
	POINT dragStartPoint_;
	//float dragVector_[2];
	LRESULT startDragging(POINT& pos);
	LRESULT doDragging(POINT& clientPos, POINT& delta, int keys);
	LRESULT stopDragging();

	DRAGGING_STATE selectionState_;
	LRESULT startSelection(POINT& pos);
	LRESULT doSelection(POINT& clientPos, POINT& delta, int keys);
	LRESULT stopSelection();

	LRESULT scale(float f, POINT& pos, int keys);
	void focusOn(POINT& clientPos);

	int scrollWindow(ScrollInfo& scrollInfo, int mode, int pos, int nBar, bool update = true);
	void updateOffsetX();
	void updateOffsetY();
	
	bool fromClientPoint(POINT* clientPos, POINT* pos = NULL);
	void updateHighlight(POINT& pos, bool invalidate);

public:

	ChartCtrl();
	ChartCtrl(ChartSettings* settings);
	virtual ~ChartCtrl();
	void initialize(ChartSettings* settings);

	WndClass getWindowClass();
	void createWindow(Window* parent, char* name, LONG style = NULL, DWORD exStyle = NULL);

	void createBackBuffer(RECT& rect);
	void setVirtualSize(int width, int height);
	void setColors();
	int setScaleX(float x);
	int setScaleY(float y);
	void dataBind(Collection* dataSource, CHART_DATA_GETTER* chartGetData);

	LRESULT onCreated();
	LRESULT onSize(RECT& rect, WPARAM state);
	LRESULT onPaint();
	LRESULT onHScroll(WPARAM wParam, LPARAM lParam);
	LRESULT onVScroll(WPARAM wParam, LPARAM lParam);

private:
	static LRESULT onLeftUpProc(Window* wnd, POINT& pos, WPARAM wParam);
	static LRESULT onLeftDownProc(Window* wnd, POINT& pos, WPARAM wParam);
	static LRESULT onRightUpProc(Window* wnd, POINT& pos, WPARAM wParam);
	static LRESULT onRightDownProc(Window* wnd, POINT& pos, WPARAM wParam);
	static LRESULT onMiddleUpProc(Window* wnd, POINT& pos, WPARAM wParam);
	static LRESULT onMiddleDownProc(Window* wnd, POINT& pos, WPARAM wParam);

	static LRESULT onMouseMoveProc(Window* wnd, POINT& pos, POINT& delta, WPARAM wParam);
	static LRESULT onWheelProc(Window* wnd, POINT& pos, WPARAM wParam);

	static void selectionTimerProc(HWND unnamedParam1, UINT unnamedParam2, UINT_PTR unnamedParam3, DWORD unnamedParam4);
	//void convertFromXY();
	//void convertToXY();
	//float convertToPixel(int v);
};

NS_FW_WIN_END

#endif