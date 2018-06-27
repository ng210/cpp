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
## Solution: subsequence
* split sequences into subsequences by command type, command parameter
* Sequence = Collection{ Subsequence }
* Subsequence = Collection{ frame, parameter(s) }
* Sequence might store length (absolute position of the END command) instead of adding a subsequence with a single entry

### Examples
<pre>
Sequence1: {
 Subsequence11: { [setTempo]: { 0000, 120.0 },
 Subsequence12: { [createChannel]: { 0000, 01, 01 }, {0040, 02, 02 },
 Subsequence13: { [end]: { 0040 } }
}
Sequence2: {
 Subsequence21: { [prgChange]: { 0000, 01 },
 Subsequence22: { [noteOn]: { 0000, [C5], 0.75 },
 Subsequence23: { [setControl, FltCut]: { 0004, 40 }, { 0002, 80 }, { 0002, C0 }, { 0002, FF },
 Subsequence24: { [noteOff]: { 0004, [C5] },
 Subsequence25: { [end]:{ 0004 } }
}
</pre>
