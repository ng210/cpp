#include "synapp.h"
#include "resource.h"

static const char* fileFilters =
"All files\0*.*\0\
XM modules\0*.XM\0\
";

BOOL SynApp::processFileCommands(UINT32 cmd, WPARAM wParam, LPARAM lParam) {
	BOOL res = true;
	switch (cmd) {
		//case ID_FILE_OPEN:
		//	openFile();
		//	break;
		//case ID_FILE_SAVE:
		//	saveFile();
		//	break;
		//case ID_FILE_SAVEAS:
		//	saveFileAs();
		//	break;
		case ID_FILE_IMPORT:
			importFile();
			break;
		//case ID_FILE_EXPORT:
		//	exportFile();
		//	break;
		case ID_FILE_EXIT:
			PostQuitMessage(0);
			break;
	default:
		res = false;
		break;
	}

	return res;
}

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
		if (asu_->importFromXm(ofn.lpstrFile, start) == 0) {
			MessageBox(this->hWnd_, "Error during importing sequences from xm!", "Error!", MB_OK);
		}
	}
	return res;
}

//LRESULT SynApp::exportFile() {
//	return 0;
//}