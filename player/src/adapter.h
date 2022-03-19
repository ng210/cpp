#ifndef __PLAYER_ADAPTER_H
#define __PLAYER_ADAPTER_H

#include "collection/parray.h"

NS_FW_BASE_USE
namespace PLAYER {

    class Adapter;
    class Player;

    typedef Adapter* (AdapterCreator)(Player* player);
    typedef void (AdapterInit)(void* args);

    typedef struct AdapterInfo {
        const char name[16];
        int id;
        AdapterCreator* creator;
        AdapterInit* initializer;

    } AdapterInfo;

    class Player;
    class Device;
    class Channel;
    class Sequence;

    class Adapter {
    protected: PROP(Player*, player);
    protected: PROP_R(PArray, devices);
    public:
        Adapter();
        Adapter(Player* player);
        virtual ~Adapter();
        
        virtual void prepareContext(byte* data);
        Device* createDevice(int deviceType, byte** initData);
        Device* getDevice(int id) { return (Device*)devices_.getAt(id); }
        virtual void setRefreshRate(float fps);
        virtual AdapterInfo* info() = 0;
        virtual Device* createDeviceImpl(int deviceType, byte** initData) = 0;
        virtual byte* processCommand(Channel* channel, byte command) = 0;

        virtual byte* makeCommand(byte command, Sequence* sequence, byte* cursor) = 0;
    };
}

#endif