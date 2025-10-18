#pragma once

#include "Command.h"

namespace mpc::command::context {
    class TriggerDrumNoteOffContext;
}

namespace mpc::command {
    // Should be renamed to ReleaseProgramPad or similar, because we're not dealing with
    // hardware pad indices from 1 to 16 here, but with the 64 pads of a program.
    class TriggerDrumNoteOffCommand : public Command {
        context::TriggerDrumNoteOffContext& ctx;
    public:
        explicit TriggerDrumNoteOffCommand(context::TriggerDrumNoteOffContext& ctx);
        void execute() override;
    };
}
