#include "PushAfterCommand.h"
#include "Mpc.hpp"
#include "hardware/Hardware.h"

namespace mpc::command {

    PushAfterCommand::PushAfterCommand(mpc::Mpc &mpc) : mpc(mpc) {}

    void PushAfterCommand::execute() {
        if (mpc.getHardware()->getButton("shift")->isPressed())
        {
            mpc.getLayeredScreen()->openScreen("assign");
        } else {
            mpc.setAfterEnabled(!mpc.isAfterEnabled());
            mpc.getHardware()->getLed("after")->setEnabled(mpc.isAfterEnabled());
        }
    }

}
