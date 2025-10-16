#include "PushAfterCommand.h"
#include "Mpc.hpp"
#include "hardware/Hardware.h"

namespace mpc::command {

    PushAfterCommand::PushAfterCommand(mpc::Mpc &mpc) : mpc(mpc) {}

    void PushAfterCommand::execute() {
        if (mpc.getHardware()->getButton(hardware::ComponentId::SHIFT)->isPressed())
        {
            mpc.getLayeredScreen()->openScreen("assign");
        } else {
            mpc.setAfterEnabled(!mpc.isAfterEnabled());
            mpc.getHardware()->getLed(hardware::ComponentId::AFTER_OR_ASSIGN_LED)->setEnabled(mpc.isAfterEnabled());
        }
    }

}
