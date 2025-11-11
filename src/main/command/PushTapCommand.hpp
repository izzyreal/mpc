#pragma once
#include "command/Command.hpp"

namespace mpc::command
{

    class PushTapCommand : public Command
    {
    public:
        explicit PushTapCommand(Mpc &mpc);
        void execute() override;

    private:
        Mpc &mpc;
    };

} // namespace mpc::command
