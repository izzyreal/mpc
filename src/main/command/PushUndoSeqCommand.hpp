#pragma once
#include "command/Command.hpp"

namespace mpc::command
{

    class PushUndoSeqCommand : public Command
    {
    public:
        explicit PushUndoSeqCommand(Mpc &mpc);
        void execute() override;

    private:
        Mpc &mpc;
    };

} // namespace mpc::command
