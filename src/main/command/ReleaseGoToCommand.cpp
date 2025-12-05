#include "command/ReleaseGoToCommand.hpp"
#include "Mpc.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/Sequencer.hpp"

using namespace mpc::command;
using namespace mpc::lcdgui;

ReleaseGoToCommand::ReleaseGoToCommand(Mpc &mpc) : mpc(mpc) {}

void ReleaseGoToCommand::execute()
{
    if (!mpc.getSequencer()->getSelectedSequence()->isUsed())
    {
        return;
    }

    const auto ls = mpc.getLayeredScreen();

    if (ls->isCurrentScreen({ScreenId::LocateScreen}))
    {
        ls->closeCurrentScreen();
        return;
    }

    if (ls->isCurrentScreenOrChildOf(ScreenId::SequencerScreen))
    {
        mpc.getLayeredScreen()->openScreenById(ScreenId::LocateScreen);
    }
}
