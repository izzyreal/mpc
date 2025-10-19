#include "PushEnterCommand.h"
#include "Mpc.hpp"
#include "hardware/Hardware.h"

namespace mpc::command {

    PushEnterCommand::PushEnterCommand(mpc::Mpc &mpc) : mpc(mpc) {}

    void PushEnterCommand::execute() {
        
        if (mpc.getHardware()->getButton(hardware::ComponentId::SHIFT)->isPressed())
        {
            mpc.getLayeredScreen()->openScreen<SaveScreen>();
        }
    }

} // namespace mpc::command
