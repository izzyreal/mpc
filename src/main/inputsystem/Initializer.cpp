#include "inputsystem/Initializer.h"

#include "inputlogic/InputMapper.h"
#include "controller/InputControllerBase.h"

#include <string>

using namespace mpc::inputsystem;
using namespace mpc::inputlogic;
using namespace mpc::controller;

void Initializer::init(InputMapper &inputMapper, std::shared_ptr<InputControllerBase> inputController)
{
    for (int padNumber = 0; padNumber < 16; ++padNumber)
    {
        const auto pressActionId = "pad-" + std::to_string(padNumber) + "-press";
        inputMapper.bind(pressActionId, [inputController](const InputAction& a) {
            inputController->handleAction(a);
        });
        const auto releaseActionId = "pad-" + std::to_string(padNumber) + "-release";
        inputMapper.bind(releaseActionId, [inputController](const InputAction& a) {
            inputController->handleAction(a);
        });
    }

    inputMapper.bind("datawheel-up", [inputController](const InputAction& a) {
        inputController->handleAction(a);
    });

    inputMapper.bind("datawheel-down", [inputController](const InputAction& a) {
        inputController->handleAction(a);
    });
}

