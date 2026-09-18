#pragma once

#include "input/HostInputEvent.hpp"
#include "input/GestureSourceTracker.hpp"
#include "input/RotaryGestureHandler.hpp"

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

        void cancelGestures()
        {
            rotaryGestureHandler.cancelAll();
            gestureSourceTracker = GestureSourceTracker{};
        }

        std::optional<client::event::ClientEvent>
        translate(const HostInputEvent &,
                  std::shared_ptr<keyboard::KeyboardBindings>,
                  RotaryDragMode = RotaryDragMode::Vertical);

    private:
        GestureSourceTracker gestureSourceTracker;
        RotaryGestureHandler rotaryGestureHandler;
    };

} // namespace mpc::input
