#include "ChartCtrl.h"
#include "Collection\SArray.h"

const size_t ChartCtrl::TOOLBOX_ID = 100;
const size_t ChartCtrl::PixelPerUnitX = 16;
const size_t ChartCtrl::PixelPerUnitY = 10;

ChartCtrl::ChartCtrl(HWND hParent, DWORD id, LPCUSTOMWNDPROC wndProc) {
	init(hParent, id, wndProc);
}


ChartCtrl::~ChartCtrl(void) {
	//DEL(m_toolBox);
	DEL(m_offsetIndices);
}

int ChartCtrl::init(HWND hParent, DWORD id, LPCUSTOMWNDPROC wndProc) {
	if (wndProc == NULL) {
		wndProc = (LPCUSTOMWNDPROC)&ChartCtrl::wndProc;
	}
	memset(&m_axisX, 0, sizeof(CHARTAXISINFO));
	m_axisX.iScale = 1;
	m_axisX.scrollInfo.cbSize = sizeof(SCROLLINFO);
	m_axisX.scrollInfo.fMask = SIF_ALL;
	memset(&m_axisY, 0, sizeof(CHARTAXISINFO));
	m_axisY.iScale = 1;
	m_axisY.scrollInfo.cbSize = sizeof(SCROLLINFO);
	m_axisY.scrollInfo.fMask = SIF_ALL;

	int iRet = CustomCtrl::init(hParent, id, wndProc);

	NEW(m_offsetIndices, SArray, 16);
	setData(NULL, 0, 0);

	// set size to 0
	//setSize(0, 0, 0, 0);

	//memset(&m_xScrollInfo, 0, sizeof(CHARTSCROLLINFO));
	//m_xScrollInfo.scrollInfo.cbSize = sizeof(SCROLLINFO);
	//m_xScrollInfo.scrollInfo.fMask = SIF_ALL;
	//m_xScrollInfo.iScale = pInfo->iScaleX;
	//m_xScrollInfo.iPixelPerUnit = pInfo->iPixelPerUnitX;

	//memset(&m_yScrollInfo, 0, sizeof(CHARTSCROLLINFO));
	//m_yScrollInfo.scrollInfo.cbSize = sizeof(SCROLLINFO);
	//m_yScrollInfo.scrollInfo.fMask = SIF_ALL;
	//m_yScrollInfo.iScale = pInfo->iScaleY;
	//m_yScrollInfo.iPixelPerUnit = pInfo->iPixelPerUnitY;

	return iRet;
}

//void ChartCtrl::setScroll() {
//	GetScrollInfo(m_hWnd, SB_HORZ, &m_xScrollInfo.scrollInfo);
//	m_xScrollInfo.scrollInfo.nMax = m_iVWidth-1;
//	m_xScrollInfo.scrollInfo.nPage = m_iWidth;
//	SetScrollInfo(m_hWnd, SB_HORZ, &m_xScrollInfo.scrollInfo, false);
//
//	GetScrollInfo(m_hWnd, SB_VERT, &m_yScrollInfo.scrollInfo);
//	m_yScrollInfo.scrollInfo.nMax = m_iVHeight-1;
//	m_yScrollInfo.scrollInfo.nPage = m_iHeight;
//	SetScrollInfo(m_hWnd, SB_VERT, &m_yScrollInfo.scrollInfo, false);
//
//	// vWidth = range * unit * zoom
//	m_xScrollInfo.iStepSize = m_xScrollInfo.iPixelPerUnit * m_xScrollInfo.iScale;
//	div_t d = div(m_xScrollInfo.scrollInfo.nPos, m_xScrollInfo.iStepSize);
//	m_xScrollInfo.iOffset = d.rem; m_xScrollInfo.iUnit = d.quot;
//
//	m_yScrollInfo.iStepSize = m_yScrollInfo.iPixelPerUnit * m_yScrollInfo.iScale;
//	d = div(m_yScrollInfo.scrollInfo.nPos, m_yScrollInfo.iStepSize);
//	m_yScrollInfo.iOffset = d.rem; m_yScrollInfo.iUnit = d.quot;
//}
//
//
//void ChartCtrl::setPos(int x, int y)
//{
//	SetWindowPos(m_hWnd, 0, x, y, 0, 0, SWP_NOSIZE|SWP_NOZORDER);
//}
//void ChartCtrl::setSize(size_t iWidth, size_t iHeight, size_t iVWidth, size_t iVHeight) {
//	int iFlag = 0;
//	if (iWidth > 0 && iWidth != m_iWidth) {
//		// new and valid width
//		m_iWidth = iWidth;
//		iFlag |= 1;
//	}
//	if (iHeight > 0 && iHeight != m_iHeight) {
//		// new and valid height
//		m_iHeight = iHeight;
//		iFlag |= 1;
//	}
//	if (iVWidth > 0 && iVWidth != m_iVWidth) {
//		// new and valid virtual width
//		m_iVWidth = iVWidth;
//		iFlag |= 2;
//	}
//	if (iVHeight > 0 && iVHeight != m_iVHeight) {
//		// new and valid virtual height
//		m_iVHeight = iVHeight;
//		iFlag |= 2;
//	}
//
//	if (iFlag & 1) {
//		// set size of panel's visible area (window)
//		SetWindowPos(m_hWnd, 0, 0, 0, m_iWidth, m_iHeight, SWP_NOMOVE|SWP_NOZORDER);
//	}
//	if (iFlag & 2) {
//		// set panel's size
//		setScroll();
//		//GetScrollInfo(m_hWnd, SB_HORZ, &m_hScrollInfo);
//		//GetScrollInfo(m_hWnd, SB_VERT, &m_vScrollInfo);
//		//if ((flag & SWP_NOMOVE) == 0)
//		//{
//		//	m_hScrollInfo.nMax = vx-1;
//		//	m_vScrollInfo.nMax = vy-1;
//		//}
//		//if ((flag & SWP_NOSIZE) == 0)
//		//{
//		//	// resize
//		//	GetClientRect(m_hWnd, &m_size);
//		//	//m_size.right -= m_size.left; m_size.left = 0;
//		//	//m_size.bottom -= m_size.top; m_size.top = 0;
//		//	m_hScrollInfo.nPage = cx;	//m_size.right;
//		//	m_vScrollInfo.nPage = cy;	//m_size.bottom;
//		//}
//		//SetScrollInfo(m_hWnd, SB_HORZ, &m_hScrollInfo, false);
//		//SetScrollInfo(m_hWnd, SB_VERT, &m_vScrollInfo, false);
//	}
//}
//

