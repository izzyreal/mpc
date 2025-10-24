#pragma once
#include "command/Command.hpp"

namespace mpc::command::context
{
    struct PushPadScreenUpdateContext;
}

namespace mpc::command
{

    class PushPadScreenUpdateCommand : public Command
    {
    public:
        PushPadScreenUpdateCommand(context::PushPadScreenUpdateContext &ctx, int padIndexWithBank);
        void execute() override;

    private:
        context::PushPadScreenUpdateContext &ctx;
        const int padIndexWithBank;
    };

} // namespace mpc::command
