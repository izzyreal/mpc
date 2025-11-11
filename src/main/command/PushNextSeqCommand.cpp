#include "PushNextSeqCommand.hpp"
#include "Mpc.hpp"

using namespace mpc::command;
using namespace mpc::lcdgui;

PushNextSeqCommand::PushNextSeqCommand(Mpc &mpc) : mpc(mpc) {}

void PushNextSeqCommand::execute()
{
    if (mpc.getLayeredScreen()->isCurrentScreen(
            {ScreenId::NextSeqScreen, ScreenId::NextSeqPadScreen}))
    {
        mpc.getLayeredScreen()->openScreenById(ScreenId::SequencerScreen);
    }
    else if (mpc.getLayeredScreen()->isCurrentScreen(
                 {ScreenId::SequencerScreen, ScreenId::TrMuteScreen}))
    {
        mpc.getLayeredScreen()->openScreenById(ScreenId::NextSeqScreen);
    }
}
