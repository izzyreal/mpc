#include "PushNextSeqCommand.h"
#include "Mpc.hpp"
#include "hardware/Hardware.h"
#include "Util.hpp"

namespace mpc::command {

    PushNextSeqCommand::PushNextSeqCommand(mpc::Mpc &mpc) : mpc(mpc) {}

    void PushNextSeqCommand::execute()
    {        
        if (mpc.getLayeredScreen()->isCurrentScreen<NextSeqScreen, NextSeqScreen>())
        {
            mpc.getLayeredScreen()->openScreen<SequencerScreen>();
        }
        else if (mpc.getLayeredScreen()->isCurrentScreen<SequencerScreen, TrMuteScreen>())
        {
            mpc.getLayeredScreen()->openScreen<NextSeqScreen>();
        }
    }
}
