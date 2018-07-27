# Structure of the event based playback

## Player
* playback of sequences
* create channels: associate a sequence with an object
* set tempo: frame/tick per second/minute

## Sequence
* container of events synchronized to frames

## Frame
* container of synchron commands

## Command
* represents an event
* consists of command code and parameters

## Two aspects: playback and editor
* Player: no differences
* Sequence has major differences




## Sequence for playback
* compact and fast decoder
* compact data
### Solution: simple structure
* Sequence = Collection{ delta, command }
  * delta = 0 for synchron commands
  * delta is redundant for synchron commands
* Sequence = Collection{ delta, count, command }
  * count != 1 for synchron commands
  * count is redundant for non-synchron commands

### Examples
<pre>
Sequence1: {
 { 0000, [setTempo], 120.0 }, { 0000, [createChannel], 01, 01 },
 { 0040, [createChannel], 02, 02 },
 { 0040, [end] }
}
Sequence2: {
 { 0000, [prgChange], 01 }, { 0000, [noteOn], [C5], 0.75 },
 { 0004, [setControl], [FltCut], 40 },
 { 0004, [noteOff], [C5] },
 { 0004, [end] }
}
</pre>

## Sequences for the editor 
* easy manipulation: add/remove frames, add/remove commands, modify commands
* visualization per sequence and per command
## Solution: complex structure
* metadata (name)
* array of Frame objects
## Solution: split sequences
* a subsequence contains only 1 type of command/subcommand
* sequences are split accordingly into multiple sequences
* sequences can be groupped freely
* sequences can be merged together freely

### Examples
<pre>
Sequence1: {
  frames: [
    { delta: 0000, commands: [setTempo, 120.0] }
  ]
}
Sequence2: {
  frames: [
    { delta: 0000, commands: [createChannel, 01, 01] },
    { delta: 0040, commands: [createChannel, 02, 02] }
  ]
}
Sequence3: {
  frames: [
    { delta: 0000, commands: [prgChange, 01] }
  ]
}
Sequence4: {
  frames: [
    { delta: 0000, commands: [noteOn, C4, 0.75] },
    { delta: 0004, commands: [noteOn, C5, 0.50] },
    { delta: 0004, commands: [noteOn, C4, 0.75] },
    { delta: 0004, commands: [noteOn, C5, 0.50] }
  ]
}
Sequence5: {
  frames: [
    { delta: 0000, commands: [setControl, FltCut, 40] },
    { delta: 0004, commands: [setControl, FltCut, 20] },
    { delta: 0004, commands: [setControl, FltCut, 80] },
    { delta: 0004, commands: [setControl, FltCut, 60] }
  ]
}
Sequence6: {
  frames: [
    { delta: 0000, commands: [noteOff, C4] },
    { delta: 0004, commands: [noteOff, C5] },
    { delta: 0004, commands: [noteOff, C4] },
    { delta: 0004, commands: [noteOff, C5] }
  ]
}
</pre>
