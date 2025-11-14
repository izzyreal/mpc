#include "PushEraseCommand.hpp"
#include "sequencer/Transport.hpp"
#include "Mpc.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Sequence.hpp"

using namespace mpc::command;
using namespace mpc::lcdgui;

PushEraseCommand::PushEraseCommand(Mpc &mpc) : mpc(mpc) {}

void PushEraseCommand::execute()
{
    if (!mpc.getSequencer()->getSelectedSequence()->isUsed() ||
        mpc.getSequencer()->getTransport()->isPlaying())
    {
        return;
    }

    mpc.getLayeredScreen()->openScreenById(ScreenId::EraseScreen);
}
