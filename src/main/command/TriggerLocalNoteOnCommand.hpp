#pragma once

#include "command/Command.hpp"

namespace mpc::command::context
{
    struct TriggerLocalNoteOnContext;
}

namespace mpc::command
{
    class TriggerLocalNoteOnCommand final : public Command
    {
    public:
        explicit TriggerLocalNoteOnCommand(
            const context::TriggerLocalNoteOnContext &);
        void execute() override;

    private:
        const context::TriggerLocalNoteOnContext &ctx;
    };

} // namespace mpc::command
