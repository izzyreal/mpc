#pragma once
#include "command/Command.hpp"

namespace mpc::command {

    class PushUpCommand : public Command {
    public:
        explicit PushUpCommand(mpc::Mpc &mpc);
        void execute() override;

    private:
        mpc::Mpc &mpc;
    };

} // namespace mpc::command
