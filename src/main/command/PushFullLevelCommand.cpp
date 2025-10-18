#include "PushFullLevelCommand.h"
#include "Mpc.hpp"
#include "hardware/Hardware.h"
#include "inputlogic/PadAndButtonKeyboard.hpp"
#include "lcdgui/screens/window/NameScreen.hpp"

#include <memory>

using namespace mpc::command;
using namespace mpc::hardware;
using namespace mpc::lcdgui::screens::window;

PushFullLevelCommand::PushFullLevelCommand(mpc::Mpc &mpc) : mpc(mpc) {}

void PushFullLevelCommand::execute()
{
    if (std::dynamic_pointer_cast<NameScreen>(mpc.getScreen()))
    {
        mpc.getPadAndButtonKeyboard()->pressHardwareComponent(ComponentId::FULL_LEVEL_OR_CASE_SWITCH);
        mpc.getHardware()->getLed(ComponentId::FULL_LEVEL_OR_CASE_SWITCH_LED)->setEnabled(!mpc.getPadAndButtonKeyboard()->isUpperCase());
        return;
    }

    mpc.setFullLevelEnabled(!mpc.isFullLevelEnabled());
    mpc.getHardware()->getLed(ComponentId::FULL_LEVEL_OR_CASE_SWITCH_LED)->setEnabled(mpc.isFullLevelEnabled());
}

