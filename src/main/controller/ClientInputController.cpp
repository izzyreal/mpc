#include "controller/ClientInputController.h"
#include "audiomidi/AudioMidiServices.hpp"
#include "command/TriggerDrumNoteOffCommand.h"
#include "command/context/NoteInputScreenUpdateContext.h"
#include "command/context/PushPadScreenUpdateContext.h"
#include "hardware/ComponentId.h"
#include "hardware/HardwareComponent.h"
#include "inputlogic/ClientInput.h"
#include "command/context/TriggerDrumContextFactory.h"
#include "Mpc.hpp"
#include "lcdgui/ScreenGroups.h"
#include "lcdgui/ScreenComponent.hpp"
#include "lcdgui/screens/SequencerScreen.hpp"
#include "lcdgui/screens/StepEditorScreen.hpp"
#include "lcdgui/screens/VmpcKeyboardScreen.hpp"
#include "lcdgui/screens/VmpcMidiScreen.hpp"
#include "lcdgui/screens/dialog2/PopupScreen.hpp"
#include "hardware/Hardware.h"
#include "lcdgui/screens/window/KeepOrRetryScreen.hpp"
#include "lcdgui/screens/window/LoadASoundScreen.hpp"
#include "lcdgui/screens/window/NameScreen.hpp"
#include <memory>

using namespace mpc::controller;
using namespace mpc::inputlogic;
using namespace mpc::command;
using namespace mpc::command::context;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens::dialog2;

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
        case ClientInput::Type::ButtonPressAndRelease:
            handleButtonPress(action);
            handleButtonRelease(action);
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

    auto layeredScreen = mpc.getLayeredScreen();
    auto screen = mpc.getScreen();

    if (auto popupScreen = std::dynamic_pointer_cast<PopupScreen>(screen);
        popupScreen && !popupScreen->getScreenToReturnTo().empty())
    {
        layeredScreen->openScreen(popupScreen->getScreenToReturnTo());
    }

    auto screenName = layeredScreen->getCurrentScreenName();

    registerPhysicalPadPush(physicalPadIndex, mpc.getBank(), screenName, a.source);
    const auto programPadIndex = physicalPadIndex + (mpc.getBank() * 16);
    auto ctx = TriggerDrumContextFactory::buildTriggerDrumNoteOnContext(mpc, programPadIndex, *a.value, screenName);

    TriggerDrumNoteOnCommand(ctx, programPadIndex, velocityToUse).execute();

    const bool isF4Pressed = mpc.getHardware()->getButton(hardware::ComponentId::F4)->isPressed();
    const bool isF6Pressed = mpc.getHardware()->getButton(hardware::ComponentId::F6)->isPressed();

    PushPadScreenUpdateContext padPushScreenUpdateCtx {
        ctx.isSixteenLevelsEnabled,
        ctx.screenComponent,
        ctx.program,
        ctx.sequencer,
        isF4Pressed,
        isF6Pressed,
        mpc.getBank(),
        ctx.setMpcPad,
        ctx.allowCentralNoteAndPadUpdate
    };

    PushPadScreenUpdateCommand(padPushScreenUpdateCtx, programPadIndex).execute();

    NoteInputScreenUpdateContext noteInputScreenUpdateContext {
        ctx.isSixteenLevelsEnabled,
        ctx.allowCentralNoteAndPadUpdate,
        ctx.screenComponent,
        ctx.setMpcNote,
        ctx.currentFieldName
    };

    NoteInputScreenUpdateCommand(noteInputScreenUpdateContext, ctx.note).execute();
}

void ClientInputController::handlePadRelease(const ClientInput& a)
{
    if (!a.index) return;

    const auto physicalPadIndex = *a.index;

    const auto info = registerPhysicalPadRelease(physicalPadIndex);

    mpc.getHardware()->getPad(physicalPadIndex)->release();

    const auto programPadIndex = physicalPadIndex + (info.bankIndex * 16);
    auto ctx = TriggerDrumContextFactory::buildTriggerDrumNoteOffContext(mpc, programPadIndex, info.screenName);
    TriggerDrumNoteOffCommand(ctx).execute();
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
        mpc.getScreen()->turnWheel(steps);
    }
}

