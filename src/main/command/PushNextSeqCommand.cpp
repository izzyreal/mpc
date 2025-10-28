#include "PushNextSeqCommand.hpp"
#include "Mpc.hpp"

namespace mpc::command
{

    PushNextSeqCommand::PushNextSeqCommand(mpc::Mpc &mpc) : mpc(mpc) {}

    void PushNextSeqCommand::execute()
    {
        if (mpc.getLayeredScreen()
                ->isCurrentScreen<NextSeqScreen, NextSeqPadScreen>())
        {
            mpc.getLayeredScreen()->openScreen<SequencerScreen>();
        }
        else if (mpc.getLayeredScreen()
                     ->isCurrentScreen<SequencerScreen, TrMuteScreen>())
        {
            mpc.getLayeredScreen()->openScreen<NextSeqScreen>();
        }
    }
} // namespace mpc::command
