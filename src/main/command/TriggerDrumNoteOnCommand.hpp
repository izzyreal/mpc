#pragma once

#include "command/Command.hpp"

#include <memory>

namespace mpc::command::context
{
    struct TriggerDrumNoteOnContext;
}

namespace mpc::command
{
    class TriggerDrumNoteOnCommand : public Command
    {
    public:
        explicit TriggerDrumNoteOnCommand(
            std::shared_ptr<context::TriggerDrumNoteOnContext>);
        void execute() override;

    private:
        std::shared_ptr<context::TriggerDrumNoteOnContext> ctx;
    };

} // namespace mpc::command
