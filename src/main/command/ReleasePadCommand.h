#pragma once

#include "Command.h"

namespace mpc::controls {
    class PadReleaseContext;
}

namespace mpc::command {
    // Should be renamed to ReleaseProgramPad or similar, because we're not dealing with
    // hardware pad indices from 1 to 16 here, but with the 64 pads of a program.
    class ReleasePadCommand : public Command {
        controls::PadReleaseContext& ctx;
    public:
        explicit ReleasePadCommand(controls::PadReleaseContext& ctx);
        void execute() override;
    };
}
