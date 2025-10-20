#include "PushTrackMuteCommand.h"
#include "Mpc.hpp"
#include "Util.hpp"
#include "hardware/Hardware.h"

#include "lcdgui/screens/NextSeqScreen.hpp"
#include "lcdgui/screens/NextSeqPadScreen.hpp"
#include "lcdgui/screens/SequencerScreen.hpp"
#include "lcdgui/screens/TrMuteScreen.hpp"

namespace mpc::command {

    PushTrackMuteCommand::PushTrackMuteCommand(mpc::Mpc &mpc) : mpc(mpc) {}

    void PushTrackMuteCommand::execute()
    {
        const auto ls = mpc.getLayeredScreen();

        if (ls->isCurrentScreen<TrMuteScreen>())
        {
            if (ls->isPreviousScreen<NextSeqScreen, NextSeqPadScreen>())
            {
                ls->openScreen<NextSeqScreen>();
            }
            else
            {
                ls->openScreen<SequencerScreen>();
            }
            
            mpc.getHardware()->getLed(hardware::ComponentId::TRACK_MUTE_LED)->setEnabled(false);
        }
        else if (ls->isCurrentScreen<NextSeqScreen, NextSeqPadScreen, SequencerScreen>())
        {
            Util::initSequence(mpc);
            mpc.getLayeredScreen()->openScreen<TrMuteScreen>();
            mpc.getHardware()->getLed(hardware::ComponentId::TRACK_MUTE_LED)->setEnabled(true);
        }
    }
}

