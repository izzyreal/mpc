#pragma once

#include "input/HostInputEvent.hpp"
#include "input/GestureSourceTracker.hpp"

#include "client/event/ClientEvent.hpp"

#include <memory>

namespace mpc::input
{

    class KeyboardBindings;

    class HostToClientTranslator
    {
    public:
        HostToClientTranslator();
        
        std::optional<client::event::ClientEvent>
        translate(const HostInputEvent &, std::shared_ptr<KeyboardBindings>);

    private:
        GestureSourceTracker gestureSourceTracker;
    };

} // namespace mpc::input
