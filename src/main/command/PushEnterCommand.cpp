#include "PushEnterCommand.h"
#include "Mpc.hpp"
#include "hardware2/Hardware2.h"

namespace mpc::command {

    PushEnterCommand::PushEnterCommand(mpc::Mpc &mpc) : mpc(mpc) {}

    void PushEnterCommand::execute() {
        
        if (mpc.getHardware2()->getButton("shift")->isPressed())
        {
            mpc.getLayeredScreen()->openScreen("save");
        }
    }

} // namespace mpc::command
