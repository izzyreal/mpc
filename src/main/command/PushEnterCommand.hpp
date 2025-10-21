#pragma once
#include "command/Command.hpp"

namespace mpc::command {

    class PushEnterCommand : public Command {
    public:
        explicit PushEnterCommand(mpc::Mpc &mpc);
        void execute() override;

    private:
        mpc::Mpc &mpc;
    };

} // namespace mpc::command
