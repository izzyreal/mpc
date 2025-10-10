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

    std::vector<std::string> buttonLabels { "left","right","up","down","rec","overdub","stop","play","play-start","main-screen","prev-step-event","next-step-event","go-to","prev-bar-start","next-bar-end","tap","next-seq","track-mute","open-window","full-level","sixteen-levels","f1","f2","f3","f4","f5","f6","shift","enter","undo-seq","erase","after","bank-a","bank-b","bank-c","bank-d","0","1","2","3","4","5","6","7","8","9" };

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
