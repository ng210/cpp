#ifndef __PLAYER_ADAPTER_H
#define __PLAYER_ADAPTER_H

#include "collection/parray.h"
#include <stdarg.h>
#include "base/stream.h"

NS_FW_BASE_USE
namespace PLAYER {

    typedef struct AdapterInfo {
        const char name[16];
        int id;
    } AdapterInfo;

    class Player;
    class Device;

    class Adapter {
    public:
        Adapter();
        //virtual ~Adapter();

        virtual AdapterInfo* const getInfo() = 0;
        virtual void initialize(byte** pData);
        virtual void prepare();
        virtual void cleanUp();
        virtual Device* createDevice(int deviceType, Player* player) = 0;
#ifdef PLAYER_EDIT_MODE
        virtual void writeToStream(Stream* data);
#endif

        static COLLECTION_COMPARE compare;
    };
}

#endif