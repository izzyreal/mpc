#pragma once

#include "input/HostInputEvent.hpp"
#include "client/event/ClientHardwareEvent.hpp"

#include <memory>

namespace mpc::input
{

    class KeyboardBindings;

    struct HostToClientTranslator
    {
        static std::optional<client::event::ClientHardwareEvent> translate(const HostInputEvent &, std::shared_ptr<KeyboardBindings>);
    };

} // namespace mpc::input
