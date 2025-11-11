#pragma once
#include "command/Command.hpp"

namespace mpc::command
{

    class PushLeftCommand : public Command
    {
    public:
        explicit PushLeftCommand(Mpc &mpc);
        void execute() override;

    private:
        Mpc &mpc;
    };

} // namespace mpc::command