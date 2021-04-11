#pragma once

#include <lcdgui/LayeredScreen.hpp>

#include <disk/DiskController.hpp>
#include <disk/ProgramLoader.hpp>

#include <observer/Observable.hpp>

#include <Logger.hpp>

#include <vector>
#include <string>
#include <memory>

namespace ctoot::mpc {
class MpcSoundPlayerChannel;
class MpcBasicSoundPlayerChannel;
class MpcMultiMidiSynth;
}

namespace mpc::hardware {
class Hardware;
}

namespace mpc::disk {
class Stores;
class AbstractDisk;
}

namespace mpc::controls {
class Controls;
class BaseControls;
class GlobalReleaseControls;
}

namespace mpc::lcdgui {
class Screens;
class LayeredScreen;
class ScreenComponent;
}

namespace mpc::audiomidi {
class EventHandler;
class AudioMidiServices;
class MpcMidiInput;
class MpcMidiPorts;
}

namespace mpc::sequencer {
class Sequencer;
}

namespace mpc::sampler {
class Sampler;
}

namespace mpc {
class Mpc
: public moduru::observer::Observable {
private:
    std::unique_ptr<mpc::disk::ProgramLoader> programLoader;
    std::shared_ptr<lcdgui::LayeredScreen> layeredScreen;
    std::shared_ptr<controls::Controls> controls;
    std::shared_ptr<audiomidi::EventHandler> eventHandler;
    std::shared_ptr<sequencer::Sequencer> sequencer;
    std::shared_ptr<sampler::Sampler> sampler;
    std::shared_ptr<audiomidi::AudioMidiServices> audioMidiServices;
    std::vector<audiomidi::MpcMidiInput*> mpcMidiInputs;
    std::unique_ptr<mpc::disk::DiskController> diskController;
    std::shared_ptr<hardware::Hardware> hardware;
    int bank = 0;
    int pad = 0;
    int note = 60;
    int prevNote = 60;
    int prevPad = -1;
    
    // For returning to a specific screen after for example opening the Name screen
    std::string previousSamplerScreenName = "";
    
public:
    std::shared_ptr<mpc::lcdgui::Screens> screens;
    void init(const int sampleRate, const int inputCount, const int outputCount);
    void setBank(int i);
    int getBank();
    void setPadAndNote(int pad, int note);
    int getNote();
    int getPad();
    int getPrevNote();
    int getPrevPad();
    std::string getPreviousSamplerScreenName();
    void setPreviousSamplerScreenName(std::string s);
    
public:
    std::weak_ptr<lcdgui::LayeredScreen> getLayeredScreen();
    std::weak_ptr<controls::Controls> getControls();
    std::weak_ptr<mpc::lcdgui::ScreenComponent> getActiveControls();
    std::shared_ptr<mpc::controls::GlobalReleaseControls> getReleaseControls();
    std::weak_ptr<hardware::Hardware> getHardware();
    
public:
    std::weak_ptr<sequencer::Sequencer> getSequencer();
    std::weak_ptr<sampler::Sampler> getSampler();
    ctoot::mpc::MpcSoundPlayerChannel* getDrum(int i);
    ctoot::mpc::MpcBasicSoundPlayerChannel* getBasicPlayer();
    std::weak_ptr<audiomidi::AudioMidiServices> getAudioMidiServices();
    std::vector<ctoot::mpc::MpcSoundPlayerChannel*> getDrums();
    std::weak_ptr<audiomidi::EventHandler> getEventHandler();
    ctoot::mpc::MpcMultiMidiSynth* getMms();
    std::weak_ptr<mpc::audiomidi::MpcMidiPorts> getMidiPorts();
    mpc::audiomidi::MpcMidiInput* getMpcMidiInput(int i);
    
public:
    void loadProgram();
    void importLoadedProgram();
        
public:
    std::weak_ptr<mpc::disk::AbstractDisk> getDisk();
    std::weak_ptr<mpc::disk::Stores> getStores();
    
public:
    Mpc();
    ~Mpc();
    
public:
    static std::vector<char> akaiAsciiChar;
    static std::vector<std::string> akaiAscii;
    
private:
    std::vector<std::string> screenNames {
        "tr-move",
        "black",
        "mpc2000xl",
        "sequencer",
        "next-seq",
        "mixer",
        "step-editor",
        "load",
        "save",
        "select-drum",
        "select-mixer-drum",
        "mixer-setup",
        "program-assign",
        "program-params",
        "format",
        "setup",
        "events",
        "bars",
        "tr-move",
        "user",
        "drum",
        "purge",
        "fx-edit",
        "others",
        "init",
        "ver",
        "sync",
        "dump",
        "midi-sw",
        "sample",
        "trim",
        "loop",
        "zone",
        "params",
        "song",
        "punch",
        "trans",
        "second-seq",
        "assign",
        "track-mute",
        "next-seq-pad",
        "audio",
        "audio_no_start",
        "midi",
        "mpc2000xlallfile",
        "copy-note-parameters",
        "assignment-view",
        "init-pad-assign",
        "directory",
        "load-a-program",
        "save-all-file",
        "save-aps-file",
        "save-a-program",
        "load-aps-file",
        "load-a-sequence",
        "load-a-sequence-from-all",
        "save-a-sequence",
        "edit-multiple",
        "paste-event",
        "step-timing-correct",
        "insert-event",
        "load-a-sound",
        "save-a-sound",
        "keep-or-retry",
        "format-disk",
        "channel-settings",
        "program",
        "sound",
        "velocity-modulation",
        "velo-env-filter",
        "velo-pitch",
        "mute-assign",
        "auto-chromatic-assignment",
        "copy-effect-settings",
        "distortion-ring-mod",
        "4bandfilter",
        "effect-mixer",
        "modulation",
        "modulation-rotary",
        "modulation-fmod-auto-pan",
        "modulation-pitch-shift",
        "modulation-pitch-feedback",
        "delay-echo",
        "delay-echo-stereo",
        "reverb",
        "reverb-gated",
        "sound-memory",
        "start-fine",
        "end-fine",
        "loop-to-fine",
        "loop-end-fine",
        "zone-start-fine",
        "zone-end-fine",
        "edit-sound",
        "edit-timestretch",
        "song-name",
        "ignore-tempo-change",
        "loop-song-window",
        "loop-bars-window",
        "convert-song-to-seq",
        "time-display",
        "transpose-permanent",
        "locate",
        "erase",
        "tempo-change",
        "timing-correct",
        "change-tsig",
        "change-bars",
        "change-bars-2",
        "count-metronome",
        "erase-all-off-tracks",
        "transmit-program-changes",
        "multi-recording-setup",
        "midi-input",
        "midi-input-monitor",
        "midi-output",
        "midi-output-monitor",
        "edit-velocity",
        "assign-16-levels",
        "sequence",
        "track",
        "number-of-zones",
        "cant-find-file",
        "name",
        "file-already-exists",
        "vmpc-record-jam",
        "delete-all-sequences",
        "delete-all-tracks",
        "delete-program",
        "delete-folder",
        "delete-file",
        "create-new-program",
        "copy-program",
        "copy-sound",
        "delete-all-programs",
        "delete-sound",
        "delete-all-sound",
        "convert-sound",
        "mono-to-stereo",
        "stereo-to-mono",
        "convert-no-do-it",
        "resample",
        "bpm-match",
        "delete-song",
        "metronome-sound",
        "delete-track",
        "copy-track",
        "copy-sequence",
        "delete-sequence",
        "midi-monitor",
        "delete-all-files",
        "vmpc-disk",
        "vmpc-settings",
        "vmpc-direct-to-disk-recorder",
        "vmpc-recording-finished",
        "vmpc-file-in-use",
        "vmpc-keyboard",
        "vmpc-reset-keyboard",
        "vmpc-discard-mapping-changes",
        "vmpc-auto-save"
    };
};
}
