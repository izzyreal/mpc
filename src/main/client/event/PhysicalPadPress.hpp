#pragma once

#include "client/event/ClientHardwareEvent.hpp"

#include <memory>
#include <optional>

namespace mpc::lcdgui
{
    class ScreenComponent;
}

namespace mpc::client::event
{

    struct PhysicalPadPress
    {
        int bankIndex;
        std::shared_ptr<lcdgui::ScreenComponent> screen;
        client::event::ClientHardwareEvent::Source inputSource;
        std::optional<int> drumIndex;
    };

} // namespace mpc::client::event
