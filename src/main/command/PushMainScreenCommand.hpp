#pragma once
#include "command/Command.hpp"

namespace mpc::command
{

    class PushMainScreenCommand : public Command
    {
    public:
        explicit PushMainScreenCommand(Mpc &mpc);
        void execute() override;

    private:
        Mpc &mpc;
    };

} // namespace mpc::command
