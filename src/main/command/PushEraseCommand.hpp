#pragma once
#include "command/Command.hpp"

namespace mpc::command
{

    class PushEraseCommand : public Command
    {
    public:
        explicit PushEraseCommand(Mpc &mpc);
        void execute() override;

    private:
        Mpc &mpc;
    };

} // namespace mpc::command
