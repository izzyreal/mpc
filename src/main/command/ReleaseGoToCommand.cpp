#include "ReleaseGoToCommand.h"
#include "Mpc.hpp"
#include "controls/Controls.hpp"

using namespace mpc::command;

ReleaseGoToCommand::ReleaseGoToCommand(mpc::Mpc& mpc) : mpc(mpc) {}

void ReleaseGoToCommand::execute() {
    const auto controls = mpc.getControls();
    controls->setGoToPressed(false);
}
