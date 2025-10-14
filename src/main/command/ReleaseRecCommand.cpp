#include "ReleaseRecCommand.h"
#include "Mpc.hpp"
#include "hardware2/Hardware2.h"
#include "sequencer/Sequencer.hpp"

using namespace mpc::command;

ReleaseRecCommand::ReleaseRecCommand(mpc::Mpc& mpc) : mpc(mpc) {}

void ReleaseRecCommand::execute()
{
    mpc.getHardware2()->getLed("rec")->setEnabled(mpc.getSequencer()->isRecording());
}
