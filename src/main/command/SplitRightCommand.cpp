#include "SplitRightCommand.h"
#include "Mpc.hpp"
#include "controls/Controls.hpp"
#include "command/PushRightCommand.h"
#include "lcdgui/Field.hpp"
#include "lcdgui/LcdGuiUtil.h"

namespace mpc::command {

    SplitRightCommand::SplitRightCommand(mpc::Mpc &mpc) : mpc(mpc) {}

    void SplitRightCommand::execute() {
        const auto controls = mpc.getControls();
        if (!controls->isShiftPressed()) {
            PushRightCommand(mpc).execute();
            return;
        }

        const auto field = mpc.getLayeredScreen()->getFocusedField();
        if (lcdgui::util::isFieldSplittable(mpc.getLayeredScreen()->getCurrentScreenName(), mpc.getLayeredScreen()->getFocus()) && field->isSplit()) {
            if (!field->setActiveSplit(field->getActiveSplit() + 1))
                field->setSplit(false);
        }
    }
}

