#include "SplitRightCommand.h"
#include "Mpc.hpp"
#include "command/PushRightCommand.h"
#include "hardware2/Hardware2.h"
#include "lcdgui/Field.hpp"
#include "lcdgui/LcdGuiUtil.h"

namespace mpc::command {

    SplitRightCommand::SplitRightCommand(mpc::Mpc &mpc) : mpc(mpc) {}

    void SplitRightCommand::execute() {

        if (!mpc.getHardware2()->getButton("shift")->isPressed())
        {
            PushRightCommand(mpc).execute();
            return;
        }

        const auto field = mpc.getLayeredScreen()->getFocusedField();

        if (lcdgui::util::isFieldSplittable(mpc.getLayeredScreen()->getCurrentScreenName(), mpc.getLayeredScreen()->getFocus()) && field->isSplit())
        {
            if (!field->setActiveSplit(field->getActiveSplit() + 1))
                field->setSplit(false);
        }
    }
}

