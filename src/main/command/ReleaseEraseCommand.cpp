#include "ReleaseEraseCommand.hpp"
#include "Mpc.hpp"
#include "lcdgui/Screens.hpp"
#include "lcdgui/screens/SequencerScreen.hpp"

using namespace mpc::command;
using namespace mpc::lcdgui;

ReleaseEraseCommand::ReleaseEraseCommand(Mpc &mpc) : mpc(mpc) {}

void ReleaseEraseCommand::execute()
{
    const auto sequencerScreen = mpc.screens->get<ScreenId::SequencerScreen>();
    sequencerScreen->hideHoldHintAndRestoreFooterWidgets();
}
