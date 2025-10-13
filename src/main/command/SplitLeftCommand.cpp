#include "SplitLeftCommand.h"
#include "Mpc.hpp"

#include "PushLeftCommand.h"
#include "hardware2/Hardware2.h"
#include "lcdgui/Field.hpp"
#include "lcdgui/LcdGuiUtil.h"

namespace mpc::command {

    SplitLeftCommand::SplitLeftCommand(mpc::Mpc &mpc) : mpc(mpc) {}

    void SplitLeftCommand::execute()
    {
        if (!mpc.getHardware2()->getButton("shift")->isPressed())
        {
            PushLeftCommand(mpc).execute();
            return;
        }

        if (!lcdgui::util::isFieldSplittable(mpc.getLayeredScreen()->getCurrentScreenName(), mpc.getLayeredScreen()->getFocus()))
            return;

        const auto field = mpc.getLayeredScreen()->getFocusedField();
        if (field->isSplit()) {
            field->setActiveSplit(field->getActiveSplit() - 1);
        } else {
            field->setSplit(true);
        }
    }

}
