#include <math.h>
#include "testApp/testapp.h"
#include "resource.h"
#include "win/editctrl.h"
#include "win/chartctrl.h"
#include "testApp/test-task.h"

NS_FW_BASE_USE
NS_FW_WIN_USE

#define MAX_WIDTH 1280
#define MIN_WIDTH 640
#define MAX_HEIGHT 1080
#define MIN_HEIGHT 400

#define LOGBGCOLOR 0x00402010
#define LOGTEXTCOLOR 0x0080C0E0

char* TestApp::eos_ = "\n\0";

char* windowClassName = "TestApp";

TestApp::TestApp() {
	state_ = 0;
}

TestApp::~TestApp() {
	// put cleanup code here
	// log buffer?
	log_.clear();
	SYSPR(DeleteObject(logBgBrush_));
}

char* const TestApp::registerWindowClass() {
	Window::registerClass(windowClassName, hInstance_);
	return windowClassName;
}

void TestApp::create() {
	Window::create(NULL, windowClassName);
}

LRESULT TestApp::onCreated() {
	#pragma region EventHandlers
	onLeftUp(TestApp::onLeftUpProc);
	#pragma endregion

	RECT rect;
	LRESULT result = 0;
	setSize(1280, 768);
	//SYSFN(result, SetWindowPos(hWnd_, NULL, 0, 0, 1280, 768, SWP_NOMOVE));
	HFONT SYSFN(hFont, (HFONT)GetStockObject(SYSTEM_FIXED_FONT));

	// create controls in the upper lane
	var laneHeight = 0;
	rect.left = 4;  rect.top = 4;

	var SYSFN(hBitmap, LoadImage(NULL, "plus.bmp", IMAGE_BITMAP, 0, 0, LR_LOADTRANSPARENT | LR_LOADFROMFILE));

	#pragma region Combobox
	PArray items;
	items.push("Alma1");
	items.push("Alma2");
	items.push("Barack1");
	items.push("Barack2");
	items.push("Citrom");
	items.push("Dinnye1");
	items.push("Dinnye2");
	items.push("Eper");

	var onSelect = [](ComboboxCtrl* cb, int ix, void* item) {
		var testApp = (TestApp*)cb->parent();
		testApp->log("Select: %d='%s'\r\n", ix, item);
	};

	// create simple combobox
	cbSimple_.create(this, "cb_simple", CBS_SIMPLE | CBS_SORT | WS_VSCROLL);
	cbSimple_.addItems(&items);
	cbSimple_.addItem("Füge");
	cbSimple_.select(0);
	rect.right = 96; rect.bottom = 96;
	cbSimple_.set(rect);
	//SYSFN(result, SetWindowPos(cbSimple_.hWnd(), NULL, rect.left, rect.top, rect.right, rect.bottom, SWP_SHOWWINDOW));
	rect.left += rect.right + 4;
	if (laneHeight < rect.bottom) laneHeight = rect.bottom;
	cbSimple_.onSelectItem(onSelect);
	cbSimple_.onAddItem([](ComboboxCtrl* cb, void* item) {
		cb->addItem(item);
		var testApp = (TestApp*)cb->parent();
		testApp->log("Add: '%s'\r\n", item);
	});

	// create drop down
	cbDropDown_.create(this, "cb_dropdown", CBS_DROPDOWN | CBS_SORT | WS_VSCROLL);
	cbDropDown_.addItems(&items);
	cbDropDown_.addItem("Füge");
	cbDropDown_.select(0);
	rect.right = 96; rect.bottom = 96;
	cbDropDown_.set(rect);
	//SYSFN(result, SetWindowPos(cbDropDown_.hWnd(), NULL, rect.left, rect.top, rect.right, rect.bottom, SWP_SHOWWINDOW));
	rect.left += rect.right + 4;
	if (laneHeight < rect.bottom) laneHeight = rect.bottom;
	cbDropDown_.onSelectItem(onSelect);

	//cbDropDown_.create(this, "cb_dropdown", CBS_SIMPLE);
	//cbDropDown_.addItems(&items);
	//cbDropDown_.addItem("Füge");
	//rect.right = 64; rect.bottom = 24;
	//SYSFN(result, SetWindowPos(cbSimple_.hWnd(), NULL, rect.left, rect.top, rect.right, rect.bottom, SWP_SHOWWINDOW));
	//rect.left += rect.right + 4;
	//if (laneHeight < rect.bottom) laneHeight = rect.bottom;
	#pragma endregion

	#pragma region Static
	staticText_.create(this, "StaticText", SS_CENTER);
	rect.right = 96; rect.bottom = 24;
	SendMessage(staticText_.hWnd(), WM_SETTEXT, NULL, (LPARAM)"Hello");
	staticText_.set(rect);
	//SYSFN(result, SetWindowPos(staticText_.hWnd(), NULL, rect.left, rect.top, rect.right, rect.bottom, SWP_SHOWWINDOW));
	//SYSPR(SetWindowText(staticText_.hWnd(), "Hello"));
	rect.left += rect.right + 4;
	
	staticIcon_.create(this, "StaticIcon", SS_ICON);
	rect.right = 24;
	var SYSFN(icon, LoadIcon(NULL, (LPSTR)IDI_WARNING));
	SendMessage(staticIcon_.hWnd(), STM_SETICON, (WPARAM)icon, NULL);
	staticIcon_.set(rect);
	//SYSFN(result, SetWindowPos(staticIcon_.hWnd(), NULL, rect.left, rect.top, rect.right, rect.bottom, SWP_SHOWWINDOW));
	rect.left += rect.right + 4;

	staticBitmap_.create(this, "StaticBitmap", SS_BITMAP | SS_SUNKEN);
	rect.right = 32;
	SendMessage(staticBitmap_.hWnd(), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmap);
	staticBitmap_.set(rect);
	//SYSFN(result, SetWindowPos(staticBitmap_.hWnd(), NULL, rect.left, rect.top, rect.right, rect.bottom, SWP_SHOWWINDOW));
	rect.left += rect.right + 4;
	if (laneHeight < rect.bottom) laneHeight = rect.bottom;
	#pragma endregion

	SYSFN(hBitmap, LoadImage(hInstance_, MAKEINTRESOURCE(IDB_PLUS), IMAGE_BITMAP, 0, 0, LR_LOADTRANSPARENT));
	#pragma region Button
	rect.right = 48;
	rect.bottom = 24;
	textButton_.create(this, "Button", BS_CENTER | BS_TEXT);
	SYSPR(SetWindowText(textButton_.hWnd(), "Ok"));
	textButton_.set(rect);
	//SYSFN(result, SetWindowPos(textButton_.hWnd(), NULL, rect.left, rect.top, rect.right, rect.bottom, SWP_SHOWWINDOW));
	rect.left += rect.right + 4;

	//var bitmap = LoadImage(hInstance_, MAKEINTRESOURCE(IDB_PLUS), IMAGE_BITMAP, 0, 0, LR_LOADTRANSPARENT);
	imageButton_.create(this, "Button", BS_BITMAP);
	SYSPR(SendMessage(imageButton_.hWnd(), BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmap));
	imageButton_.set(rect);
	//SYSFN(result, SetWindowPos(imageButton_.hWnd(), NULL, rect.left, rect.top, rect.right, rect.bottom, SWP_SHOWWINDOW));
	rect.left += rect.right + 4;

	var leftButtonProc = [](Window* wnd, POINT& pos, WPARAM state) {
		var testApp = (TestApp*)wnd->parent();
		testApp->log("Left Button\r\n");
		return testApp->onLeftUp()(testApp, pos, state);
	};
	var rightButtonProc = [](Window* wnd, POINT& pos, WPARAM state) {
		var testApp = (TestApp*)wnd->parent();
		testApp->log("Right Button\r\n");
		return testApp->onLeftUp()(testApp, pos, state);
	};
	textButton_.onLeftUp(leftButtonProc);
	textButton_.onRightUp(rightButtonProc);
	imageButton_.onLeftUp(leftButtonProc);
	imageButton_.onRightUp(rightButtonProc);
	#pragma endregion

	#pragma region Log
	SYSPR(GetClientRect(hWnd_, &rect));
	rect.left += 4; rect.right -= 8;
	rect.top += laneHeight + 4; rect.bottom -= 12;
	logCtrl_.create(this, "");
	var height = rect.bottom >> 2;
	logCtrl_.set(rect.left, rect.top, rect.right, height);
	SYSFN(logBgBrush_, CreateSolidBrush(LOGBGCOLOR));
	//SYSFN(result, SetWindowPos(logCtrl_.hWnd(), NULL, rect.left, rect.top, rect.right, height, SWP_SHOWWINDOW));
	rect.top += height + 2;
	SYSPR(SendMessage(logCtrl_.hWnd(), WM_SETFONT, (WPARAM)hFont, false));
	#pragma endregion

	#pragma region Chart
	var unitX = 20, unitY = 12;
	ChartSettings settings = {
		{ unitX, unitY },		// unit1
		{  5,  5 },				// unit2
		true,					// showGrid
		0x005c3329,				// backgroundColor
		0x00a4cdd7,				// foregroundColor
		0x00b86652,				// grid1Color
		//0x00dc7a62,			// grid2Color
		0x004799ad,				// lineColor
		0x0080a020,				// textColor
		ChartInsertModeFree,	// insertMode
		ChartDragModeFree,		// dragMode
		ChartDrawModeLine,		// drawMode
		unitX >> 1,				// lineSizeX
		unitY >> 1,				// lineSizeY
		2*5*unitX,				// pageSizeX
		2*5*unitY,				// pageSizeY
		10,						// dragSpeed
		0.1f					// scaleSpeed
	};
	chartCtrl_.initialize(&settings);
	chartCtrl_.create(this, "");
	var cols = settings.unit2.x * 18;
	var rows = settings.unit2.y * 8;
	var list = NEW_(Array, sizeof(POINT), cols);
	POINT p;
	for (var pi = 0; pi < cols; pi++) {
		p.x = pi;
		p.y = (int)(0.5 * rows * (1.0 + 0.8 * sin(2.0 * pi * 3.14159265359 / cols)));
		list->push(&p);
	}
	chartCtrl_.dataBind(list, [](int x, int y1, int y2, POINT* p, Collection* dataSource) {
		var ret = false;
		var src = (Array*)dataSource;
		Key ix = -1;
		var point = (POINT*)src->binSearch(x, ix, Collection::compareInt);
		if (point != NULL && point->y >= y1 && point->y <= y2) {
			p->x = point->x;
			p->y = point->y;
			ret = true;
		}
		return ret;
	});
	unitX *= settings.unit2.x;
	unitY *= settings.unit2.y;
	var cw = rect.right;	// (rect_.right / unitX)* unitX;
	var ch = (2 * height / unitY)* unitY;
	chartCtrl_.set(rect.left, rect.top, cw, ch);
	//SYSFN(result, SetWindowPos(chartCtrl_.hWnd(), NULL, rect.left, rect.top,  cw, ch, SWP_SHOWWINDOW));
	chartCtrl_.setVirtualSize(cols * settings.unit1.x, rows * settings.unit1.y);
	rect.top += ch + 2;
	#pragma endregion

	testCtrl_.create(this, "");
	testCtrl_.set(rect.left, rect.top, rect.right, height);

	task1_ = NEW_(TestTask, this);
	task1_->run();
	//SYSPR(SetWindowPos(testCtrl_.hWnd(), NULL, rect.left, rect.top, rect.right, height, SWP_SHOWWINDOW));

	//char eos = '\0';
	//log_.append(&eos, 1);
	return result;
}

