#pragma once
#include "command/Command.hpp"

namespace mpc::command
{

    class PushNextSeqCommand : public Command
    {
    public:
        explicit PushNextSeqCommand(Mpc &mpc);
        void execute() override;

    private:
        Mpc &mpc;
    };

} // namespace mpc::command
