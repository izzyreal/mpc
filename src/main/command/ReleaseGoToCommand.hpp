#pragma once
#include "command/Command.hpp"

namespace mpc::command
{
    class ReleaseGoToCommand final : public Command
    {
    public:
        explicit ReleaseGoToCommand(Mpc &);
        void execute() override;

    private:
        Mpc &mpc;
    };

} // namespace mpc::command
