# Todos

## First of all: cleanup code
 * review class model: merge, remove, rename

## 1. Design for a self-containing file format: ASU Adapter-Sequence-User data
 * identify adapters required for the playback
>   00 db count of adapters<br/>
>   02 db ID of the 1st adapter<br/>
>   2n db ID of the nth adapter<br/>
 * store sequences
>   00 dw length of 1st sequence (master)<br/>
>   02 dw length of 2nd sequence<br/>
>   2n dw length of n+1 sequence<br/>
   This list has to be read sequentially to get the start address of each sequence.
 * store user data
>   00 dd length of 1st user data block<br/>
>   04 dd length of 2nd user data block<br/>
>   4n dd length of n+1 user data block<br/>
   This list has to be read sequentially to get the start address of each data block.

## 2. Player changes to support the ASU format
 * optional adapter checks
   A common list of adapters is required, that currently contains:
>   00: player adapter<br/>
>   01: synth adapter<br/>
 * possible adjustments of the sequence management
 * manage user-data blocks
   an array of arrays
 * player command to create targets
  * targets consist of an object and its adapter
  * targets should be created by the player, not the context
  * adapters should contain a factory method to create objects
  * the user data blocks can provide the data for the creation/initialization<br/>
#### Example
 * the synth adapter creates the synth objects for each channel
 * the synth objects have data banks for their instrument settings
 * these data banks have to be stored in the user data blocks
 * player reads the create target command with parameters ID of the synth adapter and ID of the user data block containing synth data bank with the instrument definitions
 * the synth adapter creates a synth object and adds the data bank from the user data block

## 3. Create "stream" player
 * currently, the player/channel processes sequences with complex structures
  * a sequence is an array of frames
  * a frame consists of a time delta and an array of commands
  * a command consists of a code and a variable length list of parameters
 * add player to process byte stream sequences
  * solution a,
   * a sequence consists of an array of time delta and a command<br/>
#### Example
>    0000: time delta #1<br/>
>    01: command code #1<br/>
>    02,03,04: parameters #1<br/>
>    <br/>
>    0002: time delta #2<br/>
>    02: command code #2<br/>
>    03,02: parameters #2<br/>
>    ...<br/>
>    0007: time delta #n<br/>
>    09: command code #n<br/>
>    01: parameters #n<br/>

   * synchron commands are seperated with a time delta of 0
   * pro: very simple structure, very simple logic to process
   * contra: redundant time delta at synchron commands
  * solution b,
   * a sequence consists of an array of time delta, length and array of commands<br/>
#### Example
>  0000: delta time #1<br/>
>  01: length of command array #1<br/>
>  01: command code #1<br/>
>  02,03,04: parameters #1<br/>
><br/>
>  0002: delta time #2<br/>
>  02: length of command array #2<br/>
>  02: command code #2<br/>
>  01,05: parameters #2<br/>
>  03: command code #3<br/>
>  04: parameters #3<br/>
>  ...<br/>
>  0004: delta time #n<br/>
>  01: length of command array #n<br/>
>  01: command code #n1<br/>
>  02,03,04: parameters #n<br/>
   * pro: clear structure, simple loop to process the commands
    * contra: single command frames contain extra byte for the command array length

## 4. Create new adapters/objects
 * 2D objects
  * sprites
  * particles
 * 3D objects
  * objects
  * particles
  * heightmap
