#pragma once

#include "../client/event/ClientMidiEvent.hpp"
#include "ClientMidiSoundGeneratorController.hpp"
#include "ClientMidiSequencerController.hpp"
#include "ClientMidiFootswitchAssignmentController.hpp"
#include "ClientMidiInputSynchronizationController.hpp"
#include "lcdgui/screens/MidiSwScreen.hpp"

namespace mpc::sequencer
{
    class Sequencer;
}

namespace mpc::controller
{
    class ClientHardwareEventController;

    class ClientMidiEventController
    {
    private:
        ClientMidiSoundGeneratorController soundGeneratorController;
        ClientMidiSequencerController sequencerController;
        std::shared_ptr<ClientMidiFootswitchAssignmentController> footswitchController;
        ClientMidiInputSynchronizationController syncController;

        std::shared_ptr<ClientHardwareEventController> clientHardwareEventController;

    public:
        ClientMidiEventController(std::shared_ptr<ClientHardwareEventController>, std::shared_ptr<lcdgui::screens::MidiSwScreen>, std::shared_ptr<sequencer::Sequencer>);

        void handleClientMidiEvent(const mpc::client::event::ClientMidiEvent &);
    };
} // namespace mpc::controller

