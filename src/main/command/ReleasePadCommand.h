#pragma once

#include "Command.h"

namespace mpc::controls {
    class PadReleaseContext;
}

namespace mpc::command {
    class ReleasePadCommand : public Command {
        controls::PadReleaseContext& ctx;
    public:
        explicit ReleasePadCommand(controls::PadReleaseContext& ctx);
        void execute() override;
    };
}
