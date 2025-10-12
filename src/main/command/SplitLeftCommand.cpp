#include "SplitLeftCommand.h"
#include "Mpc.hpp"
#include "controls/Controls.hpp"

#include "LeftCommand.h"
#include "lcdgui/Field.hpp"
#include "lcdgui/LcdGuiUtil.h"

namespace mpc::command {

    SplitLeftCommand::SplitLeftCommand(mpc::Mpc &mpc) : mpc(mpc) {}

    void SplitLeftCommand::execute() {
        const auto controls = mpc.getControls();
        if (!controls->isShiftPressed()) {
            LeftCommand(mpc).execute();
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
