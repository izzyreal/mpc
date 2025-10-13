#include "PushShiftCommand.h"
#include "Mpc.hpp"
#include "controls/Controls.hpp"
#include "lcdgui/Field.hpp"

namespace mpc::command {

    PushShiftCommand::PushShiftCommand(mpc::Mpc &mpc) : mpc(mpc) {}

    void PushShiftCommand::execute() {
        auto field = mpc.getLayeredScreen()->getFocusedField();

        if (!field || !field->isTypeModeEnabled())
        {
            return;
        }

        field->disableTypeMode();

        const auto split = field->getActiveSplit();

        if (split != -1) {
            field->setSplit(true);
            field->setActiveSplit(split);
        }
    }

}
