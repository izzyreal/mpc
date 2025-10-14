#include "controller/ClientInputController.h"

#include "command/ReleasePadCommand.h"
#include "inputlogic/ClientInput.h"
#include "controller/PadContextFactory.h"
#include "Mpc.hpp"
#include "lcdgui/ScreenComponent.hpp"
#include "hardware2/Hardware2.h"

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
        case ClientInput::Type::ButtonDoublePress:
            return handleButtonDoublePress(action);
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
    auto ctx = controller::PadContextFactory::buildPushPadContext(mpc, padIndexWithBank, *a.value, mpc.getLayeredScreen()->getCurrentScreenName());
    command::PushPadCommand(ctx, padIndexWithBank, *a.value).execute();
}

void ClientInputController::handlePadRelease(const ClientInput& a)
{
    if (!a.index) return;
    const auto num = *a.index;
    auto padIndexWithBank = num + (mpc.getBank() * 16);
    auto ctx = controller::PadContextFactory::buildPadReleaseContext(mpc, padIndexWithBank, mpc.getLayeredScreen()->getCurrentScreenName());
    command::ReleasePadCommand(ctx).execute();
}

void ClientInputController::handlePadAftertouch(const ClientInput& a)
{
    if (!a.index || !a.value) return;
    const auto padIndex = *a.index;
    const auto aftertouchPressure = *a.value;
    std::printf("[logic] pad %d aftertouch pressure %d\n", padIndex, aftertouchPressure);
    mpc.getHardware2()->getPad(padIndex)->aftertouch(aftertouchPressure);
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
    if (!a.value) return;
    const auto newSliderValue = *a.value;
    std::printf("[logic] slider moved to %d\n", newSliderValue);
    mpc.getActiveControls()->setSlider(newSliderValue);
}

void ClientInputController::handlePot(const ClientInput& a)
{
    std::printf("[logic] pot moved to %d\n", a.value.value_or(0));
}

void ClientInputController::handleButtonPress(const ClientInput& a)
{
    if (!a.label) return;
    std::printf("[logic] button %s pressed\n", a.label->c_str());

    auto label = a.label.value();
    auto screen = mpc.getActiveControls();

    if (label == "left") {
        screen->left();
    }
    else if (label == "right") {
        screen->right();
    }
    else if (label == "up") {
        screen->up();
    }
    else if (label == "down") {
        screen->down();
    }
    else if (label == "rec") {
        doubleClickLockTracker.unlock(label);
        screen->rec();
    }
    else if (label == "overdub") {
        doubleClickLockTracker.unlock(label);
        screen->overDub();
    }
    else if (label == "stop") {
        screen->stop();
    }
    else if (label == "play") {
        screen->play();
    }
    else if (label == "play-start") {
        screen->playStart();
    }
    else if (label == "main-screen") {
        screen->mainScreen();
    }
    else if (label == "open-window") {
        auto ls = mpc.getLayeredScreen();
        auto currentScreenName = ls->getCurrentScreenName();

        const auto layerIndex = ls->getFocusedLayerIndex();

        screen->openWindow();

        if (currentScreenName != ls->getCurrentScreenName() &&
                currentScreenName != "popup" &&
                ls->getFocusedLayerIndex() > layerIndex)
        {
            ls->setScreenToReturnToWhenPressingOpenWindow(currentScreenName);
        }
    }
    else if (label == "prev-step-event") {
        screen->prevStepEvent();
    }
    else if (label == "next-step-event") {
        screen->nextStepEvent();
    }
    else if (label == "go-to") {
        screen->goTo();
    }
    else if (label == "prev-bar-start") {
        screen->prevBarStart();
    }
    else if (label == "next-bar-end") {
        screen->nextBarEnd();
    }
    else if (label == "tap")
    {
        screen->tap();
    }
    else if (label == "next-seq") {
        screen->nextSeq();
    }
    else if (label == "track-mute") {
        screen->trackMute();
    }
    else if (label == "full-level") {
        screen->fullLevel();
    }
    else if (label == "sixteen-levels") {
        screen->sixteenLevels();
    }
    else if (label == "f1") {
        screen->function(0);
    }
    else if (label == "f2") {
        screen->function(1);
    }
    else if (label == "f3") {
        screen->function(2);
    }
    else if (label == "f4") {
        screen->function(3);
    }
    else if (label == "f5") {
        screen->function(4);
    }
    else if (label == "f6") {
        screen->function(5);
    }
    else if (label == "shift")
    {
        screen->shift();
    }
    else if (label == "enter") {
        screen->pressEnter();
    }
    else if (label == "undo-seq") {
        screen->undoSeq();
    }
    else if (label == "erase") {
        screen->erase();
    }
    else if (label == "after") {
        screen->after();
    }
    else if (label == "bank-a") {
        screen->bank(0);
    }
    else if (label == "bank-b") {
        screen->bank(1);
    }
    else if (label == "bank-c") {
        screen->bank(2);
    }
    else if (label == "bank-d") {
        screen->bank(3);
    }
    else if (label == "0" || label == "0_extra") {
        screen->numpad(0);
    }
    else if (label == "1" || label == "1_extra") {
        screen->numpad(1);
    }
    else if (label == "2" || label == "2_extra") {
        screen->numpad(2);
    }
    else if (label == "3" || label == "3_extra") {
        screen->numpad(3);
    }
    else if (label == "4" || label == "4_extra") {
        screen->numpad(4);
    }
    else if (label == "5" || label == "5_extra") {
        screen->numpad(5);
    }
    else if (label == "6" || label == "6_extra") {
        screen->numpad(6);
    }
    else if (label == "7" || label == "7_extra") {
        screen->numpad(7);
    }
    else if (label == "8" || label == "8_extra") {
        screen->numpad(8);
    }
    else if (label == "9" || label == "9_extra") {
        screen->numpad(9);
    }
}

void ClientInputController::handleButtonRelease(const ClientInput& a)
{
    if (!a.label)
    {
        return;
    }

    const auto label = a.label.value();

    std::printf("[logic] button %s released\n", a.label->c_str());

	if (label == "erase") {
		command::ReleaseEraseCommand(mpc).execute();
	}
	else if (label == "f1") {
		command::ReleaseFunctionCommand(mpc, 0).execute();
	}
	else if (label == "f3") {
		command::ReleaseFunctionCommand(mpc, 2).execute();
	}
	else if (label == "f4") {
		command::ReleaseFunctionCommand(mpc, 3).execute();
	}
	else if (label == "f5") {
		command::ReleaseFunctionCommand(mpc, 4).execute();
	}
	else if (label == "f6") {
		command::ReleaseFunctionCommand(mpc, 5).execute();
	}
	else if (label == "rec") {
		command::ReleaseRecCommand(mpc).execute();
	}
	else if (label == "overdub") {
		command::ReleaseOverdubCommand(mpc).execute();
	}
	else if (label == "tap") {
		command::ReleaseTapCommand(mpc).execute();
	}
}

void ClientInputController::handleButtonDoublePress(const mpc::inputlogic::ClientInput& input)
{
    if (!input.label.has_value())
    {
        return;
    }

    auto label = *input.label;

    if (label == "rec" || label == "overdub")
    {
        buttonLockTracker.toggle(label);
        printf("handled double press for %s\n", label.c_str());
    }
}

