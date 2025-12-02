#include "PushEnterCommand.hpp"
#include "Mpc.hpp"
#include "hardware/Hardware.hpp"

using namespace mpc::command;
using namespace mpc::lcdgui;

PushEnterCommand::PushEnterCommand(Mpc &mpc) : mpc(mpc) {}

void PushEnterCommand::execute()
{
    if (mpc.getHardware()->getButton(hardware::ComponentId::SHIFT)->isPressed())
    {
        mpc.getLayeredScreen()->openScreenById(ScreenId::SaveScreen);
    }
}
