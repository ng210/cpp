#include <stdio.h>
#include <math.h>
#include <conio.h>

#include "consoleapp.h"
#include "utils/directory.h"
//#include "syn/asu.h"
#include "player/player.h"
#include "adapterlib.h"
//#include "ssn1k/ssn1klib.h"

#include "ssn1k/synthadapter.h"

#include "data.h"

#include "soundlib/soundplayer.h"
//#include "soundlib/WavFmt.h"

NS_FW_BASE_USE
NS_PLAYER_USE
NS_SSN1K_USE

#define TPS (14*4)	// ticks per second
#define TICKS 4*128

const int sampleRate = 48000;

size_t frameCounter = 0;
size_t frames = 0;
Player* player = NULL;

typedef Map& GETADAPTERS();

void print(const char* const format, ...) {
	va_list args;
	va_start(args, format);
	console.vprintf(format, args);
	va_end(args);
}

COORD* gotoxy(int x, int y) {
	return console.gotoxy(x, y);
}

/// <summary>
/// Read DLL files from plugin folder
/// Activate each plugin by calling the entry function
/// Register adapters defined by the plugins
/// </summary>
static void registerPlugins() {
	const char* dirName = "plugins\\";
	size_t dirNameLength = NS_FW_BASE::strlen(dirName);
	// Get adapter plugins
	Array* files = Directory::getFiles(dirName, ".dll");
	for (UINT32 i=0; i<files->length(); i++) {
		// winapi stuff
		char* fileName = (char*)files->getAt(i);
		char path[MAX_PATH];
		strncpy(path, dirNameLength, dirName);
		strncpy(&path[dirNameLength], MAX_PATH - dirNameLength, fileName);
		HMODULE SYSFN(lib, LoadLibrary(path));
		// implicit call to DllMain
		GETADAPTERS* SYSFN(getAdapters, (GETADAPTERS*)GetProcAddress(lib, "getAdapterInfos"));
		if (getAdapters != NULL) {
			Map& adapters = getAdapters();
			MAP_FOREACH(&adapters, AdapterLib::addAdapter((IAdapter*)value););
		}
	}
}

/// <summary>
/// Create initialization data for the SynthAdapter
/// </summary>
/// <param name="data">byte array of initialization data</param>
/// <returns>length of the created data</returns>
static int createInitData(out UINT8*& data) {
	UINT32 length = sizeof(bankConfig) + sizeof(synthConfig);
	data = MALLOC(UINT8, length);
	memcpy(data, bankConfig, sizeof(bankConfig));
	memcpy(&data[sizeof(bankConfig)], synthConfig, sizeof(synthConfig));
	return length;
}

static void createSynthAdapterInitData(Player* player) {
	size_t length;
	UINT8* data, *ptr;
	// create data for the synth adapter
	// create banks
	length = sizeof(bankConfig) + sizeof(synthConfig);
	data = MALLOC(UINT8, length + 2*sizeof UINT32);
	ptr = data;
	((size_t*)ptr)[0] = 0; ((size_t*)ptr)[1] = sizeof(bankConfig);
	ptr += 2 * sizeof size_t;
	memcpy(data, bankConfig, sizeof(bankConfig));
	ptr += sizeof bankConfig;
	memcpy(ptr, synthConfig, sizeof(synthConfig));

	// create object data
	player->addUserDataBlock(length, data);
}

/// <summary>
/// create song as an ASU document
/// </summary>
/// <param name="path">path to an XM tracker file</param>
/// <returns>new ASU document</returns>
static Player* createSong(const char* path) {
	// create Player object
	Player* player = NEW_(Player);
	// add user data blocks
	createSynthAdapterInitData(player);
	// add and initialize adapters
	IAdapter* adapter = AdapterLib::getAdapter(SynthAdapter::adapterInfo_.uid);
	player->addAdapter(adapter, 0);
	// add sequences

	//UINT8* data = NULL;
	//UINT32 length = createInitData(out data);
	//UINT32 udb = asu->addUserDataBlock(length, data);
	//asu->addAdapter(adapterInfo->id, udb);
	//int start = 0;
	//if (asu->importFromXm(path, start) == 0) {
	//	console.printf("File not found or no sequence in file!");
	//}
	return player;
}

static void soundCallback(short* buffer, int sampleCount) {
	size_t start = 0;
	size_t end = 0;
	size_t remains = sampleCount;
	//asu->soundCallback(buffer, sampleCount);
}

int _main(NS_FW_BASE::Map* args) {

#ifdef _DEBUG
	Debug::initialize(DEBUG_WINDOWS/* | DEBUG_UNICODE | DEBUG_MEMORY*/);
#endif
	console.showCursor(false);
	print("SSN1K test\n");
	//Asu::initialize();

	registerPlugins();

	SSN1K::setSampleRate(48000.0f);

	// *************************************
	// Adapter: SynthAdapter
	// Sequence: importFromXM
	// UserDataBlock: adapter init data
	// *************************************
	Player* player = createSong("C:\\code\\git\\cpp\\syn\\data\\1asu.xm");
		//char* buffer = asu->info();
		//console.printf("%s", buffer);
		//FREE(buffer);

	//player->start();

	int isRunning = 1;
	//*************************************************************************
	// Main Loop

	//while (isRunning) {
	//	// 1. read input
	//	//if (console.keys[VK_ESCAPE] == 1) {
	//	//	isRunning = 0;
	//	//}
	//	// 2. update state
	//	// 3. render output
	//	
	//	//Sleep(10);
	//}

	//*************************************************************************

	while (player->masterChannel().isActive()) {
		Sleep(10);
	}
l_end:
	DEL_(player);


	console.showCursor(true);
	print("Exit\n");

//#ifdef _DEBUG
//	MemoryMgr::checkMemDbgInfo(0, NULL);
//#endif

	return 0;
}
