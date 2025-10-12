#include "SixteenLevelsCommand.h"
#include "Mpc.hpp"
#include "hardware2/Hardware2.h"

namespace mpc::command {

    SixteenLevelsCommand::SixteenLevelsCommand(mpc::Mpc &mpc) : mpc(mpc) {}

    void SixteenLevelsCommand::execute() {
        const auto currentScreenName = mpc.getLayeredScreen()->getCurrentScreenName();
        if (currentScreenName != "sequencer" && currentScreenName != "assign-16-levels") return;

        if (mpc.isSixteenLevelsEnabled()) {
            mpc.setSixteenLevelsEnabled(false);
            mpc.getHardware2()->getLed("sixteen-levels")->setEnabled(false);
        } else {
            mpc.getLayeredScreen()->openScreen("assign-16-levels");
        }
    }

}
