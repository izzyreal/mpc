#pragma once
#include "command/Command.hpp"

namespace mpc::command
{

    class PushEraseCommand : public Command
    {
    public:
        explicit PushEraseCommand(mpc::Mpc &mpc);
        void execute() override;

    private:
        mpc::Mpc &mpc;
    };

} // namespace mpc::command
