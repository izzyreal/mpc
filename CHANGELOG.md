## v0.9.0 RC21 (1-2-2026)
* Persist last used ALL/APS name.
* Fix MIDI track crash upon MIDI input.
* Fix first metronome click during REC.

## v0.9.0 RC20 (31-1-2026)
* Fix crash when setting selected sequence in Next Seq screen.
* Fix Erase Events.

## v0.9.0 RC19 (30-1-2026)
* Fix DRUM program selection when restoring plugin state.
* Fix an issue with 16 levels.
* Remember last saved APS and ALL name in SAVE screen.
* Fix UI updates in Velocity Modulation, Velo/Env >> Filter, and Velo >> Pitch windows.

## v0.9.0 RC18 (25-1-2026)
* Fix state persistence in plugin.
* Fix "copy bars" functionality in `BARS` screen.

## v0.9.0 RC17 (23-1-2026)
* UPDATE: This release was retracted on 25-1-2026 due to a problem with state persistence in plugin that could easily lead to users losing important work they created in VMPC2000XL.
* Fix random crashes when hitting pads. It seems iPadOS users were most affected by this bug, but it may well have affected users on other
* Improved extended MIDI control, the feature that allows you to use a MIDI controller to control VMPC2000XL's virtual hardware. The GUI for extended MIDI controls has remained largely the same. Quick summary of the current implementation compared to previous versions:

    - Button-like targets (i.e. all actual buttons and DATA wheel - and DATA wheel +) have a v: column. This value serves as a threshold, meaning CC events with values equal or greater than the value are interpreted as "press", and below the value as "release" (where a "release" doesn't do anything for the 2 DATA wheel targets).

    - The singular data wheel target (without - or + suffix) is meant to be associated with an endless dial on one's MIDI controller. This new build of VMPC2000XL anticipates 2 common encoding schemes, which are named RSL (relative stateless) and RSF (relative stateful). The Akai MPD218 that I have emits RSL when in the MPD218 editor I set the TYPE: for a given knob to INC/DEC 2. The MIDI events for this encoding scheme correspond with CC events with values between 1 and 63 for clockwise turns, and values between 64 and 127 for counterclockwise turns. I also have an iRig PADS, which has a notched encoder that by default emits RSF. My hope is that RSL and RSF cover most people's needs. If not, please contact me and I'd be happy to dig in. I really want this feature to work perfectly.

* Removed 'MIDI control mode'. That main reason this was introduced was to avoid duplicate handling of MIDI note input (once by the original MPC2000XL input handling, and once by VMPC2000XL's extended MIDI control handling). In the updated version, VMPC2000XL omits original 2000XL MIDI note input handling if you've configured a binding for that note in the VMPC MIDI tab (Shift + 0, F5).

* Thread-safe code (finally!). This means less random bugs, audio glitches and total crashes.

* No blocking operations on the audio thread. This means less audio glitches at lower latencies, as well as when you have a lot of plugins running simultaneously in your DAW.

* New keyboard configuration and MIDI control preset file formats. Both are now in JSON, making them human-readable, and somewhat editable outside VMPC2000XL.


## v0.9.0 RC16 (22-9-2025)
* Fix random crashes when hitting pads. It seems iPadOS users were most affected by this bug, but it may well have affected users on other platforms.
* Fix sound preview when hitting pads in the TRIM/LOOP/ZONE screens. VMPC2000XL used to play whatever sound is assigned to the pad you hit, but this behaviour is incongruent with the real MPC2000XL. It should play the selected sound, regardless of which pad you hit.
* Allow MIDI control presets to specify a value for the CC mappings. See https://vmpcdocs.izmar.nl/0.9.0.0/vmpc_specific.html#midi-control.
* Add a bundled MIDI control preset for Akai MPC Studio.
* Add a demo beat called "TRAIN" because the author of VMPC2000XL made this a while ago while traveling by train.
* Set correct 'Loop to' value after recording a new sound.
* Set correct 'St', 'End' and 'Loop to' after resampling.
* Fix saving and loading of LOOP information of a Song.
* Fix an issue with the initial new name when changing the name of a sound.
* Don't reset new sample rate in the Resample screen after changing the new name.
* Fix an issue with the playback pitch of sounds after having changed the audio engine or host's sample rate.
* Fix audio input/output device selection bug. Sometimes when changing the input device, the output device would also be changed, and vice versa.
* [Windows] Statically linked CRT. This means that it's not necessary anymore to install a Visual C++ Redistributable to make VMPC2000XL work on older systems.
* [Windows 32 bit] Fix VST3 and LV2 installation path. The installer used to place these files in "C:\Program Files (x86)\Common Files\", which is incorrect. They should be placed in "C:\Program Files\Common Files\".

## v0.9.0 RC12 (11-9-2025)
* Fix crash in slider Assign screen when hitting a pad in bank B, C or D.
* Fix 16 LEVELS handling of pads in bank B, C or D.
* Fix issues with recording note events while the sequencer is not running. Quite a few users may already know that note events can be recorded in the Step Editor, by hitting one or more of the pads. A metronome will start playing, and by default the actual duration of how long the pad is pressed is assigned to the note event's duration. Another feature of the real MPC2000XL is that you can achieve the same, but in the Main screen. To do this, you have to hit pads while holding the REC button. Both of these features were not working well in VMPC2000XL, but now they should be fixed.
* Fix an issue with assigning values in the VMPC2000XL-specific MIDI screen. Note and CC values were wrapping around to their lowest possible value if you would attempt to increase them beyond their highest possible value.

## v0.9.0 RC11 (10-9-2025)
* [plugin] Fix crash on Intel Mac in Reaper with certain output configurations. This may affect other systems in a similar way.
* Fix go-to-previous/next-event. This used to work fine, but in v0.9.0.4 a regression was introduced. Now it works like it should again.

## v0.9.0 RC9 (6-9-2025)
* [plugin] Ensure buffers are cleared after plugin is opened. There was an issue in Reaper, and quite possibly in other hosts too, where right after opening VMPC2000XL, the multi-channel tracks were muted because the signal contained absurdly high amplitude values. This happens if a buffer is not properly cleared. This bug is now fixed.

## v0.9.0 RC8 (3-9-2025)
* [plugin] Implement musical position synchronization to the DAW. Wherever you place the play head in your DAW, VMPC2000XL positions its internal sequencer reasonably. Most likely this only works as expected if VMPC2000XL uses the same time signature as the DAW, and neither VMPC2000XL or the DAW vary the time signature anywhere in the arrangement.
* [plugin] The DAW's play head can be repositioned while playing, and VMPC2000XL follows.
* [plugin] Fix synchronization with Cubase. In earlier versions of VMPC2000XL this wasn't working at all.
* [plugin] VMPC2000XL now also supports synchronizing to the DAW in Song mode.
* [standalone] Fix individual outputs. v0.9.0.0 introduced a regression due to the complete rewrite of multibus support. This has been fixed.
* Fix conflict between ordinary typing and typing via the pads in the Name screen.
* Make ordinary typing configurable. See Shift + 0 for VMPC2000XL-specific settings. If `Name typing w. keybd` is set to `NO`, you can only enter names via the cursor keys and DATA wheel, or via the pads.
* Fix small cosmetic bug when using the slider in the `TRIM` screen to change the `End` value.
* Hide unused fields in the timestretch functionality. Timestretch in VMPC2000XL isn't nearly as good and complete as the MPC2000XL's timestretch, in particular because it doesn't implement the `Preset` and `Adjust` fields. Previously, VMPC2000XL was showing these fields, even though they are ignored by the timestretch algorithm. This is very misleading, and it probably has wasted precious time of users who would keep trying to timestretch with different settings that didn't actually make a difference in the result.
* Allow the `Open Window` key to close the opened window, just like on the real MPC2000XL.

## v0.9.0 RC4 (29-7-2025)
* Fix slider UI updates when moving the slider via MIDI.
* Fix Song mode DAW synchronization.
* In the Mixer screens, holding down SHIFT and tapping the drum pads, allows you to select multiple channels simultaneously. This is an official MPC2000XL feature that was missing all these years.
* Fix various multi-bus issues with the VST3 version. The issues were noticed in Reaper, but it's possible other hosts had issues too. The wrong audio was sent to the wrong bus, and after changing the individual output of a pad, there was noise in the buffer. Additionally in Reaper many of the additional buses would at times be muted after opening the plugin. Also the initial channel configuration in Reaper was incorrect.
* Follow DAW tempo, even when the DAW is not playing.
* Fix iOS Bluetooth MIDI permissions.
* Show the disclaimer only once. It was showing every time you opened the GUI in some hosts for some plugin formats.

## v0.9.0 RC0 (28-2-2025)
* Completely new UI based on vector graphics
* Improved multi-bus plugin support
* LV2 for macOS and Windows

## v0.6.4 (24-10-2024)

* Fix various issues when using more than 127 sounds.
* Implement 256 sounds in memory limit. Before this, VMPC2000XL would allow over 256 sounds in memory at a time, which is incongruent and incompatible with the real MPC2000XL.
* Fix iOS keyboard configuration and learn (i.e. `Shift + 0` and then `F2` for `KEYBRD`).
* Fix sound and program name derivation. To summarize, the real MPC2000XL uses the sound or program name that is encoded in the actual the file data -- with mixed casing, like `CoolKick` or `NewPgm-A` -- if, and only if, the encoded name is a case-insensitive equivalent of the file name. If it's not, the file name becomes the sound or program name, resulting in upper casing, like `COOLKICK` or `NEWPGM-A`. VMPC2000XL now follows this behaviour.
* Minor cosmetic fixes in `KEYBRD`, `Copy Sound`, and `Channel Settings` screens.
* For developers: I'm learning Vim, so I've made some minor changes to facilitate working on VMPC2000XL in Vim. The main change is the creation of `compile_commands.json` during the CMake generation stage. The rest of my setup can be found [here](https://github.com/izzyreal/vim-config).

A special thank you to all the bug reporters on GitHub!

## v0.6.0 (15-6-2024)
* First public iOS TestFlight release: https://testflight.apple.com/join/yEgfYk21
* Allow typing tempo, bar/beat/clock, and velocity ratio in MAIN screen
* Do not ignore attack when dcy md is start
* Fix recording time when sampling
## v0.5.14.4 (29-1-2024)
* Fix crash when quickly and repeatedly previewing SND or WAV files in the LOAD or Directory screen.
* Implement Locate screen. This screen and its functionality, which is present on the real MPC2000XL, was simply not implemented in VMPC2000XL so far. Now it is. The locations are persisted to the ALL file as well.
* Fix various ALL-file persistence issues.
* Never rename `.DS_Store` file as part of making all files Akai-name compatible. This should avoid some garbage files accumulating in your MPC2000XL directory.
* Fix hanging clock after using pads to add notes via step editor.

## v0.5.14.3 (5-1-2024)
* Implement ALL file song first loop step, last loop step and loop enabled reading and writing.
* Fix loading songs from ALL files created by the real MPC2000XL.
* Fix previewing 24-bit WAV files and WAV files with non-fmt chunks before the fmt chunk via the PLAY function in the LOAD and Directory screens.

## v0.5.14.2 (31-12-2023)
* Fix song mode when running VMPC2000XL as a plugin.

## v0.5.14.1 (30-12-2023, macOS only)
* This macOS only release addresses an issue with asking for microphone/audio input permissions in the standalone version.

## v0.5.14 (29-12-2023, all systems except iPadOS)
* [plugin] Keep following the host tempo after loading TEST1 or TEST2 demo beats. VMPC2000XL uses the MIDI/SYNC input and output modes to determine whether to follow the host tempo and transport. These settings are stored in `.ALL` files, and therefor also in standalone auto-save and DAW project data. With this update, when you're running VMPC2000XL as a plugin, the MIDI/SYNC input and output modes are not changed when loading a `.ALL` file or restoring a DAW project.
* Fix 'Hold pads or keys to erase' while overdubbing.

## iPadOS build v0.5.13 (3) (28-12-2023)
* Better resizing in AUv3.
* [AUv3] Keep following the host tempo after loading TEST1 or TEST2 demo beats. See v0.5.14 above for more explanation.
* Fix 'Hold pads or keys to erase' while overdubbing.

## v0.5.13 (28-12-2023)
iPadOS specific improvements:
* AUv3 and Standalone now share the same documents (including imported samples).
* There's a share/export button. You can share the following things: APS, ALL and SND files of the active state, a selected (in the LOAD screen) file or directory, or a direct-to-disk recording (aka bounce). Directories and recordings are always zipped before sharing/exporting. There are many share destinations available: Save to Files, AirDrop, etc.
* There's a Recording Manager button (folder icon). It shows a list of direct-to-disk recordings, which you can play and delete.
* The standalone application can only be used in landscape orientation.
* iPhoneOS support has been dropped. The screen is too small for the current UI and there are no concrete plans to make a UI that is suitable for iOS devices that are smaller than the smallest iPad. So from now on the mentioning of iOS will be phased out in favor of iPadOS.

Please note that the VMPC2000XL folder that is located in "On My iPad" is not used anymore starting with this version (v0.5.13) of VMPC2000XL. Make sure any important files you have there are moved out, because in the next iPad release of VMPC2000XL this folder will be removed completely.

Generic improvements:
* Fixed a regression in direct-to-disk recording where the recording process would never start.
* Direct-to-disk recordings are now stored in a directory with a nice name with timestamp. They will not be overwritten anymore.
* Direct-to-disk recordings that are silent are automatically removed right after finishing recording.

## v0.5.12 (24-12-2023)
* Recording new sounds while your audio engine is running at a sample rate other than 44100 Hz was resulting in corrupted sounds and crashes. This is now fixed.

## v0.5.11 (18-12-2023)
* Improve plugin-to-host synchronization. You can now change the tempo in the host however you like, including big changes in tempo, for example from 30 to 300 BPM in an instant or other way around. It should work with any buffer size, as long as the host starts at the start of a bar. If you have a loop configured in the host, this should also work. You can also start playing in VMPC2000XL when the host is not playing.
* [macOS] VST3 on older macOS versions was not working due to an issue with the version of Xcode I was using for my builds. This is now fixed.

## v0.5.10 (15-12-2023)
* Fix plugin-to-host synchronization. The problem that was fixed in this version is that VMPC2000XL used to start playing too late. But note that at the moment tempo changes from the host are handled ok on the iPad version in AUM, but not in Ableton Live and possibly other DAWs. More investigation and improvements needed, so for now to keep VMPC2000XL in sync with the DAW use small buffer sizes (preferably 128 or lower), keep the tempo static, and start playing from the start of a bar. By the way, if you notice VMPC2000XL is not syncing to the host at all, check if you have MIDI clock enabled in the MIDI/SYNC screen (Shift + 9):
<img width="407" alt="image" src="https://github.com/izzyreal/mpc/assets/3707432/52832a21-e512-4c94-87cb-52838c4274ae">


## v0.5.9 (5-12-2023)
* Fix tempo change bug when tempo source is master (shout out to playpm for reporting this bug)

## v0.5.8 (3-12-2023)
* Allow custom skins. See https://github.com/izzyreal/vmpc-juce/tree/master/resources/img for the original bitmaps. Place files with those file names in `~/Documents/VMPC2000XL/Skin` to override the originals. At the moment the background, slider and datawheel are skinnable. Let me know if you need anything else to be skinnable.
* Fixed an issue with Timing Correct. It was ignoring the note range.
* Support using the slider in the Zone screen, as well as in all the Fine screens. As usual, hold Shift while moving the slider to change Start, End or Loop To.
* Fixed unnecessary memory allocation.
* Optimized memory and disk usage while recording and previewing sounds, and when bouncing sequences and songs to disk.
* Support key repeats on Linux. Note that the delay and interval are hardcoded at 400ms and 25ms respectively.
* Added demo beats to Linux and iOS versions.
* Updated documentation: https://vmpcdocs.izmar.nl/ and https://vmpcdocs.izmar.nl/vmpc2000xl.pdf
* [internal] [Unit tests](https://github.com/izzyreal/mpc/tree/master/src/test) are now run in the CI/CD. Although we don't have that many unit tests, a bunch of important functionality that was historically fragile is now always tested before publishing a new release.
* Additional info regarding AU use in Logic: https://github.com/izzyreal/vmpc-juce/blob/master/plugin-compatibility-matrix.md

## v0.5.7 (3-10-2023)
* Fix a rare plugin crash related to MIDI handling after unloading and loading the plugin. Standalone was and still is unaffected by this issue.

## v0.5.6 (20-9-2023)
* Fix looping sounds. They resulted in hanging notes.

## v0.5.5 (19-9-2023)
* Fix insert bars and changing time signature. It was fine in trivial cases, but it corrupted the less trivial ones. It should now work properly. This was affecting the Convert Song to Seq functionality, which should now also be reliable.

## v0.5.4 (17-9-2023)
* Implement Convert Song to Seq (shout out to c3ntr0n800 for the reminder).
* Fix Erase window issue. It wasn't removing all the events that should be erased.

## v0.5.3.1 (23-8-2023, Linux only release)
Fixes Edit window | NORMALIZE. It was corrupting the sound due to my absentminded use of `abs` rather than `std::fabs`, `min` rather than `std::fmin` and same for `fmax`.

## v0.5.3 (19-8-2023)
* Fix sampling at 44.1KHz.
* Fix waveform display of right channel in TRIM, LOOP and ZONE screens.
* Fix waveform display in Linux (it was messing up the lower half).

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
