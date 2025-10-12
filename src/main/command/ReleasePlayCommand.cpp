#include "ReleasePlayCommand.h"
#include "Mpc.hpp"
#include "controls/Controls.hpp"

using namespace mpc::command;

ReleasePlayCommand::ReleasePlayCommand(mpc::Mpc& mpc) : mpc(mpc) {}

void ReleasePlayCommand::execute() {
    const auto controls = mpc.getControls();
    controls->setPlayPressed(false);
}
