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

namespace mpc::input {
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

        std::shared_ptr<mpc::input::PadAndButtonKeyboard> padAndButtonKeyboard;
        std::shared_ptr<mpc::controller::ClientInputControllerBase> inputController;

        void dispatchHostInput(const mpc::input::HostInputEvent& hostEvent);

    public:
        std::shared_ptr<lcdgui::LayeredScreen> getLayeredScreen();
        std::shared_ptr<mpc::lcdgui::ScreenComponent> getScreen();
        std::shared_ptr<hardware::Hardware> getHardware();
        mpc::disk::DiskController* getDiskController();
        std::shared_ptr<mpc::input::PadAndButtonKeyboard> getPadAndButtonKeyboard();

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
    };
}

