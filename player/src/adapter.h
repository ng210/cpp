#ifndef __PLAYER_ADAPTER_H
#define __PLAYER_ADAPTER_H

#include "collection/parray.h"
#include <stdarg.h>
#include "stream.h"

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
        virtual Device* createDevice(int deviceType) = 0;
#ifdef PLAYER_EDIT_MODE
        virtual void writeToStream(Stream* data);
#endif

        static int compare(void* item, UINT32 ix, Collection* collection, void* key);
    };
}

#endif