#pragma once
#include "command/Command.hpp"

namespace mpc::command
{

    class PushPlayStartCommand : public Command
    {
    public:
        explicit PushPlayStartCommand(mpc::Mpc &mpc);
        void execute() override;

    private:
        mpc::Mpc &mpc;
    };

} // namespace mpc::command
