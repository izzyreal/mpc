#include "PushSixteenLevelsCommand.hpp"

#include "lcdgui/LayeredScreen.hpp"
#include "controller/ClientEventController.hpp"

#include "hardware/Hardware.hpp"

using namespace mpc::command;
using namespace mpc::lcdgui;
using namespace mpc::controller;
using namespace mpc::hardware;

PushSixteenLevelsCommand::PushSixteenLevelsCommand(
    std::shared_ptr<LayeredScreen> layeredScreenToUse,
    std::shared_ptr<ClientEventController> controllerToUse,
    std::shared_ptr<Hardware> hardwareToUse)
    : layeredScreen(layeredScreenToUse), controller(controllerToUse),
      hardware(hardwareToUse)
{
}

void PushSixteenLevelsCommand::execute()
{
    if (!layeredScreen->isCurrentScreen({ScreenId::SequencerScreen,
                                        ScreenId::Assign16LevelsScreen}))
    {
        return;
    }

    if (controller->isSixteenLevelsEnabled())
    {
        controller->setSixteenLevelsEnabled(false);
        hardware->getLed(hardware::ComponentId::SIXTEEN_LEVELS_OR_SPACE_LED)
            ->setEnabled(false);
    }
    else
    {
        layeredScreen->openScreenById(ScreenId::Assign16LevelsScreen);
    }
}