LRESULT TestApp::onClose() {
	if (task1_) DEL_(task1_);
	DEL_(chartCtrl_.dataSource());
	return 0;
}

LRESULT CALLBACK TestApp::wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	LRESULT ret = 0;
	switch (uMsg) {
		//case WM_SIZING:
		//	sizing((int)wParam, (LPRECT)lParam, NULL, 640, 400);
		//	ret = TRUE;
		//	break;
		case WM_GETMINMAXINFO:
			MINMAXINFO* mmi;
			mmi = (MINMAXINFO*)lParam;
			mmi->ptMaxSize.x = MAX_WIDTH; mmi->ptMaxSize.y = MAX_HEIGHT;
			//mmi->ptMaxPosition.x = 20; mmi->ptMaxPosition.y = 20;
			mmi->ptMaxTrackSize.x = MAX_WIDTH; mmi->ptMaxTrackSize.y = MAX_HEIGHT;
			mmi->ptMinTrackSize.x = MIN_WIDTH; mmi->ptMinTrackSize.y = MIN_HEIGHT;
			break;
		//case WM_SIZE:
		//	break;
		//case WM_NOTIFY:
		//	NMHDR* nmhdr;
		//	nmhdr = (NMHDR*)lParam;
		//	//char* item;
		//	//item = (char*)SendMessage(nmhdr->hwndFrom, CB_GETITEMDATA, wParam, NULL);
		//	switch (nmhdr->code) {
		//	case CBN_ITEMSELECTED:
		//		log("item='%d'\n", wParam, nmhdr->code);
		//		break;
		//	case CBN_NEWITEM:
		//		log("item='%s'\n", wParam, nmhdr->code);
		//		break;
		//	}
		//	break;
		case WM_CTLCOLORSTATIC:
			if ((HWND)lParam == logCtrl_.hWnd()) {
				// set color for log
				var hDC = (HDC)wParam;
				SetTextColor(hDC, LOGTEXTCOLOR);
				SetBkColor(hDC, LOGBGCOLOR);
				return (INT_PTR)logBgBrush_;
			}
		default:
			ret = Window::wndProc(hWnd, uMsg, wParam, lParam);
	}
	return ret;
}

