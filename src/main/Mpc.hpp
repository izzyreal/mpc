#pragma once

#define NOMINMAX

#include <lcdgui/LayeredScreen.hpp>

#include <disk/DiskController.hpp>

#include <Observer.hpp>

#include <Logger.hpp>

#include "Paths.hpp"

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

namespace mpc::controller {
    class ClientInputControllerBase;
}

namespace mpc::inputlogic {
    struct HostInputEvent;
    class PadAndButtonKeyboard;
}

namespace mpc::disk {
    class AbstractDisk;
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
    class Clock;
}

namespace mpc::sampler {
    class Sampler;
}

namespace mpc::nvram {
    struct MidiControlPreset;
    // Presets available in the default preset path,
    // loaded into memory, and maintained as presets are changed, added or deleted.
}

namespace mpc {
    class Mpc : public Observable {
    private:
        bool afterEnabled = false;
        bool fullLevelEnabled = false;
        bool sixteenLevelsEnabled = false;
        bool pluginModeEnabled = false;
        std::shared_ptr<lcdgui::LayeredScreen> layeredScreen;
        std::shared_ptr<sequencer::Sequencer> sequencer;
        std::shared_ptr<sampler::Sampler> sampler;
        std::shared_ptr<audiomidi::EventHandler> eventHandler;
        std::shared_ptr<audiomidi::AudioMidiServices> audioMidiServices;
        std::shared_ptr<mpc::audiomidi::MidiDeviceDetector> midiDeviceDetector;
        std::vector<audiomidi::MidiInput*> midiInputs;
        std::shared_ptr<audiomidi::MidiOutput> midiOutput;
        std::unique_ptr<mpc::disk::DiskController> diskController;
        std::shared_ptr<hardware::Hardware> hardware;
        std::shared_ptr<mpc::sequencer::Clock> clock;
        int bank = 0;
        int pad = 0;
        int note = 60;

        // For returning to a specific screen after for example opening the Name screen
        std::string previousSamplerScreenName;

    public:
        std::shared_ptr<mpc::lcdgui::Screens> screens;
        std::shared_ptr<mpc::Paths> paths;
        std::vector<std::shared_ptr<nvram::MidiControlPreset>> midiControlPresets;
        void init();
        void startMidiDeviceDetector();
        void setBank(int i);
        int getBank();
        void setNote(int note);
        void setPad(unsigned char padIndexWithBank);
        int getNote();
        int getPad();
        std::string getPreviousSamplerScreenName();
        void setPreviousSamplerScreenName(std::string s);
        void panic();
        void setPluginModeEnabled(bool);
        bool isPluginModeEnabled();

        std::shared_ptr<mpc::inputlogic::PadAndButtonKeyboard> padAndButtonKeyboard;
        std::shared_ptr<mpc::controller::ClientInputControllerBase> inputController;

        void dispatchHostInput(const mpc::inputlogic::HostInputEvent& hostEvent);

    public:
        std::shared_ptr<lcdgui::LayeredScreen> getLayeredScreen();
        std::shared_ptr<mpc::lcdgui::ScreenComponent> getActiveControls();
        std::shared_ptr<hardware::Hardware> getHardware();
        mpc::disk::DiskController* getDiskController();
        std::shared_ptr<mpc::inputlogic::PadAndButtonKeyboard> getPadAndButtonKeyboard();

    public:
        std::shared_ptr<sequencer::Sequencer> getSequencer();
        std::shared_ptr<sampler::Sampler> getSampler();
        mpc::engine::Drum& getDrum(int i);
        mpc::engine::PreviewSoundPlayer& getBasicPlayer();
        std::shared_ptr<audiomidi::AudioMidiServices> getAudioMidiServices();
        std::shared_ptr<audiomidi::EventHandler> getEventHandler();
        std::shared_ptr<mpc::audiomidi::MidiOutput> getMidiOutput();
        mpc::audiomidi::MidiInput* getMpcMidiInput(int i);
        std::shared_ptr<mpc::sequencer::Clock> getClock();

        bool isAfterEnabled() const;
        void setAfterEnabled(bool);
        bool isFullLevelEnabled() const;
        void setFullLevelEnabled(bool);
        bool isSixteenLevelsEnabled() const;
        void setSixteenLevelsEnabled(bool);

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
                "vmpc-continue-previous-session",
                "locate"
        };
    };
}
