#include "controller/ClientInputController.h"
#include "command/ReleasePadCommand.h"
#include "hardware2/HardwareComponent.h"
#include "inputlogic/ClientInput.h"
#include "controller/PadContextFactory.h"
#include "Mpc.hpp"
#include "lcdgui/ScreenComponent.hpp"
#include "hardware2/Hardware2.h"

using namespace mpc::controller;
using namespace mpc::inputlogic;

ClientInputController::ClientInputController(mpc::Mpc& mpcToUse, const fs::path keyboardMappingConfigDirectory)
    : ClientInputControllerBase(mpcToUse.paths->configPath()), mpc(mpcToUse)
{
}

void ClientInputController::handleAction(const ClientInput& action)
{
    switch (action.type) {
        case ClientInput::Type::PadPress:
            handlePadPress(action);
            break;
        case ClientInput::Type::PadAftertouch:
            handlePadAftertouch(action);
            break;
        case ClientInput::Type::PadRelease:
            handlePadRelease(action);
            break;
        case ClientInput::Type::DataWheelTurn:
            handleDataWheel(action);
            break;
        case ClientInput::Type::SliderMove:
            handleSlider(action);
            break;
        case ClientInput::Type::PotMove:
            handlePot(action);
            break;
        case ClientInput::Type::ButtonPress:
            handleButtonPress(action);
            break;
        case ClientInput::Type::ButtonRelease:
            handleButtonRelease(action);
            break;
        case ClientInput::Type::ButtonDoublePress:
            handleButtonDoublePress(action);
            break;
        default:
            break;
    }
}

void ClientInputController::handlePadPress(const ClientInput& a)
{
    if (!a.index || !a.value) return;

    const auto physicalPadIndex = *a.index;

    if (!mpc.getHardware2()->getPad(physicalPadIndex)->pressWithVelocity(*a.value))
    {
        return;
    }

    auto screenName = mpc.getLayeredScreen()->getCurrentScreenName();

    registerPhysicalPadPush(physicalPadIndex, mpc.getBank(), screenName);
    const auto programPadIndex = physicalPadIndex + (mpc.getBank() * 16);
    auto ctx = controller::PadContextFactory::buildPushPadContext(mpc, programPadIndex, *a.value, screenName);
    command::PushPadCommand(ctx, programPadIndex, *a.value).execute();
}

void ClientInputController::handlePadRelease(const ClientInput& a)
{
    if (!a.index) return;

    const auto physicalPadIndex = *a.index;

    const auto info = registerPhysicalPadRelease(physicalPadIndex);

    mpc.getHardware2()->getPad(physicalPadIndex)->release();

    const auto programPadIndex = physicalPadIndex + (info.bankIndex * 16);
    auto ctx = controller::PadContextFactory::buildPadReleaseContext(mpc, programPadIndex, info.screenName);
    command::ReleasePadCommand(ctx).execute();
}

void ClientInputController::handlePadAftertouch(const ClientInput& a)
{
    if (!a.index || !a.value) return;
    const auto padIndex = *a.index;
    const auto aftertouchPressure = *a.value;
    mpc.getHardware2()->getPad(padIndex)->aftertouch(aftertouchPressure);
}

void ClientInputController::handleDataWheel(const ClientInput& a)
{
    if (!a.value) return;
    const auto steps = *a.value;
    mpc.getHardware2()->getDataWheel()->turn(steps);
    mpc.getActiveControls()->turnWheel(steps);
}

void ClientInputController::handleSlider(const ClientInput& a)
{
    if (!a.value && !a.deltaValue) return;
    
    if (a.value && a.deltaValue && *a.deltaValue != 0.f)
    {
        return;
    }

    auto slider = mpc.getHardware2()->getSlider();

    if (a.value)
    {
        slider->moveToNormalizedY(*a.value);
        mpc.getActiveControls()->setSlider(std::round(slider->getValue()));
    }
    else if (*a.deltaValue != 0.f)
    {
        const auto currentSliderValue = slider->getValue();
        slider->setValue(currentSliderValue + *a.deltaValue);
        mpc.getActiveControls()->setSlider(std::round(slider->getValue()));
    }
}

void ClientInputController::handlePot(const ClientInput& a)
{
    if (!a.label)
    {
        return;
    }

    if (!a.deltaValue) return;

    const float potDelta = *a.deltaValue;

    if (a.label->find("rec") != std::string::npos)
    {
        auto pot = mpc.getHardware2()->getRecPot();
        pot->moveTo(pot->getValue() + potDelta);
    }
    else if (a.label->find("main") != std::string::npos)
    {
        auto pot = mpc.getHardware2()->getVolPot();
        pot->moveTo(pot->getValue() + potDelta);
    }
}

