#pragma once

#include "../client/event/ClientMidiEvent.hpp"
#include "ClientMidiSoundGeneratorController.hpp"
#include "ClientMidiSequencerController.hpp"
#include "ClientMidiFootswitchAssignmentController.hpp"
#include "ClientMidiInputSynchronizationController.hpp"

namespace mpc::controller
{
    class ClientMidiEventController
    {
    private:
        ClientMidiSoundGeneratorController soundGeneratorController;
        ClientMidiSequencerController sequencerController;
        ClientMidiFootswitchAssignmentController footswitchController;
        ClientMidiInputSynchronizationController syncController;

    public:
        ClientMidiEventController();

        void handleClientMidiEvent(const mpc::client::event::ClientMidiEvent &);
    };
} // namespace mpc::controller

