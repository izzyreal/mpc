#include "ReleaseRecCommand.h"
#include "Mpc.hpp"
#include "controls/Controls.hpp"
#include "hardware2/Hardware2.h"
#include "sequencer/Sequencer.hpp"

using namespace mpc::command;

ReleaseRecCommand::ReleaseRecCommand(mpc::Mpc& mpc) : mpc(mpc) {}

void ReleaseRecCommand::execute() {
    const auto controls = mpc.getControls();
    controls->setRecPressed(false);
    mpc.getHardware2()->getLed("rec")->setEnabled(mpc.getSequencer()->isRecording());
}
