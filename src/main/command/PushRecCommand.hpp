#pragma once
#include "command/Command.hpp"

namespace mpc::command
{

    class PushRecCommand : public Command
    {
    public:
        explicit PushRecCommand(Mpc &mpc);
        void execute() override;

    private:
        Mpc &mpc;
    };

} // namespace mpc::command
