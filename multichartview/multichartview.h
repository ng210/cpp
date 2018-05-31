#ifndef __MULTICHARTVIEW_H
#define __MULTICHARTVIEW_H

#include "win/ctrl.h"
#include "dataseries.h"

NS_FW_WIN_BEGIN

#define MULTICHARTVIEW_TOOLBARHEIGHT 24

enum CHARTPAINTMODE {
	CHARTPAINTMODE_NONE,
	CHARTPAINTMODE_LINE,
	CHARTPAINTMODE_BAR,
	CHARTPAINTMODE_POINT
};

typedef struct CHARTCONFIGURATION_ {
	DWORD backgroundColor;
	POINT stepSize;
	POINT grid;
} CHARTCONFIGURATION;

typedef struct CHARTCHANNELINFO_ {
	float min;
	float max;
	COLORREF color;
	CHARTPAINTMODE paintMode;
} CHARTCHANNELINFO;

class MultiChartView : 	public Ctrl {
	static ATOM class_;
	static WNDCLASSEX wndClassEx_;
protected: PROP_R(float, scaleX);
protected: PROP_R(int, offsetX);
protected: PROP_R(int, maxX);
protected: PROP_R(float, scaleY);
protected: PROP_R(int, offsetY);
protected: PROP_R(Array, selection);
//protected: PROP_R(float, minY);
//protected: PROP_R(float, maxY);

protected: PROP_R(CHARTCHANNELINFO*, channels);
protected: PROP_REF(CHARTCONFIGURATION, configuration);
protected:
	DataSeries* dataSeries_;
	int channelCount_;
	int isSelecting_;
	int dragStartX_;
	RECT selectRect_;
	HBITMAP hSelectionBitmap_;
	HDC selectHdc_;

	LRESULT CALLBACK wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
public:
	MultiChartView(Window* parent, size_t ctrlId);
	virtual ~MultiChartView();

	void setDataSource(DataSeries* source, int channelCount, CHARTCHANNELINFO* channels);
	void select();
	void draw(LPARAM lParam);

	//int onCreate();
	int onPaint(HDC hdc, PAINTSTRUCT* ps);
	void onSize(WPARAM wParam, LPARAM lParam);
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