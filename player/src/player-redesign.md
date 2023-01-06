Adapter
- getInfo()
- prepareContext()
- createDevice()

Sequence : Stream
- fromFrames*
- toFrames*
- saveToStream*

Device(Object, Adapter)
- initialize()
- processCommand()
- makeCommand()*

Channel(Sequence, Device)
- run()

PlayerAdapter : Adapter
- prepareContext() { read fps? }

Player
- channels
- devices
- sequences
- dataBlocks

PlayerDevice(Player, PlayerAdapter) : Device
- adapters
- addAdapter()
- initialize() { create channels/devices/sequences/dataBlocks in Player }
- processCommand()
- load() => initialize()
- save()

Load
1. PlayerDevice.Create(data)
 - new PlayerDevice
   - addAdapter(PlayerAdapter)
   - create master channel
   - new Player
   - Player.initialize(data)
     - 

