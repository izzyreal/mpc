#include "PushSixteenLevelsCommand.h"
#include "Mpc.hpp"
#include "hardware/Hardware.h"

namespace mpc::command {

    PushSixteenLevelsCommand::PushSixteenLevelsCommand(mpc::Mpc &mpc) : mpc(mpc) {}

    void PushSixteenLevelsCommand::execute() {
        const auto currentScreenName = mpc.getLayeredScreen()->getCurrentScreenName();
        if (currentScreenName != "sequencer" && currentScreenName != "assign-16-levels") return;

        if (mpc.isSixteenLevelsEnabled()) {
            mpc.setSixteenLevelsEnabled(false);
            mpc.getHardware()->getLed(hardware::ComponentId::SIXTEEN_LEVELS_OR_SPACE_LED)->setEnabled(false);
        } else {
            mpc.getLayeredScreen()->openScreen<Assign16LevelsScreen>();
        }
    }

}
