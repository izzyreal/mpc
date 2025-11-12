#pragma once

#include "Command.hpp"

#include <memory>

namespace mpc::command::context
{
    struct TriggerLocalNoteOffContext;
}

namespace mpc::command
{
    class TriggerLocalNoteOffCommand : public Command
    {
        std::shared_ptr<context::TriggerLocalNoteOffContext> ctx;

    public:
        explicit TriggerLocalNoteOffCommand(
            const std::shared_ptr<context::TriggerLocalNoteOffContext> &);

        // Should be invoked from the audio thread only
        void execute() override;
    };
} // namespace mpc::command
