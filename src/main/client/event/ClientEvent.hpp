#pragma once

#include "client/event/ClientHardwareEvent.hpp"

#include <utility>

namespace mpc::client::event
{

    struct ClientEvent
    {
        std::variant<ClientHardwareEvent> payload;
    };
} // namespace mpc::client::event
