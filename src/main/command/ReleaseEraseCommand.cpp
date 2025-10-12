#include "ReleaseEraseCommand.h"
#include "Mpc.hpp"
#include "controls/Controls.hpp"
#include "lcdgui/screens/SequencerScreen.hpp"

using namespace mpc::command;

ReleaseEraseCommand::ReleaseEraseCommand(mpc::Mpc& mpc) : mpc(mpc) {}

void ReleaseEraseCommand::execute() {
    const auto controls = mpc.getControls();
    controls->setErasePressed(false);
    const auto sequencerScreen = mpc.screens->get<lcdgui::screens::SequencerScreen>("sequencer");
    sequencerScreen->releaseErase();
}
