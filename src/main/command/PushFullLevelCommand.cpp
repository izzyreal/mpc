#include "PushFullLevelCommand.hpp"

#include "controller/ClientEventController.hpp"
#include "hardware/Hardware.hpp"
#include "input/PadAndButtonKeyboard.hpp"
#include "lcdgui/LayeredScreen.hpp"

#include <memory>

using namespace mpc::command;
using namespace mpc::hardware;
using namespace mpc::input;
using namespace mpc::lcdgui;
using namespace mpc::controller;

PushFullLevelCommand::PushFullLevelCommand(
    std::shared_ptr<LayeredScreen> layeredScreenToUse,
    std::shared_ptr<PadAndButtonKeyboard> padAndButtonKeyboardToUse,
    std::shared_ptr<Hardware> hardwareToUse,
    std::shared_ptr<ClientEventController> controllerToUse)
    : layeredScreen(layeredScreenToUse),
      padAndButtonKeyboard(padAndButtonKeyboardToUse), hardware(hardwareToUse),
      controller(controllerToUse)
{
}

void PushFullLevelCommand::execute()
{
    if (layeredScreen->isCurrentScreen({ScreenId::NameScreen}))
    {
        padAndButtonKeyboard->pressHardwareComponent(
            ComponentId::FULL_LEVEL_OR_CASE_SWITCH);
        hardware->getLed(ComponentId::FULL_LEVEL_OR_CASE_SWITCH_LED)
            ->setEnabled(!padAndButtonKeyboard->isUpperCase());
        return;
    }

    controller->setFullLevelEnabled(!controller->isFullLevelEnabled());
    hardware->getLed(ComponentId::FULL_LEVEL_OR_CASE_SWITCH_LED)
        ->setEnabled(controller->isFullLevelEnabled());
}
