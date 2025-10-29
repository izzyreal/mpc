#pragma once

#include "command/context/TriggerDrumNoteOnContext.hpp"
#include "command/context/TriggerDrumNoteOffContext.hpp"

#include <memory>

namespace mpc
{
    class Mpc;
}

namespace mpc::sequencer
{
    class Track;
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
            mpc::Mpc &mpc, const int programPadIndex, const int velocity,
            const std::shared_ptr<mpc::lcdgui::ScreenComponent>);

        static TriggerDrumNoteOffContext buildTriggerDrumNoteOffContext(
            mpc::Mpc &mpc, const int programPadIndex, int drumIndex,
            const std::shared_ptr<mpc::lcdgui::ScreenComponent>, const int note,
            std::shared_ptr<sampler::Program>,
            std::shared_ptr<sequencer::Track>);
    };
} // namespace mpc::command::context
