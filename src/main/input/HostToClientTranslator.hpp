#pragma once

#include "input/HostInputEvent.hpp"
#include "input/ClientHardwareEvent.hpp"

#include <memory>

namespace mpc::input
{

    class KeyboardBindings;

    struct HostToClientTranslator
    {
        static std::optional<ClientHardwareEvent> translate(const HostInputEvent &, std::shared_ptr<KeyboardBindings>);
    };

} // namespace mpc::input