INT_PTR CALLBACK ChartCtrl::toolBoxProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	INT_PTR iRet = false;
	ChartCtrl* pParent;
	switch (uMsg) {
		case WM_INITDIALOG:
			pParent = (ChartCtrl*)lParam;
			SYS(SetWindowPos(hwndDlg,HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE));
			iRet = true;
			break;
	}
	return iRet;
}

LRESULT ChartCtrl::wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	LRESULT iRet = 0;
	RECT rect;
	switch (uMsg) {
		case WM_CREATE:
			HWND hParent;
			hParent = SYS(GetParent(hWnd));
			if (hParent != NULL) {
				// create toolbox from resource
				SYS(m_hToolBox = CreateDialogParam(m_hInst, MAKEINTRESOURCE(IDD_TOOLBOX), hWnd, toolBoxProc, (LPARAM)this));
				if (m_hToolBox != NULL) {
					SYS(GetWindowRect(m_hToolBox, &rect));
					SYS(ShowWindow(m_hToolBox, SW_SHOW));
					m_iToolBoxHeight = rect.bottom - rect.top;
					// create toolbox items
					//if (m_toolBoxItems == NULL) {
					//	;
					//}
					//// add toolbox
					//NEW(m_toolBox, ToolBoxCtrl, hWnd, TOOLBOX_ID, m_toolBoxItems);
					SYS(GetClientRect(hParent, &rect));
					setSize(rect.right, rect.bottom);
					SYS(ShowWindow(hWnd, SW_SHOW));
					break;
				}
				setScroll();
				//scroll(SB_HORZ, 0);
			}
			iRet = -1;
			break;
		case WM_PAINT:
			onPaint();
			//goto _default;
			break;
		//case WM_SIZING:
		//	break;
		case WM_HSCROLL:
			scroll(SB_HORZ, wParam);
			break;
		case WM_VSCROLL:
			scroll(SB_VERT, wParam);
			break;
		//case WM_SIZE:
		//	hParent = SYS(GetParent(hWnd));
		//	if (hParent != NULL) {
		//		SYS(GetClientRect(hParent, &rect));
		//		setSize(rect.right, rect.bottom);
		//	}
		//	goto _default;
_default:
		default:
			iRet = DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return iRet;
}


