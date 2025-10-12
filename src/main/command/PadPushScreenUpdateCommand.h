#pragma once
#include "command/Command.h"

#include <optional>

namespace mpc::controls {
struct PadPushScreenUpdateContext;
}

namespace mpc::command {

    class PadPushScreenUpdateCommand : public Command {
    public:
        PadPushScreenUpdateCommand(controls::PadPushScreenUpdateContext &ctx, const int note, const std::optional<int> padIndexWithBank);
        void execute() override;

    private:
        mpc::controls::PadPushScreenUpdateContext &ctx;
        const int note;
        const std::optional<int> padIndexWithBank;
    };

} // namespace mpc::command

