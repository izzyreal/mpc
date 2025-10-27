#pragma once

#include "input/HostInputEvent.hpp"
#include "client/event/ClientEvent.hpp"

#include <memory>

namespace mpc::input
{

    class KeyboardBindings;

    struct HostToClientTranslator
    {
        static std::optional<client::event::ClientEvent>
        translate(const HostInputEvent &, std::shared_ptr<KeyboardBindings>);
    };

} // namespace mpc::input
