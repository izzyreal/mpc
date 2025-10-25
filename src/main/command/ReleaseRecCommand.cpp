#include "ReleaseRecCommand.hpp"
#include "Mpc.hpp"
#include "hardware/Hardware.hpp"
#include "sequencer/Sequencer.hpp"

using namespace mpc::command;

ReleaseRecCommand::ReleaseRecCommand(mpc::Mpc &mpc) : mpc(mpc) {}

void ReleaseRecCommand::execute()
{
    mpc.getHardware()->getLed(hardware::ComponentId::REC_LED)->setEnabled(mpc.getSequencer()->isRecording());
    mpc.getHardware()->getLed(hardware::ComponentId::OVERDUB_LED)->setEnabled(mpc.getSequencer()->isOverdubbing());
}
