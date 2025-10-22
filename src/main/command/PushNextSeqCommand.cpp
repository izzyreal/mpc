#include "PushNextSeqCommand.hpp"
#include "Mpc.hpp"
#include "hardware/Hardware.hpp"
#include "Util.hpp"

namespace mpc::command {

    PushNextSeqCommand::PushNextSeqCommand(mpc::Mpc &mpc) : mpc(mpc) {}

    void PushNextSeqCommand::execute()
    {        
        if (mpc.getLayeredScreen()->isCurrentScreen<NextSeqScreen, NextSeqPadScreen>())
        {
            mpc.getLayeredScreen()->openScreen<SequencerScreen>();
        }
        else if (mpc.getLayeredScreen()->isCurrentScreen<SequencerScreen, TrMuteScreen>())
        {
            mpc.getLayeredScreen()->openScreen<NextSeqScreen>();
        }
    }
}
