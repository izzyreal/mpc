#include "ReleaseEraseCommand.h"
#include "Mpc.hpp"
#include "lcdgui/screens/SequencerScreen.hpp"

using namespace mpc::command;

ReleaseEraseCommand::ReleaseEraseCommand(mpc::Mpc& mpc) : mpc(mpc) {}

void ReleaseEraseCommand::execute()
{
    const auto sequencerScreen = mpc.screens->get<lcdgui::screens::SequencerScreen>();
    sequencerScreen->hideFooterLabelAndShowFunctionKeys();
}

