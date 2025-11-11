#pragma once
#include "command/Command.hpp"

namespace mpc::command
{

    class PushOverdubCommand : public Command
    {
    public:
        explicit PushOverdubCommand(Mpc &mpc);
        void execute() override;

    private:
        Mpc &mpc;
    };

} // namespace mpc::command
