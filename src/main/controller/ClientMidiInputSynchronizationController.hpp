#pragma once

#include "../client/event/ClientMidiEvent.hpp"

namespace mpc::controller
{
    class ClientMidiInputSynchronizationController
    {
    public:
        ClientMidiInputSynchronizationController() = default;
        void handleEvent(const mpc::client::event::ClientMidiEvent &);
    };
} // namespace mpc::controller
