#pragma once

#include "../client/event/ClientMidiEvent.hpp"
#include <iostream>

namespace mpc::controller
{
    class ClientMidiSoundGeneratorController
    {
    public:
        ClientMidiSoundGeneratorController() = default;
        void handleEvent(const mpc::client::event::ClientMidiEvent &);
    };
} // namespace mpc::controller
