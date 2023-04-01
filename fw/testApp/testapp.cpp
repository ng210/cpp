#include "testapp.h"
#include "resource.h"
#include "win/editctrl.h"

NS_FW_BASE_USE
NS_FW_WIN_USE

char* TestApp::eos_ = "\n\0";

TestApp::TestApp() {
	state_ = 0;
}

TestApp::~TestApp() {
	// put cleanup code here
	// log buffer?
}

void TestApp::create(WndClass wndClass, LONG style, DWORD exStyle) {
	Window::create(wndClass, NULL, "TestApp", style, exStyle);
}

LRESULT TestApp::onCreate() {
#pragma region EventHandlers
	onLeftUp(TestApp::onLeftUpProc);
#pragma endregion

	RECT rect;
	LRESULT result = 0;
	SYSFN(result, SetWindowPos(hWnd_, NULL, 0, 0, 800, 600, SWP_NOMOVE));
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
		testApp->log("Select: %d='%s'", ix, item);
	};

	// create simple combobox
	cbSimple_.create(this, "cb_simple", CBS_SIMPLE | CBS_SORT | WS_VSCROLL);
	cbSimple_.addItems(&items);
	cbSimple_.addItem("Füge");
	cbSimple_.select(0);
	rect.right = 96; rect.bottom = 96;
	SYSFN(result, SetWindowPos(cbSimple_.hWnd(), NULL, rect.left, rect.top, rect.right, rect.bottom, SWP_SHOWWINDOW));
	rect.left += rect.right + 4;
	if (laneHeight < rect.bottom) laneHeight = rect.bottom;
	cbSimple_.onSelectItem(onSelect);
	cbSimple_.onAddItem([](ComboboxCtrl* cb, void* item) {
		cb->addItem(item);
		var testApp = (TestApp*)cb->parent();
		testApp->log("Add: '%s'", item);
	});

	// create drop down
	cbDropDown_.create(this, "cb_dropdown", CBS_DROPDOWN | CBS_SORT | WS_VSCROLL);
	cbDropDown_.addItems(&items);
	cbDropDown_.addItem("Füge");
	cbDropDown_.select(0);
	rect.right = 96; rect.bottom = 96;
	SYSFN(result, SetWindowPos(cbDropDown_.hWnd(), NULL, rect.left, rect.top, rect.right, rect.bottom, SWP_SHOWWINDOW));
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
	SYSFN(result, SetWindowPos(staticText_.hWnd(), NULL, rect.left, rect.top, rect.right, rect.bottom, SWP_SHOWWINDOW));
	//SYSPR(SetWindowText(staticText_.hWnd(), "Hello"));
	rect.left += rect.right + 4;
	
	staticIcon_.create(this, "StaticIcon", SS_ICON);
	rect.right = 24;
	var SYSFN(icon, LoadIcon(NULL, (LPSTR)IDI_WARNING));
	SendMessage(staticIcon_.hWnd(), STM_SETICON, (WPARAM)icon, NULL);
	SYSFN(result, SetWindowPos(staticIcon_.hWnd(), NULL, rect.left, rect.top, rect.right, rect.bottom, SWP_SHOWWINDOW));
	rect.left += rect.right + 4;

	staticBitmap_.create(this, "StaticBitmap", SS_BITMAP | SS_SUNKEN);
	rect.right = 32;
	SendMessage(staticBitmap_.hWnd(), STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmap);
	SYSFN(result, SetWindowPos(staticBitmap_.hWnd(), NULL, rect.left, rect.top, rect.right, rect.bottom, SWP_SHOWWINDOW));
	rect.left += rect.right + 4;
	if (laneHeight < rect.bottom) laneHeight = rect.bottom;
#pragma endregion

	SYSFN(hBitmap, LoadImage(hInstance_, MAKEINTRESOURCE(IDB_PLUS), IMAGE_BITMAP, 0, 0, LR_LOADTRANSPARENT));