void ClientInputController::handleSlider(const ClientInput& a)
{
    auto slider = mpc.getHardware()->getSlider();

    if (a.value)
    {
        slider->moveToNormalizedY(*a.value);
        mpc.getScreen()->setSlider(std::round(slider->getValue()));
    }
    else if (a.deltaValue && *a.deltaValue != 0.f)
    {
        slider->setValue(slider->getValue() + *a.deltaValue);
        mpc.getScreen()->setSlider(std::round(slider->getValue()));
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

    auto screen = mpc.getScreen();
    auto layeredScreen = mpc.getLayeredScreen();
    const auto currentScreenName = layeredScreen->getCurrentScreenName();

    using Id = hardware::ComponentId;

    const auto id = a.componentId;

    if (auto stepEditorScreen = std::dynamic_pointer_cast<StepEditorScreen>(screen); stepEditorScreen)
    {
        if (id == Id::PREV_STEP_OR_EVENT)
        {
            stepEditorScreen->prevStepEvent();
            return;
        }
        else if (id == Id::NEXT_STEP_OR_EVENT)
        {
            stepEditorScreen->nextStepEvent();
            return;
        }
        else if (id == Id::PREV_BAR_START)
        {
            stepEditorScreen->prevBarStart();
            return;
        }
        else if (id == Id::NEXT_BAR_END)
        {
            stepEditorScreen->nextBarEnd();
            return;
        }
    }

    if (auto popupScreen = std::dynamic_pointer_cast<PopupScreen>(screen);
        popupScreen && !popupScreen->getScreenToReturnTo().empty())
    {
        layeredScreen->openScreen(popupScreen->getScreenToReturnTo());
        return;
    }

    if (id == Id::CURSOR_LEFT_OR_DIGIT) { screen->left(); }
    else if (id == Id::CURSOR_RIGHT_OR_DIGIT) { screen->right(); }
    else if (id == Id::CURSOR_UP) { screen->up(); }
    else if (id == Id::CURSOR_DOWN) { screen->down(); }
    else if (id == Id::REC) { screen->rec(); }
    else if (id == Id::OVERDUB) { screen->overDub(); }
    else if (id == Id::STOP) { screen->stop(); }
    else if (id == Id::PLAY) { screen->play(); }
    else if (id == Id::PLAY_START) { screen->playStart(); }
    else if (id == Id::MAIN_SCREEN)
    {
        if (screengroups::isScreenThatIsNotAllowedToOpenMainScreen(currentScreenName))
        {
            return;
        }

        if (auto vmpcKeyboardScreen = std::dynamic_pointer_cast<VmpcKeyboardScreen>(screen); vmpcKeyboardScreen)
        {
            if (vmpcKeyboardScreen->hasMappingChanged())
            {
                layeredScreen->openScreen("vmpc-discard-mapping-changes");
                return;
            }
        }
        else if (auto vmpcMidiScreen = std::dynamic_pointer_cast<VmpcMidiScreen>(screen); vmpcMidiScreen)
        {
            if (vmpcMidiScreen->hasMappingChanged())
            {
                layeredScreen->openScreen("vmpc-discard-mapping-changes");
                return;
            }
        }
        else if (auto loadASoundScreen = std::dynamic_pointer_cast<LoadASoundScreen>(screen); loadASoundScreen)
        {
            mpc.getSampler()->deleteSound(mpc.getSampler()->getPreviewSound());
        }
        else if (auto nameScreen = std::dynamic_pointer_cast<NameScreen>(screen); nameScreen)
        {
            nameScreen->mainScreenAction();
        }
        else if (auto keepOrRetryScreen = std::dynamic_pointer_cast<KeepOrRetryScreen>(screen); keepOrRetryScreen)
        {
            mpc.getSampler()->deleteSound(mpc.getSampler()->getPreviewSound());
        }

        PushMainScreenCommand(mpc).execute();
    }
    else if (id == Id::OPEN_WINDOW)
    {
        const auto layerIndex = layeredScreen->getFocusedLayerIndex();
        screen->openWindow();

        if (currentScreenName != layeredScreen->getCurrentScreenName() &&
            currentScreenName != "popup" &&
            layeredScreen->getFocusedLayerIndex() > layerIndex)
        {
            layeredScreen->setScreenToReturnToWhenPressingOpenWindow(currentScreenName);
        }
    }
    else if (id == Id::GO_TO)
    {
        PushGoToCommand(mpc).execute();
    }
    else if (id == Id::TAP_TEMPO_OR_NOTE_REPEAT)
    {
        PushTapCommand(mpc).execute();

        if (auto sequencerScreen = std::dynamic_pointer_cast<SequencerScreen>(screen); sequencerScreen)
        {
            // Pure UI update
            sequencerScreen->tap();
        }
    }
    else if (id == Id::NEXT_SEQ)
    {
        PushNextSeqCommand(mpc).execute();
    }
    else if (id == Id::TRACK_MUTE) { PushTrackMuteCommand(mpc).execute(); }
    else if (id == Id::FULL_LEVEL_OR_CASE_SWITCH) { PushFullLevelCommand(mpc).execute(); }
    else if (id == Id::SIXTEEN_LEVELS_OR_SPACE) { PushSixteenLevelsCommand(mpc).execute(); }
    else if (id == Id::F1) { screen->function(0); }
    else if (id == Id::F2) { screen->function(1); }
    else if (id == Id::F3) { screen->function(2); }
    else if (id == Id::F4) { screen->function(3); }
    else if (id == Id::F5) { screen->function(4); }
    else if (id == Id::F6) { screen->function(5); }
    else if (id == Id::SHIFT)
    {
        PushShiftCommand(mpc).execute();

        if (auto stepEditorScreen = std::dynamic_pointer_cast<StepEditorScreen>(screen); stepEditorScreen)
        {
            // Start/reset event selection and update UI
            stepEditorScreen->shift();
        }
    }
    else if (id == Id::ENTER_OR_SAVE) { screen->pressEnter(); }
    else if (id == Id::UNDO_SEQ) { screen->undoSeq(); }
    else if (id == Id::ERASE) { screen->erase(); }
    else if (id == Id::AFTER_OR_ASSIGN) { PushAfterCommand(mpc).execute(); }
    else if (id == Id::BANK_A) { PushBankCommand(mpc, 0).execute(); }
    else if (id == Id::BANK_B) { PushBankCommand(mpc, 1).execute(); }
    else if (id == Id::BANK_C) { PushBankCommand(mpc, 2).execute(); }
    else if (id == Id::BANK_D) { PushBankCommand(mpc, 3).execute(); }
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

    if (id == Id::ERASE) { ReleaseEraseCommand(mpc).execute(); }
    else if (id == Id::F1) { ReleaseFunctionCommand(mpc, 0).execute(); }
    else if (id == Id::F3) { ReleaseFunctionCommand(mpc, 2).execute(); }
    else if (id == Id::F4) { ReleaseFunctionCommand(mpc, 3).execute(); }
    else if (id == Id::F5) { ReleaseFunctionCommand(mpc, 4).execute(); }
    else if (id == Id::F6) { ReleaseFunctionCommand(mpc, 5).execute(); }
    else if (id == Id::REC) { ReleaseRecCommand(mpc).execute(); }
    else if (id == Id::OVERDUB) { ReleaseOverdubCommand(mpc).execute(); }
    else if (id == Id::TAP_TEMPO_OR_NOTE_REPEAT) { ReleaseTapCommand(mpc).execute(); }
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
    // Our vmpc-juce wrapper's raw keyboard module automatically iterates
    // through all pressed keys and invokes key-up against them when the
    // app loses focus. We could disable that, and handle it like below.
    // But the advantage of relying the on the raw keyboard module's key-up
    // is that it takes care of all key-presses in one go, not just the ones
    // for pads.
    
    /*
    using Id = hardware::ComponentId;

    for (int physicalPadIndex = 0; physicalPadIndex < 16; ++physicalPadIndex)
    {
        if (!isPhysicallyPressedDueToKeyboardInput(physicalPadIndex))
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
    */
}

