#include "ShiftCommand.h"
#include "Mpc.hpp"
#include "controls/Controls.hpp"
#include "lcdgui/Field.hpp"

namespace mpc::command {

    ShiftCommand::ShiftCommand(mpc::Mpc &mpc) : mpc(mpc) {}

    void ShiftCommand::execute() {
        const auto controls = mpc.getControls();
        if (controls->isShiftPressed()) return;

        controls->setShiftPressed(true);

        auto field = mpc.getLayeredScreen()->getFocusedField();
        if (!field || !field->isTypeModeEnabled()) return;

        field->disableTypeMode();

        const auto split = field->getActiveSplit();
        if (split != -1) {
            field->setSplit(true);
            field->setActiveSplit(split);
        }
    }

}
