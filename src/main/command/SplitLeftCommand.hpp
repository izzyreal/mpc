#pragma once
#include "command/Command.hpp"

namespace mpc::command
{

    class SplitLeftCommand : public Command
    {
    public:
        explicit SplitLeftCommand(Mpc &mpc);
        void execute() override;

    private:
        Mpc &mpc;
    };

} // namespace mpc::command
