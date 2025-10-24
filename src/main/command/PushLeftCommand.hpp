#pragma once
#include "command/Command.hpp"

namespace mpc::command
{

    class PushLeftCommand : public Command
    {
    public:
        explicit PushLeftCommand(mpc::Mpc &mpc);
        void execute() override;

    private:
        mpc::Mpc &mpc;
    };

} // namespace mpc::command