void ClientInputController::handleButtonPress(const ClientInput& a)
{
    if (!a.label) return;

    auto button = mpc.getHardware2()->getButton(*a.label);

    // The below check is necessary because the keyboard mapping routines in mpc::controls may return
    // labels like "ctrl" and "alt" rather than component labels. After we've improved the keyboard
    // input handling, we can remove this check.
    if (!button)
    {
        return;
    }

    if (!button->press())
    {
        return;
    }

    auto label = a.label.value();
    auto screen = mpc.getActiveControls();

    if (label == "left") { screen->left(); }
    else if (label == "right") { screen->right(); }
    else if (label == "up") { screen->up(); }
    else if (label == "down") { screen->down(); }
    else if (label == "rec") { screen->rec(); }
    else if (label == "overdub") { screen->overDub(); }
    else if (label == "stop") { screen->stop(); }
    else if (label == "play") { screen->play(); }
    else if (label == "play-start") { screen->playStart(); }
    else if (label == "main-screen") { screen->mainScreen(); }
    else if (label == "open-window") {
        auto ls = mpc.getLayeredScreen();
        auto currentScreenName = ls->getCurrentScreenName();
        const auto layerIndex = ls->getFocusedLayerIndex();
        screen->openWindow();
        if (currentScreenName != ls->getCurrentScreenName() &&
            currentScreenName != "popup" &&
            ls->getFocusedLayerIndex() > layerIndex) {
            ls->setScreenToReturnToWhenPressingOpenWindow(currentScreenName);
        }
    }
    else if (label == "prev-step-event") { screen->prevStepEvent(); }
    else if (label == "next-step-event") { screen->nextStepEvent(); }
    else if (label == "go-to") { screen->goTo(); }
    else if (label == "prev-bar-start") { screen->prevBarStart(); }
    else if (label == "next-bar-end") { screen->nextBarEnd(); }
    else if (label == "tap") { screen->tap(); }
    else if (label == "next-seq") { screen->nextSeq(); }
    else if (label == "track-mute") { screen->trackMute(); }
    else if (label == "full-level") { screen->fullLevel(); }
    else if (label == "sixteen-levels") { screen->sixteenLevels(); }
    else if (label == "f1") { screen->function(0); }
    else if (label == "f2") { screen->function(1); }
    else if (label == "f3") { screen->function(2); }
    else if (label == "f4") { screen->function(3); }
    else if (label == "f5") { screen->function(4); }
    else if (label == "f6") { screen->function(5); }
    else if (label == "shift") { screen->shift(); }
    else if (label == "enter") { screen->pressEnter(); }
    else if (label == "undo-seq") { screen->undoSeq(); }
    else if (label == "erase") { screen->erase(); }
    else if (label == "after") { screen->after(); }
    else if (label == "bank-a") { screen->bank(0); }
    else if (label == "bank-b") { screen->bank(1); }
    else if (label == "bank-c") { screen->bank(2); }
    else if (label == "bank-d") { screen->bank(3); }
    else if (label == "0" || label == "0_extra") { screen->numpad(0); }
    else if (label == "1" || label == "1_extra") { screen->numpad(1); }
    else if (label == "2" || label == "2_extra") { screen->numpad(2); }
    else if (label == "3" || label == "3_extra") { screen->numpad(3); }
    else if (label == "4" || label == "4_extra") { screen->numpad(4); }
    else if (label == "5" || label == "5_extra") { screen->numpad(5); }
    else if (label == "6" || label == "6_extra") { screen->numpad(6); }
    else if (label == "7" || label == "7_extra") { screen->numpad(7); }
    else if (label == "8" || label == "8_extra") { screen->numpad(8); }
    else if (label == "9" || label == "9_extra") { screen->numpad(9); }
}

void ClientInputController::handleButtonRelease(const ClientInput& a)
{
    if (!a.label) return;

    auto button = mpc.getHardware2()->getButton(*a.label);

    // The below check is necessary because the keyboard mapping routines in mpc::controls may return
    // labels like "ctrl" and "alt" rather than component labels. After we've improved the keyboard
    // input handling, we can remove this check.
    if (!button)
    {
        return;
    }

    button->release();

    const auto label = a.label.value();
    if (label == "erase") { command::ReleaseEraseCommand(mpc).execute(); }
    else if (label == "f1") { command::ReleaseFunctionCommand(mpc, 0).execute(); }
    else if (label == "f3") { command::ReleaseFunctionCommand(mpc, 2).execute(); }
    else if (label == "f4") { command::ReleaseFunctionCommand(mpc, 3).execute(); }
    else if (label == "f5") { command::ReleaseFunctionCommand(mpc, 4).execute(); }
    else if (label == "f6") { command::ReleaseFunctionCommand(mpc, 5).execute(); }
    else if (label == "rec") { command::ReleaseRecCommand(mpc).execute(); }
    else if (label == "overdub") { command::ReleaseOverdubCommand(mpc).execute(); }
    else if (label == "tap") { command::ReleaseTapCommand(mpc).execute(); }
}

void ClientInputController::handleButtonDoublePress(const ClientInput& a)
{
    if (!a.label) return;
    mpc.getHardware2()->getButton(*a.label)->doublePress();

    auto label = *a.label;
    if (label == "rec") {
        buttonLockTracker.toggle("rec");
        if (buttonLockTracker.isLocked("rec")) {
            buttonLockTracker.unlock("overdub");
        }
    }
    else if (label == "overdub") {
        buttonLockTracker.toggle("overdub");
        if (buttonLockTracker.isLocked("overdub")) {
            buttonLockTracker.unlock("rec");
        }
    }
}

