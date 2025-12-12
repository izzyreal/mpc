#pragma once

#include "Command.hpp"

namespace mpc::command::context
{
    struct TriggerLocalNoteOffContext;
}

namespace mpc::command
{
    class TriggerLocalNoteOffCommand final : public Command
    {
        const context::TriggerLocalNoteOffContext &ctx;

    public:
        explicit TriggerLocalNoteOffCommand(
            const context::TriggerLocalNoteOffContext &);

        // Should be invoked from the audio thread only
        void execute() override;
    };
} // namespace mpc::command
