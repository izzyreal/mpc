#include "PushAfterCommand.hpp"

#include "controller/ClientEventController.hpp"
#include "hardware/Hardware.hpp"
#include "lcdgui/LayeredScreen.hpp"

using namespace mpc::controller;
using namespace mpc::hardware;
using namespace mpc::lcdgui;

namespace mpc::command
{

    PushAfterCommand::PushAfterCommand(
        std::shared_ptr<controller::ClientEventController> controllerToUse,
        std::shared_ptr<lcdgui::LayeredScreen> layeredScreenToUse,
        std::shared_ptr<hardware::Hardware> hardwareToUse)
        : controller(controllerToUse), layeredScreen(layeredScreenToUse),
          hardware(hardwareToUse)
    {
    }

    void PushAfterCommand::execute()
    {
        if (hardware->getButton(hardware::ComponentId::SHIFT)->isPressed())
        {
            layeredScreen->openScreenById(ScreenId::AssignScreen);
        }
        else
        {
            controller->setAfterEnabled(!controller->isAfterEnabled());
            hardware->getLed(hardware::ComponentId::AFTER_OR_ASSIGN_LED)
                ->setEnabled(controller->isAfterEnabled());
        }
    }

} // namespace mpc::command
