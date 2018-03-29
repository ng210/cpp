#ifndef __CHARTCTRL_H
#define __CHARTCTRL_H

#include <windows.h>
#include "base/base.h"
#include "chartctrl_res.h"
#include "CustomCtrl.h"
#include "Collection\IndexedCollection.h"
//#include "ToolBoxCtrl.h"

typedef struct {
	size_t iScale;
	size_t iPixelPerUnit;
	int iOffset;
	size_t iUnit;
	size_t iStepSize;
	SCROLLINFO scrollInfo;
} CHARTSCROLLINFO;

typedef struct {
	size_t iScale;			// scale
	size_t iOffsetInPixels;	// offset for scrolling
	size_t iOffsetInUnits;	// offset for drawing points
	SCROLLINFO scrollInfo;
} CHARTAXISINFO;

class ChartCtrl : public CustomCtrl {
	// handle of the tool box child window
	PROP_R(protected, HWND, ToolBox, h);

	static const size_t TOOLBOX_ID;
	static const size_t PixelPerUnitX;
	static const size_t PixelPerUnitY;

protected:
	// toolbox items
	PROP_R(protected, size_t, ToolBoxHeight, i);
	//PROP_R(protected, IndexedCollection*, toolBoxItems, );
	// chart's size in pixels (0, 0, width, heigth)
	RECT m_size;
	// chart axis info
	CHARTAXISINFO m_axisX, m_axisY;
	// associated data
	IndexedCollection* m_data;
	// data length
	size_t m_iLength;
	// data value range
	size_t m_iRange;
	// index of first visible data element
	IndexedCollection *m_offsetIndices;

	static int seekData(void *pParent, void* pMember, int ix, void* pArgs);

	//// data for general scrolling
	//PROP_R(protected, CHARTSCROLLINFO, xScrollInfo, );
	//PROP_R(protected, CHARTSCROLLINFO, yScrollInfo, );
	//// size of chart in units
	//PROP_R(protected, size_t, Width, i);
	//PROP_R(protected, size_t, Height, i);
	//// pixel per unit
	//PROP(protected, size_t, PixelPerUnitX, i);
	//PROP(protected, size_t, PixelPerUnitY, i);

	LRESULT wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	//virtual char* ClassName();
	void onPaint();
	void drawSeries(IndexedCollection* pSeries, int i, RECT* lpRect);
	void setScroll();
	//virtual void setScroll();
public:
	ChartCtrl(HWND hParent, DWORD id, LPCUSTOMWNDPROC wndProc);
	virtual ~ChartCtrl(void);
	int init(
		HWND hParent,			// parent window
		DWORD id,				// window id (child id)
		LPCUSTOMWNDPROC wndProc	// window function
	);

/*
	inline int toDataX(size_t cx) { return (cx + m_xScrollInfo.iOffset) / m_xScrollInfo.iStepSize + m_xScrollInfo.iUnit; };
	inline int toDataY(size_t cy) { return (cy + m_yScrollInfo.iOffset) / m_yScrollInfo.iStepSize + m_yScrollInfo.iUnit; };
	inline int toViewX(size_t x) { return (x - m_xScrollInfo.iUnit) * m_xScrollInfo.iStepSize - m_xScrollInfo.iOffset; };
	inline int toViewY(size_t y) { return (y - m_yScrollInfo.iUnit) * m_yScrollInfo.iStepSize - m_yScrollInfo.iOffset; };
*/
	void setSize(size_t iWidth, size_t iHeight);
	void setData(IndexedCollection *data, size_t iLength, size_t iRange);
	void scale(size_t iScaleX, size_t iScaleY);
	void scroll(UINT fnBar, WPARAM wParam);
	static INT_PTR CALLBACK toolBoxProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
};

#endif