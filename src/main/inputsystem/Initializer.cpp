#include "inputsystem/Initializer.h"

#include "inputlogic/ClientInputMapper.h"
#include "controller/ClientInputControllerBase.h"

#include <string>

using namespace mpc::inputsystem;
using namespace mpc::inputlogic;
using namespace mpc::controller;

void Initializer::init(ClientInputMapper &inputMapper, std::shared_ptr<ClientInputControllerBase> inputController)
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

    // Example button bindings - any button label can be bound elsewhere as needed
    inputMapper.bindButtonPress("play", [inputController](const ClientInput& a) {
        inputController->handleAction(a);
    });
    inputMapper.bindButtonRelease("play", [inputController](const ClientInput& a) {
        inputController->handleAction(a);
    });

    inputMapper.bindButtonPress("stop", [inputController](const ClientInput& a) {
        inputController->handleAction(a);
    });
    inputMapper.bindButtonRelease("stop", [inputController](const ClientInput& a) {
        inputController->handleAction(a);
    });

    // Other buttons can be bound by the hardware code when constructing buttons if desired.
}
