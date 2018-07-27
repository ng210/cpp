#include "synapp.h"

static const char* fileFilters =
"All files\0*.*\0\
XM modules\0*.XM\0\
";


LRESULT SynApp::openFile() {
	return 0;
}
LRESULT SynApp::saveFile() {
	return 0;
}
LRESULT SynApp::saveFileAs() {
	return 0;
}

LRESULT SynApp::importFile() {
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd_;
	//ofn.hInstance = hInstance_;
	ofn.lpstrFilter = fileFilters;
	ofn.lpstrCustomFilter = NULL;
	char path[512] = "";
	ofn.lpstrFile = path;
	ofn.nMaxFile = sizeof(path);
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
	BOOL SYSFN(res, GetOpenFileName(&ofn));
	if (res) {
		int start = 0;
		if (asu_->importXm(ofn.lpstrFile, start) == 0) {
			MessageBox(this->hWnd_, "Error during importing sequences from xm!", "Error!", MB_OK);
		}
	}
	return res;
}

LRESULT SynApp::exportFile() {
	return 0;
}