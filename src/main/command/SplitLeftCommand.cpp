#include "SplitLeftCommand.hpp"
#include "Mpc.hpp"

#include "PushLeftCommand.hpp"
#include "hardware/Hardware.hpp"
#include "lcdgui/Field.hpp"
#include "lcdgui/LcdGuiUtil.hpp"

namespace mpc::command {

    SplitLeftCommand::SplitLeftCommand(mpc::Mpc &mpc) : mpc(mpc) {}

    void SplitLeftCommand::execute()
    {
        if (!mpc.getHardware()->getButton(hardware::ComponentId::SHIFT)->isPressed())
        {
            PushLeftCommand(mpc).execute();
            return;
        }

        if (!lcdgui::util::isFieldSplittable(mpc.getLayeredScreen()->getCurrentScreen(), mpc.getLayeredScreen()->getFocusedFieldName()))
            return;

        const auto field = mpc.getLayeredScreen()->getFocusedField();
        if (field->isSplit()) {
            field->setActiveSplit(field->getActiveSplit() - 1);
        } else {
            field->setSplit(true);
        }
    }

}
