#include "controller/ClientInputController.h"

#include "inputlogic/ClientInput.h"
#include "controller/PadContextFactory.h"
#include "controls/GlobalReleaseControls.hpp"
#include "controls/BaseControls.hpp"
#include "Mpc.hpp"
#include "lcdgui/ScreenComponent.hpp"

using namespace mpc::controller;
using namespace mpc::inputlogic; 

ClientInputController::ClientInputController(mpc::Mpc &mpcToUse) : mpc(mpcToUse)
{
}

void ClientInputController::handleAction(const ClientInput& action)
{
    switch (action.type) {
    case ClientInput::Type::PadPress:
        return handlePadPress(action);
    case ClientInput::Type::PadAftertouch:
        return handlePadAftertouch(action);
    case ClientInput::Type::PadRelease:
        return handlePadRelease(action);
    case ClientInput::Type::DataWheelTurn:
        return handleDataWheel(action);
    case ClientInput::Type::SliderMove:
        return handleSlider(action);
    case ClientInput::Type::PotMove:
        return handlePot(action);
    case ClientInput::Type::ButtonPress:
        return handleButtonPress(action);
    case ClientInput::Type::ButtonRelease:
        return handleButtonRelease(action);
    default:
        return;
    }
}

void ClientInputController::handlePadPress(const ClientInput& a)
{
    if (!a.index) return;
    assert(a.value.has_value());
    const auto num = *a.index;
    auto padIndexWithBank = num + (mpc.getBank() * 16);
    auto ctx = controller::PadContextFactory::buildPadPushContext(mpc, padIndexWithBank, *a.value, mpc.getLayeredScreen()->getCurrentScreenName());
    mpc::controls::BaseControls::pad(ctx, padIndexWithBank, *a.value);
}

void ClientInputController::handlePadRelease(const ClientInput& a)
{
    if (!a.index) return;
    const auto num = *a.index;
    auto padIndexWithBank = num + (mpc.getBank() * 16);
    auto ctx = controller::PadContextFactory::buildPadReleaseContext(mpc, padIndexWithBank, mpc.getLayeredScreen()->getCurrentScreenName());
    mpc::controls::GlobalReleaseControls::simplePad(ctx);
}

void ClientInputController::handlePadAftertouch(const ClientInput& a)
{
    if (!a.index) return;
    const auto num = *a.index;
    std::printf("[logic] pad %d aftertouch pressure %d\n", num, a.value.value_or(0));
}

void ClientInputController::handleDataWheel(const ClientInput& a)
{
    assert(a.value.has_value());
    const auto steps = *a.value;
    std::printf("[logic] data wheel turned (%d)\n", steps);
    mpc.getActiveControls()->turnWheel(steps);
}

void ClientInputController::handleSlider(const ClientInput& a)
{
    std::printf("[logic] slider moved to %d\n", a.value.value_or(0));
}

void ClientInputController::handlePot(const ClientInput& a)
{
    std::printf("[logic] pot moved to %d\n", a.value.value_or(0));
}

void ClientInputController::handleButtonPress(const ClientInput& a)
{
    if (!a.label) return;
    std::printf("[logic] button %s pressed\n", a.label->c_str());
}

void ClientInputController::handleButtonRelease(const ClientInput& a)
{
    if (!a.label) return;
    std::printf("[logic] button %s released\n", a.label->c_str());
}
