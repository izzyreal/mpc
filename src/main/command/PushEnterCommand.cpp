#include "PushEnterCommand.h"
#include "Mpc.hpp"
#include "hardware/Hardware.h"

namespace mpc::command {

    PushEnterCommand::PushEnterCommand(mpc::Mpc &mpc) : mpc(mpc) {}

    void PushEnterCommand::execute() {
        
        if (mpc.getHardware()->getButton("shift")->isPressed())
        {
            mpc.getLayeredScreen()->openScreen("save");
        }
    }

} // namespace mpc::command
