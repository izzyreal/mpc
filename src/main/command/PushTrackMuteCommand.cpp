#include "PushTrackMuteCommand.hpp"
#include "sequencer/Transport.hpp"
#include "Mpc.hpp"
#include "Util.hpp"
#include "hardware/Hardware.hpp"

#include "lcdgui/screens/NextSeqScreen.hpp"
#include "lcdgui/screens/NextSeqPadScreen.hpp"
#include "lcdgui/screens/SequencerScreen.hpp"
#include "lcdgui/screens/TrMuteScreen.hpp"
#include "sequencer/Sequencer.hpp"

using namespace mpc::command;
using namespace mpc::lcdgui;

PushTrackMuteCommand::PushTrackMuteCommand(Mpc &mpc) : mpc(mpc) {}

void PushTrackMuteCommand::execute()
{
    const auto ls = mpc.getLayeredScreen();

    if (ls->isCurrentScreen({ScreenId::TrMuteScreen}))
    {
        if (ls->isPreviousScreen(
                {ScreenId::NextSeqScreen, ScreenId::NextSeqPadScreen}))
        {
            ls->openScreenById(ScreenId::NextSeqScreen);
        }
        else
        {
            ls->openScreenById(ScreenId::SequencerScreen);
        }

        mpc.getHardware()
            ->getLed(hardware::ComponentId::TRACK_MUTE_LED)
            ->setEnabled(false);
    }
    else if (ls->isCurrentScreen({ScreenId::NextSeqScreen,
                                  ScreenId::NextSeqPadScreen,
                                  ScreenId::SequencerScreen}) &&
             !mpc.getSequencer()->getTransport()->isRecordingOrOverdubbing())
    {
        Util::initSequence(mpc);
        mpc.getLayeredScreen()->openScreenById(ScreenId::TrMuteScreen);
        mpc.getHardware()
            ->getLed(hardware::ComponentId::TRACK_MUTE_LED)
            ->setEnabled(true);
    }
}
