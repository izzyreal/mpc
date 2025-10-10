#include "inputsystem/Initializer.h"

#include "inputlogic/ClientInputMapper.h"
#include "controller/ClientInputControllerBase.h"

using namespace mpc::inputsystem;
using namespace mpc::inputlogic;
using namespace mpc::controller;

void Initializer::init(ClientInputMapper &inputMapper,
                       std::shared_ptr<ClientInputControllerBase> inputController,
                       const std::vector<std::string> buttonLabels)
{
    // Bind pad handlers
    for (int padNumber = 0; padNumber < 16; ++padNumber)
    {
        inputMapper.bindPadPress(padNumber, [inputController](const ClientInput& a) {
            inputController->handleAction(a);
        });
        inputMapper.bindPadRelease(padNumber, [inputController](const ClientInput& a) {
            inputController->handleAction(a);
        });
        inputMapper.bindPadAftertouch(padNumber, [inputController](const ClientInput& a) {
            inputController->handleAction(a);
        });
    }

    // Data wheel
    inputMapper.bindDataWheel([inputController](const ClientInput& a) {
        inputController->handleAction(a);
    });

    // Slider
    inputMapper.bindSlider([inputController](const ClientInput& a) {
        inputController->handleAction(a);
    });

    // Pot
    inputMapper.bindPot([inputController](const ClientInput& a) {
        inputController->handleAction(a);
    });

    for (auto &buttonLabel : buttonLabels)
    {
        inputMapper.bindButtonPress(buttonLabel, [inputController](const ClientInput& a) {
            inputController->handleAction(a);
        });
        inputMapper.bindButtonRelease(buttonLabel, [inputController](const ClientInput& a) {
            inputController->handleAction(a);
        });
    }
}
