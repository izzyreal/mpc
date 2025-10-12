#include "PushPressEnterCommand.h"
#include "Mpc.hpp"
#include "controls/Controls.hpp"

namespace mpc::command {

    PushPressEnterCommand::PushPressEnterCommand(mpc::Mpc &mpc) : mpc(mpc) {}

    void PushPressEnterCommand::execute() {
        auto controls = mpc.getControls();
        
        if (controls->isShiftPressed())
        {
            mpc.getLayeredScreen()->openScreen("save");
        }
    }

} // namespace mpc::command
