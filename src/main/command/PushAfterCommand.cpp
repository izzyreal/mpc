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
        const std::shared_ptr<ClientEventController>
            &controllerToUse,
        const std::shared_ptr<LayeredScreen> &layeredScreenToUse,
        const std::shared_ptr<Hardware> &hardwareToUse)
        : controller(controllerToUse), layeredScreen(layeredScreenToUse),
          hardware(hardwareToUse)
    {
    }

    void PushAfterCommand::execute()
    {
        if (hardware->getButton(SHIFT)->isPressed())
        {
            layeredScreen->openScreenById(ScreenId::AssignScreen);
        }
        else
        {
            controller->setAfterEnabled(!controller->isAfterEnabled());
            hardware->getLed(AFTER_OR_ASSIGN_LED)
                ->setEnabled(controller->isAfterEnabled());
        }
    }

} // namespace mpc::command
