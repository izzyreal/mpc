#pragma once
#include "command/Command.h"

#include <optional>

namespace mpc::command::context {
    struct PushPadScreenUpdateContext;
}

namespace mpc::command {

    class PushPadScreenUpdateCommand : public Command {
    public:
        PushPadScreenUpdateCommand(context::PushPadScreenUpdateContext &ctx, const int note, const std::optional<int> padIndexWithBank);
        void execute() override;

    private:
        context::PushPadScreenUpdateContext &ctx;
        const int note;
        const std::optional<int> padIndexWithBank;
    };

} // namespace mpc::command

