#include "controller/ClientInputController.h"
#include "audiomidi/AudioMidiServices.hpp"
#include "command/ReleasePadCommand.h"
#include "hardware/ComponentId.h"
#include "hardware/HardwareComponent.h"
#include "inputlogic/ClientInput.h"
#include "controller/PadContextFactory.h"
#include "Mpc.hpp"
#include "lcdgui/ScreenComponent.hpp"
#include "hardware/Hardware.h"

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
        case ClientInput::Type::HostFocusLost:
            handleFocusLost();
        default:
            break;
    }
}

void ClientInputController::handlePadPress(const ClientInput& a)
{
    if (!a.index || !a.value) return;

    const auto physicalPadIndex = *a.index;

    const auto velocityToUse = std::clamp(*a.value * (float) hardware::VelocitySensitivePressable::MAX_VELO, (float) hardware::VelocitySensitivePressable::MIN_VELO, (float) hardware::VelocitySensitivePressable::MAX_VELO);

    auto pad = mpc.getHardware()->getPad(physicalPadIndex);

    if (!pad->pressWithVelocity(velocityToUse))
    {
        return;
    }

    auto screenName = mpc.getLayeredScreen()->getCurrentScreenName();

    registerPhysicalPadPush(physicalPadIndex, mpc.getBank(), screenName, a.source);
    const auto programPadIndex = physicalPadIndex + (mpc.getBank() * 16);
    auto ctx = controller::PadContextFactory::buildPushPadContext(mpc, programPadIndex, *a.value, screenName);
    command::PushPadCommand(ctx, programPadIndex, velocityToUse).execute();
}

void ClientInputController::handlePadRelease(const ClientInput& a)
{
    if (!a.index) return;

    const auto physicalPadIndex = *a.index;

    const auto info = registerPhysicalPadRelease(physicalPadIndex);

    mpc.getHardware()->getPad(physicalPadIndex)->release();

    const auto programPadIndex = physicalPadIndex + (info.bankIndex * 16);
    auto ctx = controller::PadContextFactory::buildPadReleaseContext(mpc, programPadIndex, info.screenName);
    command::ReleasePadCommand(ctx).execute();
}

void ClientInputController::handlePadAftertouch(const ClientInput& a)
{
    if (!a.index || !a.value) return;
    const auto padIndex = *a.index;
    const auto pressureToUse = std::clamp(*a.value * hardware::Aftertouchable::MAX_PRESSURE, (float) hardware::Aftertouchable::MIN_PRESSURE, (float) hardware::Aftertouchable::MAX_PRESSURE);
    mpc.getHardware()->getPad(padIndex)->aftertouch(pressureToUse);
}

void ClientInputController::handleDataWheel(const ClientInput& a)
{
    if (!a.deltaValue) return;
    float& acc = deltaAccumulators[a.componentId];
    acc += *a.deltaValue;
    int steps = static_cast<int>(acc);

    if (steps != 0)
    {
        acc -= steps;
        mpc.getHardware()->getDataWheel()->turn(steps);
        mpc.getActiveControls()->turnWheel(steps);
    }
}

void ClientInputController::handleSlider(const ClientInput& a)
{
    auto slider = mpc.getHardware()->getSlider();

    if (a.value)
    {
        slider->moveToNormalizedY(*a.value);
        mpc.getActiveControls()->setSlider(std::round(slider->getValue()));
    }
    else if (a.deltaValue && *a.deltaValue != 0.f)
    {
        slider->setValue(slider->getValue() + *a.deltaValue);
        mpc.getActiveControls()->setSlider(std::round(slider->getValue()));
    }
}

void ClientInputController::handlePot(const ClientInput& a)
{
    std::shared_ptr<mpc::hardware::Pot> pot =
        a.componentId == hardware::ComponentId::REC_GAIN_POT ? mpc.getHardware()->getRecPot() : mpc.getHardware()->getVolPot();

    auto audioMidiServices = mpc.getAudioMidiServices();

    pot->setValue(pot->getValue() + *a.deltaValue * 0.01f);

    if (a.componentId == hardware::ComponentId::REC_GAIN_POT)
    {
        audioMidiServices->setRecordLevel(std::round(pot->getValue() * 100.f));
    }
    else
    {
        audioMidiServices->setMainLevel(std::round(pot->getValue() * 100.f));
    }
}

