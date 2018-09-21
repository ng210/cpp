#include "synapp.h"
#include "resource.h"

BOOL SynApp::processEditCommands(UINT32 cmd, WPARAM wParam, LPARAM lParam) {
	BOOL res = true;
	switch (cmd) {
		//case ID_EDIT_UNDO:
		//	openFile();
		//	break;
		//case ID_EDIT_REDO:
		//	saveFile();
		//	break;
		//case ID_EDIT_CUT:
		//	saveFileAs();
		//	break;
		//case ID_EDIT_COPY:
		//	importFile();
		//	break;
		//case ID_EDIT_PASTE:
		//	exportFile();
		//	break;
	default:
		res = false;
		break;
	}
	return res;
}
