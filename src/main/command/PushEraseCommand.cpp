#include "PushEraseCommand.hpp"
#include "Mpc.hpp"
#include "sequencer/Sequencer.hpp"

using namespace mpc::command;
using namespace mpc::lcdgui;

PushEraseCommand::PushEraseCommand(mpc::Mpc &mpc) : mpc(mpc) {}

void PushEraseCommand::execute()
{
    if (!mpc.getSequencer()->getActiveSequence()->isUsed() ||
        mpc.getSequencer()->isPlaying())
    {
        return;
    }

    mpc.getLayeredScreen()->openScreenById(ScreenId::EraseScreen);
}
