#pragma once

#include "command/context/TriggerDrumNoteOnContext.hpp"
#include "command/context/TriggerDrumNoteOffContext.hpp"

#include <memory>

namespace mpc
{
    class Mpc;
}

namespace mpc::lcdgui
{
    class ScreenComponent;
}

namespace mpc::command::context
{
    class TriggerDrumContextFactory
    {
    public:
        static TriggerDrumNoteOnContext buildTriggerDrumNoteOnContext(
            mpc::Mpc &mpc,
            const int programPadIndex,
            const int velocity,
            const std::shared_ptr<mpc::lcdgui::ScreenComponent>);

        static TriggerDrumNoteOffContext buildTriggerDrumNoteOffContext(
            mpc::Mpc &mpc,
            const int programPadIndex,
            std::optional<int> drumIndex,
            const std::shared_ptr<mpc::lcdgui::ScreenComponent>);
    };
} // namespace mpc::command::context