#pragma region Button
	rect.right = 48;
	rect.bottom = 24;
	textButton_.create(this, "Button", BS_CENTER | BS_TEXT);
	SYSPR(SetWindowText(textButton_.hWnd(), "Ok"));
	SYSFN(result, SetWindowPos(textButton_.hWnd(), NULL, rect.left, rect.top, rect.right, rect.bottom, SWP_SHOWWINDOW));
	rect.left += rect.right + 4;

	//var bitmap = LoadImage(hInstance_, MAKEINTRESOURCE(IDB_PLUS), IMAGE_BITMAP, 0, 0, LR_LOADTRANSPARENT);
	imageButton_.create(this, "Button", BS_BITMAP);
	SYSPR(SendMessage(imageButton_.hWnd(), BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmap));
	SYSFN(result, SetWindowPos(imageButton_.hWnd(), NULL, rect.left, rect.top, rect.right, rect.bottom, SWP_SHOWWINDOW));
	rect.left += rect.right + 4;

	var leftButtonProc = [](Window* wnd, POINT& pos, WPARAM state) {
		var testApp = (TestApp*)wnd->parent();
		testApp->log("Left Button");
		return testApp->onLeftUp()(testApp, pos, state);
	};
	var rightButtonProc = [](Window* wnd, POINT& pos, WPARAM state) {
		var testApp = (TestApp*)wnd->parent();
		testApp->log("Right Button");
		return testApp->onLeftUp()(testApp, pos, state);
	};
	textButton_.onLeftUp(leftButtonProc);
	textButton_.onRightUp(rightButtonProc);
	imageButton_.onLeftUp(leftButtonProc);
	imageButton_.onRightUp(rightButtonProc);
#pragma endregion

#pragma region Edit
	SYSPR(GetClientRect(hWnd_, &rect));
	rect.left += 4; rect.right -= 8;
	rect.top += laneHeight + 4; rect.bottom -= 12;
	logCtrl_.create(this, "", WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL);
	testCtrl_.create(this, "");
	var height = rect.bottom >> 1;
	SYSFN(result, SetWindowPos(logCtrl_.hWnd(), NULL, rect.left, rect.top, rect.right, height, SWP_SHOWWINDOW));
	rect.top += height + 2;
	SYSPR(SetWindowPos(testCtrl_.hWnd(), NULL, rect.left, rect.top, rect.right, height, SWP_SHOWWINDOW));
	SendMessage(logCtrl_.hWnd(), WM_SETFONT, (WPARAM)hFont, false);
#pragma endregion

	//char eos = '\0';
	//log_.append(&eos, 1);
	return result;
}

LRESULT TestApp::onDestroy() {
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
			mmi->ptMaxSize.x = 800; mmi->ptMaxSize.y = 600;
			//mmi->ptMaxPosition.x = 20; mmi->ptMaxPosition.y = 20;
			mmi->ptMaxTrackSize.x = 800; mmi->ptMaxTrackSize.y = 600;
			mmi->ptMinTrackSize.x = 640; mmi->ptMinTrackSize.y = 400;
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
	char text[1024];
	wvsprintf(text, format, args);
	// log text
	log_.write(text, NS_FW_BASE::strlen(text), 0, log_.length()-1);
	char* eol = "\r\n---\r\n\0";
	log_.append(eol, 8);
	char* buffer = log_.getBufferAsType<char>();
	SetWindowText(logCtrl_.hWnd(), buffer);
	testCtrl_.setText(buffer);
	FREE(buffer);
	//ScrollWindow(logControl_->hWnd(), 0, 0, NULL, &rect);
}

void TestApp::update() {
	switch (state_) {
		case 0:
			log("State 0: Click to advance state!");
			state_++;
			break;
		case 2:
			log("State 1: Hello world!");
			state_++;
			break;
		case 4:
			log("State 2: TestApp works!");
			state_++;
			break;
		case 6:
			log("State 3: Final state reached, restarting!");
			state_ = -1;
			break;
		default:
			break;
	}
	Sleep(1);
}


WinApp* createApplication(HINSTANCE hInstance, Map* args) {
	// Load menu
	HMENU hMenu = NULL;
	var wndClass = Window::registerClass((char*)"TestApp", hInstance);
	var app = NEW_(TestApp);
	app->create(wndClass);
	return app;
}