LRESULT TestApp::onLeftUpProc(Window* wnd, POINT& pos, WPARAM state) {
	var testApp = (TestApp*)wnd;
	if ((testApp->state_ & 0x01) != 0) {
		testApp->state_++;
	}
	return 0;
}

void TestApp::log(const char* format, ...) {
	va_list args;
	va_start(args, format);
	logCtrl_.vprintf(format, args);
	va_end(args);

	var buffer = logCtrl_.getBuffer();
	testCtrl_.setText(buffer);
	FREE(buffer);
}

void TestApp::update() {
	switch (state_) {
		case 0:
			log("State 0: Click to advance state!\r\n");
			state_++;
			break;
		case 2:
			log("State 1: Hello world!\r\n");
			state_++;
			break;
		case 4:
			log("State 2: TestApp works!\r\n");
			state_++;
			break;
		case 6:
			log("State 3: Final state reached, restarting!\r\n");
			state_ = -1;
			break;
		default:
			break;
	}
	if (task1_ && task1_->state() == TASK_STATE::TaskCompleted) {
		DEL_(task1_);
		task1_ = NULL;
	}
	Sleep(1);
}

WinApp* createApplication(HINSTANCE hInstance, Map* args) {
	// Load menu
	HMENU hMenu = NULL;
	var app = NEW_(TestApp);
	app->create();
	return app;
}
