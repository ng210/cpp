#include "console/consoleapp.h"
#include "player/test/player-test.h"

NS_FW_BASE_USE
using namespace PLAYER;

int _main(NS_FW_BASE::Map* args) {
#ifdef _DEBUG
    Debug::initialize(/*DEBUG_UNICODE | DEBUG_MEMORY*/);
#endif

    Test::cons = getConsole();
    ConsAdapter::getConsole = &getConsole;

    PlayerTest::LAST_TICK = 120;

    var tests = NEW_(PlayerTest);
    LOG("Player tests\n");
    int totalPassed, totalFailed;
    tests->runAll(totalPassed, totalFailed);
    DEL_(tests);

    return 0;
}