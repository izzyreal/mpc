#pragma once
#include "command/Command.hpp"

namespace mpc::command
{

    class PushPlayStartCommand : public Command
    {
    public:
        explicit PushPlayStartCommand(Mpc &mpc);
        void execute() override;

    private:
        Mpc &mpc;
    };

} // namespace mpc::command
