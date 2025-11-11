#pragma once
#include "command/Command.hpp"

namespace mpc::command
{

    class PushPlayCommand : public Command
    {
    public:
        explicit PushPlayCommand(Mpc &mpc);
        void execute() override;

    private:
        Mpc &mpc;
    };

} // namespace mpc::command