void ChartCtrl::onPaint() {
	RECT rect;
	HDC hDC = m_hDC;
	SYS(GetClientRect(m_hWnd, &rect));

	// paint toolbox

	//int iRight = rect.right;
	//rect.right = m_graph.VWidth();
	// background color
	HBRUSH hBr;
	SYS(hBr = CreateSolidBrush(RGB(0x0a, 0x10, 0x40)));
	SYS(FillRect(hDC, &rect, hBr));

	//if (m_graph.VWidth() < (size_t)iRight)
	//{
	//	// background color
	//	//HBRUSH hBr = CreateSolidBrush(RGB(0x0a, 0x10, 0x40));
	//	RECT inactiveRect = { m_graph.VWidth(), 0, iRight, rect.bottom };
	//	FillRect(hDC, &inactiveRect, (HBRUSH)COLOR_WINDOW+1);
	//}

	// draw vertical gridlines
	POINT p;
	SYS(SelectObject(hDC, GetStockObject(DC_PEN)));
	SYS(SetDCPenColor(hDC, RGB(0x22, 034, 0x48)));
	int step = ChartCtrl::PixelPerUnitX * m_axisY.iScale;
	for (size_t cx = m_axisX.iOffsetInPixels + 0; cx < m_axisX.iOffsetInPixels + rect.right; cx += step) {
		// draw a gridline
		SYS(MoveToEx(hDC, cx, 0, &p));
		SYS(LineTo(hDC, cx, rect.bottom));
	}
	// draw horizontal gridlines
	step = ChartCtrl::PixelPerUnitY * m_axisY.iScale;
	for (size_t cy = m_axisY.iOffsetInPixels; cy < m_axisY.iOffsetInPixels + rect.bottom; cy += step) {
		// draw a gridline
		SYS(MoveToEx(hDC, 0, cy, &p));
		SYS(LineTo(hDC, rect.right, cy));
	}
	// draw points
	if (m_data != NULL) {
		//HBRUSH hbr = CreateSolidBrush(RGB(0xff, 0xe0, 0x80));
		Iterator *it = m_data->begin(0);
		while (m_data->next(it)) {
			IndexedCollection *pSeries = (IndexedCollection*)it->pItem;
			drawSeries(pSeries, it->iItem, &rect);
		}
	}
	DeleteObject(hBr);
	SYS(ValidateRect(m_hWnd, NULL));
}

void ChartCtrl::drawSeries(IndexedCollection* pSeries, int i, RECT* lpRect) {
	COLORREF colors[] = {
		RGB(0xff, 0x80, 0x80), RGB(0x80, 0xff, 0x80), RGB(0x80, 0x80, 0xff), RGB(0xff, 0xff, 0x80),
		RGB(0xff, 0x80, 0xff), RGB(0x80, 0xff, 0xff),
		RGB(0xff, 0x40, 0x40), RGB(0x40, 0xff, 0x40), RGB(0x40, 0x40, 0xff), RGB(0xff, 0xff, 0x40),
		RGB(0xff, 0x40, 0xff), RGB(0x40, 0xff, 0xff),
		RGB(0x80, 0x40, 0x40), RGB(0x40, 0x80, 0x40), RGB(0x40, 0x40, 0x80), RGB(0x80, 0x80, 0x40)
	};
	HBRUSH hbr = SYS(CreateSolidBrush(colors[i & 0x0f]));
	// get index of first visible point
	int ix = (int)m_offsetIndices->getAt(i);
	Iterator* it = pSeries->begin(ix);
	RECT rect;
	int sx = PixelPerUnitX * this->m_axisX.iScale;
	int sy = PixelPerUnitY * this->m_axisY.iScale;
	SYS(SelectObject(m_hDC, GetStockObject(DC_PEN)));
	SYS(SetDCPenColor(m_hDC, colors[i & 0x0f]));
	int px, py;
	int* d = (ix > 0) ? (int*)pSeries->getAt(ix - 1) : (int*)pSeries->getAt(0);
	px = d[0]; py = d[1];

	POINT p;
	SYS(MoveToEx(m_hDC, px, py, &p));
	while (pSeries->next(it)) {
		int* data = (int*)it->pItem;
		size_t x = sx * (data[0] - m_axisX.iOffsetInUnits) + m_axisX.iOffsetInPixels;
		size_t y = sy * (data[1] - m_axisY.iOffsetInUnits) + m_axisY.iOffsetInPixels;
		if (x < m_axisX.iOffsetInPixels + lpRect->right) {
		SYS(LineTo(m_hDC, x, y));
		//SYS(LineTo(m_hDC, rect.right, cy));
			SYS(SetRect(&rect, x-1, y-1, x+2, y+2));
			SYS(FillRect(m_hDC, &rect, hbr));
		} else {
			break;
		}
	}
		//if (true/*channel was selected*/ /*&& pChn != m_pSelectedChannel*/) {
		//	SelectObject(m_hDC, hDefaultPen);
		//	SetDCPenColor(m_hDC, pChn->color);
		//	m_info.pfDraw(m_hDC, this, pChn);
		//}
	pSeries->end(it);
}


void ChartCtrl::setSize(size_t iWidth, size_t iHeight) {
	m_rect.right = iWidth; m_rect.bottom = iHeight - m_iToolBoxHeight;
	scale(m_axisX.iScale, m_axisY.iScale);
	SYS(MoveWindow(m_hWnd, m_rect.left, m_rect.top + m_iToolBoxHeight, m_rect.right, m_rect.bottom, false));
}

