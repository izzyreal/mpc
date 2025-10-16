#include "ReleaseOverdubCommand.h"
#include "Mpc.hpp"
#include "hardware/Hardware.h"
#include "sequencer/Sequencer.hpp"

using namespace mpc::command;

ReleaseOverdubCommand::ReleaseOverdubCommand(mpc::Mpc& mpc) : mpc(mpc) {}

void ReleaseOverdubCommand::execute()
{
    mpc.getHardware()->getLed(hardware::ComponentId::OVERDUB_LED)->setEnabled(mpc.getSequencer()->isOverdubbing());
}

