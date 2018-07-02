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
Subsequence1: { frames: { [setTempo]: { 0000, 120.0 } },
Subsequence2: { frames: { [createChannel]: { 0000, 01, 01 }, {0040, 02, 02 },

Subsequence4: { [prgChange]: { 0000, 01 },
Subsequence5: { [noteOn]: { 0000, [C5], 0.75 },
Subsequence6: { [setControl, FltCut]: { 0004, 40 }, { 0002, 80 }, { 0002, C0 }, { 0002, FF },
Subsequence7: { [noteOff]: { 0004, [C5] },
Subsequence8: { [end]:{ 0004 } }
</pre>
