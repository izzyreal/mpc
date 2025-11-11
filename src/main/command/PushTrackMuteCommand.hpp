#pragma once
#include "command/Command.hpp"

namespace mpc::command
{

    class PushTrackMuteCommand : public Command
    {
    public:
        explicit PushTrackMuteCommand(Mpc &mpc);
        void execute() override;

    private:
        Mpc &mpc;
    };

} // namespace mpc::command
