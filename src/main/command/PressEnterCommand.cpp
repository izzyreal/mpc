#include "PressEnterCommand.h"
#include "Mpc.hpp"
#include "controls/Controls.hpp"

namespace mpc::command {

    PressEnterCommand::PressEnterCommand(mpc::Mpc &mpc) : mpc(mpc) {}

    void PressEnterCommand::execute() {
        auto controls = mpc.getControls();
        
        if (controls->isShiftPressed())
        {
            mpc.getLayeredScreen()->openScreen("save");
        }
    }

} // namespace mpc::command
