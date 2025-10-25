#pragma once

#include "../client/event/ClientMidiEvent.hpp"

namespace mpc::controller {
    class ClientMidiEventController {
    public:
        ClientMidiEventController();

        void handleClientMidiEvent(const mpc::client::event::ClientMidiEvent &);


    };
} // namespace mpc::controller
