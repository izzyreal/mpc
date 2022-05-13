## v0.4.4 (13-5-2022)
* Fix crash when loading 24 or 32 bit WAV files

## v0.4.3 (10-3-2022)
* Pads light up when dragging a WAV or SND file over them (to indicate you can drop 'm there)
* Fix crash when loading WAV files with corrupt chunk size information -- If file is too small, silence is loaded for the missing data
* Fix crash when drag 'n dropping a WAV with filename length over 16 characters

## v0.4.2 (17-1-2022)
* [Convert 24- and 32-bit and > 44.1KHz WAV files (optionally automatic)](https://vmpcdocs.izmar.nl/vmpc_specific_settings.html#auto-convert-wavs)

## v0.4.1 (26-12-2021)
* [Normalize low volume sounds](https://vmpcdocs.izmar.nl/vmpc_specific_settings.html#normalize)

## v0.4.0 (14-11-2021)
* Installers installs standalone and plugins
* Self-contained, portable binaries with [new file locations](https://vmpcdocs.izmar.nl/install.html#file-locations)
* User guide at https://vmpcdocs.izmar.nl
* [Raw USB volume support](https://vmpcdocs.izmar.nl/vmpc_specific_settings.html#configuring-usb-disk-devices-in-the-disks-tab)
* Fix AU/VST3 crashes on MacOS when the UI is closed
* Better error handling when loading/saving
* Fix bug when changing DRUM in MAIN screen
* Fix bug when setting next sequence to below 1
* Fix incorrect sound index assignment crashes when encountering missing SND/WAV files in APS/PGM files
* Fix crash when loading ALL file with sysex/mixer events
* Various fixes to custom file names when saving APS, ALL, etc.
* MIDI in respects 16 LEVELS mode
* Display waveform of very short samples correctly
* Show popup when trying to load unsupported/invalid SND/WAV
* Numeric keys are clickable with the mouse
* Data wheel, slider and rec gain/main volume knobs are mouse-wheel controllable
* Fix crash when receiving unassigned MIDI notes from external MIDI controller
* Self-recovering demo beats on Windows and MacOS (if you remove them, they will be restored anew)
* Fixed a bug when loading WAV files without loops
* Fixed a bug when saving and replacing an existing APS file

## v0.3.1 (7-3-2021)
* Native M1 included in MacOS universal binary
* Support for variable sized buffers, i.e. improved FL Studio support
* Retain plugin state when saving plugin hostproject and when changing buffer size
* Ubuntu 18.04 standalone binary published
* Auto-save programs, sounds, sequences and songs
* MIDISW screen implemented (control some of MPC2000XL's functions with MIDI CCs)
* Fix vmpc-specific setting 16 levels Erase mode -- options have become All levels (like real MPC2000XL) and Only pressed level
* Performance improvements (don't allocate memory on the audio thread)
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
