#include "synth/ui/ui-manager.h"
#include "synth/ui/synth-app.h"

NS_FW_BASE_USE
using namespace SYNTH;
using namespace SYNTH_APP;

UiManager::UiManager(SynthApp* app) : Component(app) {
	controls_.init(16);
}

UiManager::~UiManager() {
	// remove every control
	controls_.apply(
		[](COLLECTION_ARGUMENTS) {
			DEL_((Ctrl*)value);
			return value;
		});
}

//int UiManager::updateLayout() {
//
//}