void ChartCtrl::setData(IndexedCollection *data, size_t iLength, size_t iRange) {
	m_data = data;
	if (m_data != NULL) {
		Iterator *it = m_data->begin(0);
		while (m_data->next(it)) {
			COLLECTIONITEM item = m_offsetIndices->add();
			*item.ref = 0;
		}
		m_data->end(it);
	} else {
		m_offsetIndices->empty();
	}
	m_iLength = iLength; m_iRange = iRange;
	scale(1, 1);
	setScroll();
	SYS(InvalidateRect(m_hWnd, NULL, false));
}

int ChartCtrl::seekData(void *pParent, void* pMember, int ix, void* pArgs) {
	return (((int*)pMember)[0] < (int)pArgs);
}

void ChartCtrl::scale(size_t iScaleX, size_t iScaleY) {
	m_axisX.iScale = iScaleX;
	m_axisY.iScale = iScaleY;
	m_size.right = m_iLength * ChartCtrl::PixelPerUnitX * iScaleX;
	m_size.bottom = m_iRange * ChartCtrl::PixelPerUnitY * iScaleY;
	// set scrolling
	//setScroll();
}

void ChartCtrl::scroll(UINT fnBar, WPARAM wParam) {
	SCROLLINFO& si = fnBar == SB_VERT ? m_axisY.scrollInfo : m_axisX.scrollInfo;
	if (LOWORD(wParam) != SB_THUMBPOSITION && LOWORD(wParam) != SB_ENDSCROLL) {
		SYS(GetScrollInfo(m_hWnd, fnBar, &si));
		int scrollPos = si.nPos;
		switch (LOWORD(wParam)) {
			//case SB_LINELEFT:
			//case SB_LINEUP:
			case 0:
				si.nPos--; break;
			//case SB_LINERIGHT:
			//case SB_LINEDOWN:
			case 1:
				si.nPos++; break;
			//case SB_PAGELEFT:
			//case SB_PAGEUP:
			case 2:
				si.nPos -= si.nPage; break;
			//case SB_PAGERIGHT:
			//case SB_PAGEDOWN:
			case 3:
				si.nPos += si.nPage; break;
			case SB_TOP:
				si.nPos = si.nMax; break;
			case SB_BOTTOM:
				si.nPos = si.nMin; break;
			case SB_THUMBTRACK:
				si.nPos = si.nTrackPos; break;
		}
		SYS(SetScrollInfo(m_hWnd, fnBar, &si, false));
		SYS(GetScrollInfo(m_hWnd, fnBar, &si));
		if (si.nPos != scrollPos) {
			scrollPos += si.nPos; 
			int xPos = 0, yPos = 0;
			if (fnBar == SB_HORZ) {
				xPos = scrollPos;
				si.nMax = m_size.right;
			} else {
				yPos = scrollPos;
				si.nMax = m_size.bottom;
			}
			setScroll();
			SYS(ScrollWindow(m_hWnd, xPos, yPos, NULL, NULL));
		}
	}
}

void ChartCtrl::setScroll() {
	//SYS(EnableScrollBar(m_hWnd, SB_BOTH, ESB_ENABLE_BOTH));
	SYS(GetScrollInfo(m_hWnd, SB_HORZ, &m_axisX.scrollInfo));
	m_axisX.scrollInfo.nMax = m_size.right - 1;
	m_axisX.scrollInfo.nPage = m_rect.right;
	SYS(SetScrollInfo(m_hWnd, SB_HORZ, &m_axisX.scrollInfo, false));
	size_t iStepSize = ChartCtrl::PixelPerUnitX * m_axisX.iScale;
	div_t d = div(m_axisX.scrollInfo.nPos, iStepSize);
	m_axisX.iOffsetInPixels = iStepSize - d.rem; m_axisX.iOffsetInUnits = d.quot;
	// get index of first visible element of each data series
	if (m_data != NULL) {
		Iterator *it = m_data->begin(0);
		int i = 0, v;
		while (m_data->next(it)) {
			((IndexedCollection*)it->pItem)->apply(ChartCtrl::seekData, (void*)d.quot, (int&)v);
			m_offsetIndices->setAt(i++, (void*)v, sizeof(int));
		}
		m_data->end(it);
	}

	SYS(GetScrollInfo(m_hWnd, SB_VERT, &m_axisY.scrollInfo));
	m_axisY.scrollInfo.nMax = m_size.bottom - 1;
	m_axisY.scrollInfo.nPage = m_rect.bottom;
	SYS(SetScrollInfo(m_hWnd, SB_VERT, &m_axisY.scrollInfo, false));
	iStepSize = ChartCtrl::PixelPerUnitY * m_axisY.iScale;
	d = div(m_axisY.scrollInfo.nPos, iStepSize);
	m_axisY.iOffsetInPixels = iStepSize - d.rem; m_axisY.iOffsetInUnits = d.quot;
}