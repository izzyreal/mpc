#pragma once
#include "IntTypes.hpp"
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
        PushPadScreenUpdateCommand(
            context::PushPadScreenUpdateContext &ctxToUse,
            int padIndexWithBankToUse);
        void execute() override;

    private:
        context::PushPadScreenUpdateContext &ctx;
        const ProgramPadIndex padIndexWithBank;
    };

} // namespace mpc::command
