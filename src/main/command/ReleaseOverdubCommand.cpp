#include "ReleaseOverdubCommand.h"
#include "Mpc.hpp"
#include "hardware2/Hardware2.h"
#include "sequencer/Sequencer.hpp"

using namespace mpc::command;

ReleaseOverdubCommand::ReleaseOverdubCommand(mpc::Mpc& mpc) : mpc(mpc) {}

void ReleaseOverdubCommand::execute() {
    const auto controls = mpc.getControls();
    controls->setOverdubPressed(false);
    mpc.getHardware2()->getLed("overdub")->setEnabled(mpc.getSequencer()->isOverdubbing());
}
