#ifndef __MULTICHARTVIEW_H
#define __MULTICHARTVIEW_H

#include "win/ctrl.h"
#include "dataseries.h"
#include "toolbar.h"

NS_FW_WIN_BEGIN

enum CHART_PAINTMODE {
	CHART_PAINTMODE_NONE,
	CHART_PAINTMODE_LINE,
	CHART_PAINTMODE_BAR,
	CHART_PAINTMODE_BOX,
	CHART_PAINTMODE_CUSTOM1,
	CHART_PAINTMODE_CUSTOM2,
	CHART_PAINTMODE_CUSTOM3,
	CHART_PAINTMODE_CUSTOM4
};

typedef void CHART_CUSTOM_DRAW(HDC hdc, RECT* rect, int ix);

typedef struct CHARTCONFIGURATION_ {
	DWORD backgroundColor;
	POINT stepSize;
	POINT grid;
	BOOL canDrawWithoutSelection;
	CHART_CUSTOM_DRAW* customDraw[4];
} CHARTCONFIGURATION;

typedef struct CHARTCHANNELINFO_ {
	const char* label;
	float min;
	float max;
	COLORREF color;
	CHART_PAINTMODE paintMode;
} CHARTCHANNELINFO;

class MultiChartView : 	public Ctrl {
	static ATOM class_;
	static WNDCLASSEX wndClassEx_;
protected: PROP_R(float, scaleX);
protected: PROP_R(float, scaleY);
protected: PROP_R(int, offsetX);
protected: PROP_R(int, offsetY);
protected: PROP_R(int, maxPosX);
protected: PROP_R(int, maxPosY);
protected: PROP_R(CHARTCHANNELINFO*, channels);
protected: PROP_R(int, channelCount);
protected: PROP_REF(CHARTCONFIGURATION, configuration);
protected: PROP_R(Array, selection);
protected: PROP_R(int, selectedChannel);
protected:
	DataSeries* dataSeries_;
	int isSelecting_;
	int dragStartX_;
	int lastDrawX_;
	double lastDrawY_;

	RECT selectRect_;
	HDC selectHdc_;
	HBITMAP hSelectionBitmap_;
	HDC hdc_;
	HBITMAP hBitmap_;
	HBRUSH hBackgroundBrush_;
	HBRUSH hGridBrush_;
	HPEN hGridPen_;

	MultiChartViewToolbar* toolbar_;

	LRESULT CALLBACK wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
public:
	MultiChartView(Window* parent, size_t ctrlId);
	virtual ~MultiChartView();
	void create(Window* parent, char* name);

	void setDataSource(DataSeries* source, int channelCount, CHARTCHANNELINFO* channels);
	void select();
	void draw(LPARAM lParam);
	void drawLine(LPARAM lParam);
	void selectChannel(int chnId);
	void drawChannel(HDC hdc, int chnId, RECT* rect, int dataStart, int dataRange);
	void update();

	LRESULT onCreate();
	int onPaint(HDC hdc, PAINTSTRUCT* ps);
	LRESULT onSize(WPARAM wParam, LPARAM lParam);
	void onScrollX(WPARAM wParam, LPARAM lParam);
	void onLeftDown(WPARAM wParam, LPARAM lParam);
	void onLeftUp(WPARAM wParam, LPARAM lParam);
	void onRightDown(WPARAM wParam, LPARAM lParam);
	void onRightUp(WPARAM wParam, LPARAM lParam);
	void onMouseMove(WPARAM wParam, LPARAM lParam);
	//int onDestroy();
};

NS_FW_WIN_END

#endif