#pragma once
#include "command/Command.hpp"

namespace mpc::command
{

    class PushUndoSeqCommand : public Command
    {
    public:
        explicit PushUndoSeqCommand(mpc::Mpc &mpc);
        void execute() override;

    private:
        mpc::Mpc &mpc;
    };

} // namespace mpc::command
