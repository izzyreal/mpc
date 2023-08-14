## v0.5.2 (14-8-2023)
* Experimental skin feature: place `sliders.png`, `datawheels.png` or `bg.jpg` in `~/Documents/VMPC2000XL/Skin` to replace these assets with your own. The original assets can be found here: https://github.com/izzyreal/vmpc-juce/tree/master/resources/img.
* Lock `RECORD` or `OVERDUB` button by double-clicking with the mouse. Click once to unlock. This way you can make VMPC2000XL as a plugin start recording even when it's not in focus and when it's synced to the DAW.
* Smaller macOS installer and binaries by moving resource files out of the binary and placing them in the Resources directory in the Bundle.

## v0.5.1 (29-7-2023)
* Updated documentation: https://vmpcdocs.izmar.nl
* Documentation also [available in PDF](https://vmpcdocs.izmar.nl/vmpc2000xl.pdf).
* Song mode: Do not crash when pressing PLAY when `(end of song)` is selected.
* Fixed Delete All Programs.
* Fixed step editor cosmetic issue when changing sequencer position while events are selected.
* Fix polyphonic recording.
* Implement transpose (MIDI only, like the original).
* Type names in the Name window via keyboard, pads and buttons.
* Auxiliary LCD (which can be opened by double-clicking the LCD) now has a maximize button.
* Show all keyboard mappings when hovering over the keyboard button in the top-right.
* Note repeat MIDI output.
* Fix maximum number of audio output channels. It was limited to a single stereo pair.
* Fix 16-level note repeats.
* Improve step editor navigation (remember active field when you were last in that screen better).
* Playing events in the step editor now results in the correct duration.
* PURGE to remove all unused samples is now working correctly.
* Fix `~/Documents/VMPC2000XL/Volumes/MPC2000XL` file renaming on Windows. For example if you've copied a samplepack over, the names usually need to be shortened. This was messed up and now it's better.
* Load MIDI files that were exported from Ableton Live.
* Fix macOS audio input.
* Fix macOS occasional keyboard focus issue.
* 2MB smaller binaries.
* Fix looping sounds.
* Fix Velo to Start.
* Allow play / rec / overdub in mixer screens.
* When you load a sound with a name that already exists (case insensitive) you have the option to rename or replace. The functionality of renaming was broken, and this is now fixed.
* In TRIM screen `St:` or `End:` field didn't update if their values are the same. Fixed.
* Fixed a display bug when using Punch.
* The Change Bars screen was openable (by turning the DATA wheel on the Bars field in the MAIN screen) while playing. This is now blocked.
* Added Akai MPD16 and IK iRig PADS to bundled MIDI controller presets
* Restore the bundled MIDI control presets (currently only 3, for MPD218, MPD16 and iRig PADS) by removing them from `~/Documents/VMPC2000XL/MidiControlPresets` and restarting VMPC2000XL.
* The number pad labels have been redesigned to be more legible.
* Official Windows 7 32/64 bit support. No more need to email me for Windows 7.
* [openSUSE, Fedora, Ubuntu and Debian packages]( https://software.opensuse.org//download.html?project=multimedia%3Aproaudio&package=vmpc2000xl).
* Use your favorite MIDI controller with VMPC2000XL. Go to Shift + 0 for VMPC settings, set 'MIDI control mode' to 'VMPC' and press F5 to go to the MIDI tab. Use LEARN to create a controller preset. Your active preset will be auto-saved. To name and manage multiple presets, press OPEN WINDOW. Presets are stored in `~/Documents/VMPC2000XL/MidiControlPresets`. Let me know which controller you use and I'll try to add it to VMPC2000XL's bundled presets. Currently the only bundled presets are for the Akai MPD218, Akai MPD16 and iRig PADS.
* Hold Alt/Option/Ctrl while hovering over a keyboard-controllable component to see its key.
* iOS version is ready for early access. Send me an email and I'll add you to the TestFlight programme.
* Songs are now persisted correctly.
* Standalone version now has a native window border. The "Options" button for Audio/MIDI Settings has been replaced by the gear icon in the top-right.
* Resizable auxiliary LCD, for example to put on another monitor. Double-click it to open or close auxiliary display.
* Auto-converting of WAVs is enabled by default.
* Improved MIDI sync out accuracy in standalone.
* Sync to incoming MIDI clock and transmit MIDI clock are enabled by default.
* 'Master level' in 'Mixer setup' now actually affects volume.
* [macOS] Separate utility ['FAT16 Mount Blocker'](https://drive.google.com/file/d/1xTA4-yKwb0oVeSN1okQbWHkYMRW-aaSg/view?usp=sharing) to prevent macOS from corrupting your MPC2000XL CF cards and other media. Just start the app, and as long as it's running in the background it will mount all FAT16 volumes as read-only. Note that if you only use up to 8 characters for your sound names, there is no corruption and hence no need for this utility. If you use up to 16 characters, you should use this utility.

## v0.4.4 (13-5-2022)
* Fix crash when loading 24 or 32 bit WAV files
* Fix stuck modifier keys when changing focus (DATA wheel is incrementing too much)
* Lower CPU usage

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
