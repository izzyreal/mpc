#pragma once

#include "client/event/ClientMidiEvent.hpp"
#include "ClientMidiSoundGeneratorController.hpp"
#include "ClientMidiSequencerController.hpp"
#include "ClientMidiFootswitchAssignmentController.hpp"
#include "ClientMidiInputSynchronizationController.hpp"
#include "lcdgui/screens/MidiSwScreen.hpp"

#include "Observer.hpp"

namespace mpc::sequencer
{
    class Sequencer;
}

namespace mpc::sampler
{
    class Sampler;
}

namespace mpc::lcdgui::screens::window
{
    class MidiInputScreen;
    class MultiRecordingSetupScreen;
    class TimingCorrectScreen;
} // namespace mpc::lcdgui::screens::window

namespace mpc::audiomidi
{
    class EventHandler;
}

namespace mpc::hardware
{
    class Button;
}

namespace mpc::controller
{
    class ClientEventController;
    class ClientHardwareEventController;

    class ClientMidiEventController : public Observable
    {
    private:
        using MidiEvent = mpc::client::event::ClientMidiEvent;
        using Sequencer = mpc::sequencer::Sequencer;
        using Sampler = mpc::sampler::Sampler;
        using EventHandler = mpc::audiomidi::EventHandler;
        using MidiInputScreen = mpc::lcdgui::screens::window::MidiInputScreen;
        using MultiRecordingSetupScreen =
            mpc::lcdgui::screens::window::MultiRecordingSetupScreen;
        using TimingCorrectScreen =
            mpc::lcdgui::screens::window::TimingCorrectScreen;

        std::shared_ptr<ClientEventController> clientEventController;
        std::shared_ptr<ClientMidiSoundGeneratorController>
            soundGeneratorController;

        ClientMidiSequencerController sequencerController;
        std::shared_ptr<ClientMidiFootswitchAssignmentController>
            footswitchController;
        ClientMidiInputSynchronizationController syncController;

        std::shared_ptr<ClientHardwareEventController>
            clientHardwareEventController;

    public:
        ClientMidiEventController(
            std::shared_ptr<ClientEventController>,
            std::shared_ptr<ClientHardwareEventController>,
            std::shared_ptr<MidiSwScreen>,
            std::shared_ptr<Sequencer>,
            std::shared_ptr<Sampler>,
            std::shared_ptr<MidiInputScreen>,
            std::shared_ptr<EventHandler>,
            std::shared_ptr<MultiRecordingSetupScreen>,
            std::shared_ptr<TimingCorrectScreen>);

        std::shared_ptr<ClientMidiSoundGeneratorController>
        getSoundGeneratorController();

        void handleClientMidiEvent(const mpc::client::event::ClientMidiEvent &);
    };
} // namespace mpc::controller
