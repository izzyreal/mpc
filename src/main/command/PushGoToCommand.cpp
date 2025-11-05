#include "PushGoToCommand.hpp"
#include "Mpc.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/Sequencer.hpp"

using namespace mpc::command;
using namespace mpc::lcdgui;

PushGoToCommand::PushGoToCommand(mpc::Mpc &mpc) : mpc(mpc) {}

void PushGoToCommand::execute()
{
    if (!mpc.getSequencer()->getActiveSequence()->isUsed())
    {
        return;
    }

    if (mpc.getLayeredScreen()->getCurrentScreenName() == "sequencer")
    {
        mpc.getLayeredScreen()->openScreenById(ScreenId::LocateScreen);
    }
}
