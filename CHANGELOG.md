## v0.3.1 (7-3-2021)
* Native M1 included in MacOS universal binary
* Support for variable sized buffers, i.e. improved FL Studio support
* Retain plugin state when saving plugin hostproject and when changing buffer size
* Ubuntu 18.04 standalone binary published
* Auto-save programs, sounds, sequences and songs
* MIDISW screen implemented (control some of MPC2000XL's functions with MIDI CCs)
* Fix vmpc-specific setting 16 levels Erase mode -- options have become All levels (like real MPC2000XL) and Only pressed level
* Performance improvements
* Fix note repeat footer appearing during stop
* Fix a bug where many notes would not be played during note repeat and 
* Fix Erase operation while 16 levels enabled
* Add vmpc-specific setting 16 levels Erase mode -- Any pressed pad (like the real MPC2000XL) or Only source pad
* Add note repeat footer to MAIN screen like real 2000XL
* Imlement note repeat lock like real 2000XL: press SHIFT while holding TAP/NOTE REPEAT)
* Fix hanging voices when playing looped sounds in note repeat mode
* Fix Erase screen erase operation crash
* Import loop points of drag-and-dropped SND/WAV files
* Fix erase while recording/overdubbing -- only erase the notes of the pads you press
* Fixed a loop bug (LOOP TO was ignored)
* Fixed various 16-levels bugs
* Fixed step editor F6 (PLAY) stuck pad
* Datawheel customizable keyboard mapping
* Datawheel fine mode -- hold Shift while dragging the wheel to move +/- 17x slower
* Fix LOOP bug (thank you Son Ray Burns for pointing this out)
* Import WAV loop data -- 1st loop of the "smpl" chunk
* Improved keyboard handling, notably of Shift, Control, Alt/Option and their combinations
* Customizable keyboard mapping -- finally a road towards international, non-US keyboard support
* New screens -- VmpcKeyboard -- to manage mapping, and VmpcDiscardMapping, VmpcResetKeyboard -- to prevent unwanted (loss of) mapping changes

## v0.2.0 (11-10-2020)
* Drag-and-drop single SND/WAV files onto pads
* Large refactoring, mostly GUI related
* Punch mode
* Tap-to-set-tempo
* Fix loading and saving of most file formats
* Fixes too many bugs and crashes to mention here

## v0.1.5 (23-6-2020)
* Fix no audio input (sampling) issue in standalone

## v0.1.4 (9-5-2020)
* [Plugin compatibility matrix](https://github.com/izzyreal/vmpc-juce/blob/master/plugin-compatibility-matrix.md)
* Logic Pro X support (experimental)
* Cubase 10 support (experimental)
* Ableton support (experimental)
* Reaper support (experimental)
* Fix ASSIGNABLE MIX OUT bugs
* Return to SAVE screen after saving PGM

## v0.1.3 (5-5-2020)
* Fix various Timing Correct screen cosmetic issues and some functionality
* Honor full-level when clicking pads with the mouse
* Fix an UNDO + Timing Correct related crash
* Fix MASTER tempo source (currently no tempo changes are supported)
* Popups when saving SND/WAV as part of a PGM or APS

## v0.1.2
* Fix Erase screen crashes and cosmetic issues

## v0.1.1

* FXedit screen doesn't crash (the screen is still useless because there is no EB16 emulation)
* WAV-files that have non-data chunks will now quick-preview correctly
* No more crash after visiting the Metronome Sound screen

## v.0.1.0
* Initial versioned release
* Version number displayed in top right
* Fixed INSERT SOUND > SECTION START
* Switch between VMPC and ORIGINAL pad mapping in Shift + 0 screen

VMPC pad mapping is notes 35 to 98 in ascending order, making it much easier to control vMPC2000XL with a pad or keyboard controller. ORIGINAL is the original wacky mapping.
