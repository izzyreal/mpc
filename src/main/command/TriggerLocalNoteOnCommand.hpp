#pragma once

#include "command/Command.hpp"

#include <memory>

namespace mpc::command::context
{
    struct TriggerLocalNoteOnContext;
}

namespace mpc::command
{
    class TriggerLocalNoteOnCommand : public Command
    {
    public:
        explicit TriggerLocalNoteOnCommand(
            std::shared_ptr<context::TriggerLocalNoteOnContext>);
        void execute() override;

    private:
        std::shared_ptr<context::TriggerLocalNoteOnContext> ctx;
    };

} // namespace mpc::command
