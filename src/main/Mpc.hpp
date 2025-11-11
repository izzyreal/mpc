#pragma once

#define NOMINMAX

#include "lcdgui/LayeredScreen.hpp"

#include "disk/DiskController.hpp"

#include "Paths.hpp"

#include <vector>
#include <string>
#include <memory>

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
    class EngineHost;
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
        std::shared_ptr<audiomidi::EngineHost> audioMidiServices;
        std::shared_ptr<audiomidi::MidiDeviceDetector> midiDeviceDetector;
        std::shared_ptr<audiomidi::MidiOutput> midiOutput;
        std::unique_ptr<disk::DiskController> diskController;
        std::shared_ptr<hardware::Hardware> hardware;
        std::shared_ptr<sequencer::Clock> clock;

    public:
        std::shared_ptr<lcdgui::Screens> screens;
        std::shared_ptr<Paths> paths;
        std::vector<std::shared_ptr<nvram::MidiControlPreset>>
            midiControlPresets;
        void init();
        void startMidiDeviceDetector();
        void panic() const;
        void setPluginModeEnabled(bool);
        bool isPluginModeEnabled() const;

        std::shared_ptr<eventregistry::EventRegistry> eventRegistry;

        std::shared_ptr<input::PadAndButtonKeyboard> padAndButtonKeyboard;
        std::shared_ptr<controller::ClientEventController>
            clientEventController;

        void
        dispatchHostInput(const input::HostInputEvent &hostEvent) const;

    public:
        std::shared_ptr<lcdgui::LayeredScreen> getLayeredScreen();
        std::shared_ptr<lcdgui::ScreenComponent> getScreen() const;
        std::shared_ptr<hardware::Hardware> getHardware();
        disk::DiskController *getDiskController() const;
        std::shared_ptr<input::PadAndButtonKeyboard>
        getPadAndButtonKeyboard();

    public:
        std::shared_ptr<sequencer::Sequencer> getSequencer();
        std::shared_ptr<sampler::Sampler> getSampler();
        std::shared_ptr<audiomidi::EngineHost> getEngineHost();
        std::shared_ptr<audiomidi::EventHandler> getEventHandler();
        std::shared_ptr<audiomidi::MidiOutput> getMidiOutput();
        std::shared_ptr<sequencer::Clock> getClock();

    public:
        std::shared_ptr<disk::AbstractDisk> getDisk() const;
        std::vector<std::shared_ptr<disk::AbstractDisk>> getDisks() const;

        Mpc();
        ~Mpc();

        static std::vector<char> akaiAsciiChar;
        static std::vector<std::string> akaiAscii;
    };
} // namespace mpc
