#pragma once

#include "client/event/ClientHardwareEvent.hpp"

#include "controller/Bank.hpp"

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
        controller::Bank bank;
        std::shared_ptr<lcdgui::ScreenComponent> screen;
        client::event::ClientHardwareEvent::Source inputSource;
        std::optional<int> drumIndex;
    };

} // namespace mpc::client::event
