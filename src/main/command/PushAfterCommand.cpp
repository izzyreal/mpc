#include "PushAfterCommand.h"
#include "Mpc.hpp"
#include "controls/Controls.hpp"
#include "hardware2/Hardware2.h"

namespace mpc::command {

    PushAfterCommand::PushAfterCommand(mpc::Mpc &mpc) : mpc(mpc) {}

    void PushAfterCommand::execute() {
        const auto controls = mpc.getControls();
        if (controls->isShiftPressed()) {
            mpc.getLayeredScreen()->openScreen("assign");
        } else {
            mpc.setAfterEnabled(!mpc.isAfterEnabled());
            mpc.getHardware2()->getLed("after")->setEnabled(mpc.isAfterEnabled());
        }
    }

}
