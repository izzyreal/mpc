#pragma once

#include "client/event/ClientHardwareEvent.hpp"
#include "client/event/ClientMidiEvent.hpp"

#include <variant>

namespace mpc::client::event
{
    struct ClientEvent
    {
        std::variant<ClientHardwareEvent, ClientMidiEvent> payload;
    };
} // namespace mpc::client::event
