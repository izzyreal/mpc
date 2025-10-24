#pragma once
#include "command/Command.hpp"

namespace mpc::command
{

    class PushFullLevelCommand : public Command
    {
    public:
        explicit PushFullLevelCommand(mpc::Mpc &mpc);
        void execute() override;

    private:
        mpc::Mpc &mpc;
    };

} // namespace mpc::command
