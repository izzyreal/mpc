#pragma once

#include "../client/event/ClientMidiEvent.hpp"

namespace mpc::controller
{
    class ClientMidiSequencerController
    {
    public:
        ClientMidiSequencerController() = default;
        void handleEvent(const mpc::client::event::ClientMidiEvent &);
    };
} // namespace mpc::controller
