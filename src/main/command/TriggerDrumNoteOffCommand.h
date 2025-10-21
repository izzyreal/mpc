#pragma once

#include "Command.h"

namespace mpc::command::context {
    class TriggerDrumNoteOffContext;
}

namespace mpc::command {
    class TriggerDrumNoteOffCommand : public Command {
        context::TriggerDrumNoteOffContext& ctx;
    public:
        explicit TriggerDrumNoteOffCommand(context::TriggerDrumNoteOffContext& ctx);
        void execute() override;
    };
}
