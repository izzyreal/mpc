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
            std::shared_ptr<lcdgui::screens::MidiSwScreen>,
            std::shared_ptr<sequencer::Sequencer>,
            std::shared_ptr<lcdgui::screens::window::MidiInputScreen>,
            std::shared_ptr<audiomidi::EventHandler>,
            std::shared_ptr<lcdgui::screens::window::MultiRecordingSetupScreen>,
            std::shared_ptr<lcdgui::screens::window::TimingCorrectScreen>);

        std::shared_ptr<ClientMidiSoundGeneratorController>
        getSoundGeneratorController();

        void handleClientMidiEvent(const mpc::client::event::ClientMidiEvent &);
    };
} // namespace mpc::controller
