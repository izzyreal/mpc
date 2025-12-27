#pragma once

#include "input/HostInputEvent.hpp"
#include "input/GestureSourceTracker.hpp"

#include "client/event/ClientEvent.hpp"

#include <memory>

namespace mpc::input::keyboard
{
    class KeyboardBindings;
}

namespace mpc::input
{
    class HostToClientTranslator
    {
    public:
        HostToClientTranslator();

        std::optional<client::event::ClientEvent>
        translate(const HostInputEvent &, std::shared_ptr<keyboard::KeyboardBindings>);

    private:
        GestureSourceTracker gestureSourceTracker;
    };

} // namespace mpc::input
