#pragma once

#define NOMINMAX

#include <lcdgui/LayeredScreen.hpp>

#include <disk/DiskController.hpp>

#include <observer/Observable.hpp>

#include <Logger.hpp>

#include <vector>
#include <string>
#include <memory>

namespace mpc::engine {
    class Drum;
    class PreviewSoundPlayer;
}

namespace mpc::hardware {
    class Hardware;
}

namespace mpc::disk {
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
    class MidiInput;
    class MidiOutput;
    class MidiDeviceDetector;
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
        std::shared_ptr<lcdgui::LayeredScreen> layeredScreen;
        std::shared_ptr<controls::Controls> controls;
        std::shared_ptr<audiomidi::EventHandler> eventHandler;
        std::shared_ptr<sequencer::Sequencer> sequencer;
        std::shared_ptr<sampler::Sampler> sampler;
        std::shared_ptr<audiomidi::AudioMidiServices> audioMidiServices;
        std::shared_ptr<mpc::audiomidi::MidiDeviceDetector> midiDeviceDetector;
        std::vector<audiomidi::MidiInput*> midiInputs;
        std::shared_ptr<audiomidi::MidiOutput> midiOutput;
        std::unique_ptr<mpc::disk::DiskController> diskController;
        std::shared_ptr<hardware::Hardware> hardware;
        int bank = 0;
        int pad = 0;
        int note = 60;

        // For returning to a specific screen after for example opening the Name screen
        std::string previousSamplerScreenName;

    public:
        std::shared_ptr<mpc::lcdgui::Screens> screens;
        void init(const int inputCount, const int outputCount);
        void setBank(int i);
        int getBank();
        void setNote(int note);
        void setPad(unsigned char padIndexWithBank);
        int getNote();
        int getPad();
        std::string getPreviousSamplerScreenName();
        void setPreviousSamplerScreenName(std::string s);
        void panic();

    public:
        std::shared_ptr<lcdgui::LayeredScreen> getLayeredScreen();
        std::shared_ptr<controls::Controls> getControls();
        std::shared_ptr<mpc::lcdgui::ScreenComponent> getActiveControls();
        std::shared_ptr<mpc::controls::GlobalReleaseControls> getReleaseControls();
        std::shared_ptr<hardware::Hardware> getHardware();
        mpc::disk::DiskController* getDiskController();

    public:
        std::shared_ptr<sequencer::Sequencer> getSequencer();
        std::shared_ptr<sampler::Sampler> getSampler();
        mpc::engine::Drum& getDrum(int i);
        mpc::engine::PreviewSoundPlayer& getBasicPlayer();
        std::shared_ptr<audiomidi::AudioMidiServices> getAudioMidiServices();
        std::shared_ptr<audiomidi::EventHandler> getEventHandler();
        std::shared_ptr<mpc::audiomidi::MidiOutput> getMidiOutput();
        mpc::audiomidi::MidiInput* getMpcMidiInput(int i);

    public:
        std::shared_ptr<mpc::disk::AbstractDisk> getDisk();
        std::vector<std::shared_ptr<mpc::disk::AbstractDisk>> getDisks();

        Mpc();
        ~Mpc();

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
                "step-edit-options",
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
                "mpc2000xl-all-file",
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
                "time-display",
                "transpose-permanent",
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
                "file-exists",
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
                "resample",
                "delete-song",
                "metronome-sound",
                "delete-track",
                "copy-track",
                "copy-sequence",
                "delete-sequence",
                "delete-all-files",
                "vmpc-disks",
                "vmpc-settings",
                "vmpc-direct-to-disk-recorder",
                "vmpc-recording-finished",
                "vmpc-file-in-use",
                "vmpc-keyboard",
                "vmpc-reset-keyboard",
                "vmpc-discard-mapping-changes",
                "vmpc-auto-save",
                "vmpc-midi",
                "vmpc-midi-presets",
                "vmpc-warning-settings-ignored",
                "vmpc-known-controller-detected",
                "vmpc-continue-previous-session"
        };
    };
}
