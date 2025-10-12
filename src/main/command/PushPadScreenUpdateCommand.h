#pragma once
#include "command/Command.h"

#include <optional>

namespace mpc::controls {
struct PushPadScreenUpdateContext;
}

namespace mpc::command {

    class PushPadScreenUpdateCommand : public Command {
    public:
        PushPadScreenUpdateCommand(controls::PushPadScreenUpdateContext &ctx, const int note, const std::optional<int> padIndexWithBank);
        void execute() override;

    private:
        mpc::controls::PushPadScreenUpdateContext &ctx;
        const int note;
        const std::optional<int> padIndexWithBank;
    };

} // namespace mpc::command