void ClientInputController::handleButtonPress(const ClientInput& a)
{
    auto button = mpc.getHardware()->getButton(a.componentId);

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

    auto screen = mpc.getActiveControls();

    using Id = hardware::ComponentId;

    auto id = a.componentId;

    if (id == Id::CURSOR_LEFT_OR_DIGIT) { screen->left(); }
    else if (id == Id::CURSOR_RIGHT_OR_DIGIT) { screen->right(); }
    else if (id == Id::CURSOR_UP) { screen->up(); }
    else if (id == Id::CURSOR_DOWN) { screen->down(); }
    else if (id == Id::REC) { screen->rec(); }
    else if (id == Id::OVERDUB) { screen->overDub(); }
    else if (id == Id::STOP) { screen->stop(); }
    else if (id == Id::PLAY) { screen->play(); }
    else if (id == Id::PLAY_START) { screen->playStart(); }
    else if (id == Id::MAIN_SCREEN) { screen->mainScreen(); }
    else if (id == Id::OPEN_WINDOW) {
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
    else if (id == Id::PREV_STEP_OR_EVENT) { screen->prevStepEvent(); }
    else if (id == Id::NEXT_STEP_OR_EVENT) { screen->nextStepEvent(); }
    else if (id == Id::GO_TO) { screen->goTo(); }
    else if (id == Id::PREV_BAR_START) { screen->prevBarStart(); }
    else if (id == Id::NEXT_BAR_END) { screen->nextBarEnd(); }
    else if (id == Id::TAP_TEMPO_OR_NOTE_REPEAT) { screen->tap(); }
    else if (id == Id::NEXT_SEQ) { screen->nextSeq(); }
    else if (id == Id::TRACK_MUTE) { screen->trackMute(); }
    else if (id == Id::FULL_LEVEL_OR_CASE_SWITCH) { screen->fullLevel(); }
    else if (id == Id::SIXTEEN_LEVELS_OR_SPACE) { screen->sixteenLevels(); }
    else if (id == Id::F1) { screen->function(0); }
    else if (id == Id::F2) { screen->function(1); }
    else if (id == Id::F3) { screen->function(2); }
    else if (id == Id::F4) { screen->function(3); }
    else if (id == Id::F5) { screen->function(4); }
    else if (id == Id::F6) { screen->function(5); }
    else if (id == Id::SHIFT) { screen->shift(); }
    else if (id == Id::ENTER_OR_SAVE) { screen->pressEnter(); }
    else if (id == Id::UNDO_SEQ) { screen->undoSeq(); }
    else if (id == Id::ERASE) { screen->erase(); }
    else if (id == Id::AFTER_OR_ASSIGN) { screen->after(); }
    else if (id == Id::BANK_A) { screen->bank(0); }
    else if (id == Id::BANK_B) { screen->bank(1); }
    else if (id == Id::BANK_C) { screen->bank(2); }
    else if (id == Id::BANK_D) { screen->bank(3); }
    else if (id == Id::NUM_0_OR_VMPC) { screen->numpad(0); }
    else if (id == Id::NUM_1_OR_SONG) { screen->numpad(1); }
    else if (id == Id::NUM_2_OR_MISC) { screen->numpad(2); }
    else if (id == Id::NUM_3_OR_LOAD) { screen->numpad(3); }
    else if (id == Id::NUM_4_OR_SAMPLE) { screen->numpad(4); }
    else if (id == Id::NUM_5_OR_TRIM) { screen->numpad(5); }
    else if (id == Id::NUM_6_OR_PROGRAM) { screen->numpad(6); }
    else if (id == Id::NUM_7_OR_MIXER) { screen->numpad(7); }
    else if (id == Id::NUM_8_OR_OTHER) { screen->numpad(8); }
    else if (id == Id::NUM_9_OR_MIDI_SYNC) { screen->numpad(9); }
}

void ClientInputController::handleButtonRelease(const ClientInput& a)
{
    auto button = mpc.getHardware()->getButton(a.componentId);

    // The below check is necessary because the keyboard mapping routines in mpc::controls may return
    // labels like "ctrl" and "alt" rather than component labels. After we've improved the keyboard
    // input handling, we can remove this check.
    if (!button)
    {
        return;
    }

    button->release();

    using Id = hardware::ComponentId;

    auto id = a.componentId;

    if (id == Id::ERASE) { command::ReleaseEraseCommand(mpc).execute(); }
    else if (id == Id::F1) { command::ReleaseFunctionCommand(mpc, 0).execute(); }
    else if (id == Id::F3) { command::ReleaseFunctionCommand(mpc, 2).execute(); }
    else if (id == Id::F4) { command::ReleaseFunctionCommand(mpc, 3).execute(); }
    else if (id == Id::F5) { command::ReleaseFunctionCommand(mpc, 4).execute(); }
    else if (id == Id::F6) { command::ReleaseFunctionCommand(mpc, 5).execute(); }
    else if (id == Id::REC) { command::ReleaseRecCommand(mpc).execute(); }
    else if (id == Id::OVERDUB) { command::ReleaseOverdubCommand(mpc).execute(); }
    else if (id == Id::TAP_TEMPO_OR_NOTE_REPEAT) { command::ReleaseTapCommand(mpc).execute(); }
}

void ClientInputController::handleButtonDoublePress(const ClientInput& a)
{
    if (!mpc.getHardware()->getButton(a.componentId)->doublePress())
    {
        return;
    }

    if (a.componentId == hardware::ComponentId::REC)
    {
        buttonLockTracker.toggle(a.componentId);

        if (buttonLockTracker.isLocked(hardware::ComponentId::REC))
        {
            buttonLockTracker.unlock(hardware::ComponentId::OVERDUB);
        }
    }
    else if (a.componentId == hardware::ComponentId::OVERDUB)
    {
        buttonLockTracker.toggle(hardware::ComponentId::OVERDUB);

        if (buttonLockTracker.isLocked(hardware::ComponentId::OVERDUB))
        {
            buttonLockTracker.unlock(hardware::ComponentId::REC);
        }
    }
}

void ClientInputController::handleFocusLost()
{
    using Id = hardware::ComponentId;

    for (int physicalPadIndex = 0; physicalPadIndex < 16; ++physicalPadIndex)
    {
        if (!isPhysicallyPressedDueToFocusRequiringInput(physicalPadIndex))
        {
            continue;
        }

        ClientInput clientInput;
        clientInput.source = ClientInput::Source::Internal;
        clientInput.type = ClientInput::Type::PadRelease;
        clientInput.componentId = static_cast<Id>(Id::PAD_1_OR_AB + physicalPadIndex);
        clientInput.index = physicalPadIndex;
        handleAction(clientInput);
    }
}

