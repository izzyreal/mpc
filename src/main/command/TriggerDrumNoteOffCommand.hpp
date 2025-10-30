#pragma once

#include "Command.hpp"

namespace mpc::command::context
{
    class TriggerDrumNoteOffContext;
}

namespace mpc::command
{
    class TriggerDrumNoteOffCommand : public Command
    {
    private:
        context::TriggerDrumNoteOffContext &ctx;

    public:
        explicit TriggerDrumNoteOffCommand(
            context::TriggerDrumNoteOffContext &ctx);

        // Should be invoked from the audio thread only
        void execute() override;
    };
} // namespace mpc::command
