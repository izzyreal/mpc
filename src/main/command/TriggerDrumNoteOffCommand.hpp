#pragma once

#include "Command.hpp"

#include <memory>

namespace mpc::command::context
{
    class TriggerDrumNoteOffContext;
}

namespace mpc::command
{
    class TriggerDrumNoteOffCommand : public Command
    {
    private:
        std::shared_ptr<context::TriggerDrumNoteOffContext> ctx;

    public:
        explicit TriggerDrumNoteOffCommand(
            std::shared_ptr<context::TriggerDrumNoteOffContext>);

        // Should be invoked from the audio thread only
        void execute() override;
    };
} // namespace mpc::command
