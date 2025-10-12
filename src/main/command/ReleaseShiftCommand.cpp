#include "ReleaseShiftCommand.h"
#include "Mpc.hpp"
#include "controls/Controls.hpp"

using namespace mpc::command;

ReleaseShiftCommand::ReleaseShiftCommand(mpc::Mpc& mpc) : mpc(mpc) {}

void ReleaseShiftCommand::execute() {
    const auto controls = mpc.getControls();
    controls->setShiftPressed(false);
}
