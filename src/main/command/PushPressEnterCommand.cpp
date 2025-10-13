#include "PushPressEnterCommand.h"
#include "Mpc.hpp"
#include "controls/Controls.hpp"
#include "hardware2/Hardware2.h"

namespace mpc::command {

    PushPressEnterCommand::PushPressEnterCommand(mpc::Mpc &mpc) : mpc(mpc) {}

    void PushPressEnterCommand::execute() {
        
        if (mpc.getHardware2()->getButton("shift")->isPressed())
        {
            mpc.getLayeredScreen()->openScreen("save");
        }
    }

} // namespace mpc::command
