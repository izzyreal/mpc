#include "SplitRightCommand.hpp"
#include "Mpc.hpp"
#include "command/PushRightCommand.hpp"
#include "hardware/Hardware.hpp"
#include "lcdgui/Field.hpp"
#include "lcdgui/LcdGuiUtil.hpp"

namespace mpc::command
{

    SplitRightCommand::SplitRightCommand(mpc::Mpc &mpc) : mpc(mpc) {}

    void SplitRightCommand::execute()
    {

        if (!mpc.getHardware()
                 ->getButton(hardware::ComponentId::SHIFT)
                 ->isPressed())
        {
            PushRightCommand(mpc).execute();
            return;
        }

        const auto field = mpc.getLayeredScreen()->getFocusedField();

        if (lcdgui::util::isFieldSplittable(
                mpc.getLayeredScreen()->getCurrentScreen(),
                mpc.getLayeredScreen()->getFocusedFieldName()) &&
            field->isSplit())
        {
            if (!field->setActiveSplit(field->getActiveSplit() + 1))
            {
                field->setSplit(false);
            }
        }
    }
} // namespace mpc::command
