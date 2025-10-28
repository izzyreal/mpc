#pragma once

#define NOMINMAX

#include <lcdgui/LayeredScreen.hpp>

#include <disk/DiskController.hpp>

#include "Paths.hpp"

#include <vector>
#include <string>
#include <memory>

namespace mpc::engine
{
    class PreviewSoundPlayer;
} // namespace mpc::engine

namespace mpc::hardware
{
    class Hardware;
}

namespace mpc::eventregistry
{
    class EventRegistry;
}

namespace mpc::controller
{
    class ClientEventController;
}

namespace mpc::input
{
    struct HostInputEvent;
    class PadAndButtonKeyboard;
} // namespace mpc::input

namespace mpc::disk
{
    class AbstractDisk;
}

namespace mpc::lcdgui
{
    class Screens;
    class LayeredScreen;
    class ScreenComponent;
} // namespace mpc::lcdgui

namespace mpc::audiomidi
{
    class EventHandler;
    class AudioMidiServices;
    class MidiOutput;
    class MidiDeviceDetector;
} // namespace mpc::audiomidi

namespace mpc::sequencer
{
    class Sequencer;
    class Clock;
} // namespace mpc::sequencer

namespace mpc::sampler
{
    class Sampler;
}

namespace mpc::nvram
{
    struct MidiControlPreset;
    // Presets available in the default preset path,
    // loaded into memory, and maintained as presets are changed, added or
    // deleted.
} // namespace mpc::nvram

namespace mpc
{
    class Mpc
    {
    public:
    private:
        bool pluginModeEnabled = false;

        std::shared_ptr<lcdgui::LayeredScreen> layeredScreen;
        std::shared_ptr<sequencer::Sequencer> sequencer;
        std::shared_ptr<sampler::Sampler> sampler;
        std::shared_ptr<audiomidi::EventHandler> eventHandler;
        std::shared_ptr<audiomidi::AudioMidiServices> audioMidiServices;
        std::shared_ptr<mpc::audiomidi::MidiDeviceDetector> midiDeviceDetector;
        std::shared_ptr<audiomidi::MidiOutput> midiOutput;
        std::unique_ptr<mpc::disk::DiskController> diskController;
        std::shared_ptr<hardware::Hardware> hardware;
        std::shared_ptr<mpc::sequencer::Clock> clock;

    public:
        std::shared_ptr<mpc::lcdgui::Screens> screens;
        std::shared_ptr<mpc::Paths> paths;
        std::vector<std::shared_ptr<nvram::MidiControlPreset>>
            midiControlPresets;
        void init();
        void startMidiDeviceDetector();
        void panic();
        void setPluginModeEnabled(bool);
        bool isPluginModeEnabled();

        std::shared_ptr<mpc::eventregistry::EventRegistry> eventRegistry;

        std::shared_ptr<mpc::input::PadAndButtonKeyboard> padAndButtonKeyboard;
        std::shared_ptr<mpc::controller::ClientEventController>
            clientEventController;

        void dispatchHostInput(const mpc::input::HostInputEvent &hostEvent);

    public:
        std::shared_ptr<lcdgui::LayeredScreen> getLayeredScreen();
        std::shared_ptr<mpc::lcdgui::ScreenComponent> getScreen();
        std::shared_ptr<hardware::Hardware> getHardware();
        mpc::disk::DiskController *getDiskController();
        std::shared_ptr<mpc::input::PadAndButtonKeyboard>
        getPadAndButtonKeyboard();

    public:
        std::shared_ptr<sequencer::Sequencer> getSequencer();
        std::shared_ptr<sampler::Sampler> getSampler();
        mpc::engine::PreviewSoundPlayer &getBasicPlayer();
        std::shared_ptr<audiomidi::AudioMidiServices> getAudioMidiServices();
        std::shared_ptr<audiomidi::EventHandler> getEventHandler();
        std::shared_ptr<mpc::audiomidi::MidiOutput> getMidiOutput();
        std::shared_ptr<mpc::sequencer::Clock> getClock();

    public:
        std::shared_ptr<mpc::disk::AbstractDisk> getDisk();
        std::vector<std::shared_ptr<mpc::disk::AbstractDisk>> getDisks();

        Mpc();
        ~Mpc();

        static std::vector<char> akaiAsciiChar;
        static std::vector<std::string> akaiAscii;
    };
} // namespace mpc
