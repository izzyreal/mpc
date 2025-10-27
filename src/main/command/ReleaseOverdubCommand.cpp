#include "ReleaseOverdubCommand.hpp"
#include "Mpc.hpp"
#include "hardware/Hardware.hpp"
#include "sequencer/Sequencer.hpp"

using namespace mpc::command;

ReleaseOverdubCommand::ReleaseOverdubCommand(mpc::Mpc &mpc) : mpc(mpc) {}

void ReleaseOverdubCommand::execute()
{
    mpc.getHardware()
        ->getLed(hardware::ComponentId::OVERDUB_LED)
        ->setEnabled(mpc.getSequencer()->isOverdubbing());
}
