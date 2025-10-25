#pragma once

#include "../client/event/ClientMidiEvent.hpp"
#include <iostream>

namespace mpc::controller
{
    class ClientMidiFootswitchAssignmentController
    {
    public:
        ClientMidiFootswitchAssignmentController() = default;
        void handleEvent(const mpc::client::event::ClientMidiEvent &);
    };
} // namespace mpc::controller
