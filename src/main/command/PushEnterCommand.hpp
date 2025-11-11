#pragma once
#include "command/Command.hpp"

namespace mpc::command
{

    class PushEnterCommand : public Command
    {
    public:
        explicit PushEnterCommand(Mpc &mpc);
        void execute() override;

    private:
        Mpc &mpc;
    };

} // namespace mpc::command
