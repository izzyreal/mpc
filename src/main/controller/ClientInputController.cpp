#include "controller/ClientInputController.hpp"
#include "audiomidi/AudioMidiServices.hpp"
#include "command/TriggerDrumNoteOffCommand.hpp"
#include "command/context/NoteInputScreenUpdateContext.hpp"
#include "command/context/PushPadScreenUpdateContext.hpp"
#include "hardware/ComponentId.hpp"
#include "hardware/HardwareComponent.hpp"
#include "input/ClientInput.hpp"
#include "command/context/TriggerDrumContextFactory.hpp"
#include "Mpc.hpp"
#include "input/PadAndButtonKeyboard.hpp"
#include "lcdgui/ScreenGroups.hpp"
#include "lcdgui/ScreenComponent.hpp"
#include "lcdgui/screens/SequencerScreen.hpp"
#include "lcdgui/screens/StepEditorScreen.hpp"
#include "lcdgui/screens/VmpcKeyboardScreen.hpp"
#include "lcdgui/screens/VmpcMidiScreen.hpp"
#include "lcdgui/screens/dialog2/PopupScreen.hpp"
#include "hardware/Hardware.hpp"
#include "lcdgui/screens/window/KeepOrRetryScreen.hpp"
#include "lcdgui/screens/window/LoadASoundScreen.hpp"
#include "lcdgui/screens/window/NameScreen.hpp"
#include <memory>

using namespace mpc::controller;
using namespace mpc::input;
using namespace mpc::hardware;
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

void ClientInputController::handleInput(const ClientInput &input)
{
    switch (input.type) {
        case ClientInput::Type::PadPress:
            handlePadPress(input);
            break;
        case ClientInput::Type::PadAftertouch:
            handlePadAftertouch(input);
            break;
        case ClientInput::Type::PadRelease:
            handlePadRelease(input);
            break;
        case ClientInput::Type::DataWheelTurn:
            handleDataWheel(input);
            break;
        case ClientInput::Type::SliderMove:
            handleSlider(input);
            break;
        case ClientInput::Type::PotMove:
            handlePot(input);
            break;
        case ClientInput::Type::ButtonPress:
            handleButtonPress(input);
            break;
        case ClientInput::Type::ButtonRelease:
            handleButtonRelease(input);
            break;
        case ClientInput::Type::ButtonDoublePress:
            handleButtonDoublePress(input);
            break;
        case ClientInput::Type::ButtonPressAndRelease:
            handleButtonPress(input);
            handleButtonRelease(input);
            break;
        case ClientInput::Type::HostFocusLost:
            handleFocusLost();
        default:
            break;
    }
}

void ClientInputController::handlePadPress(const ClientInput &input)
{
    if (!input.index || !input.value) return;

    const auto physicalPadIndex = *input.index;

    const auto velocity = *input.value * (float) VelocitySensitivePressable::MAX_VELO;

    const auto clampedVelocity = std::clamp(velocity,
                                          (float) VelocitySensitivePressable::MIN_VELO,
                                          (float) VelocitySensitivePressable::MAX_VELO);

    auto pad = mpc.getHardware()->getPad(physicalPadIndex);

    if (!pad->pressWithVelocity(clampedVelocity))
    {
        return;
    }

    auto layeredScreen = mpc.getLayeredScreen();
    auto screen = layeredScreen->getCurrentScreen();

    registerPhysicalPadPush(physicalPadIndex, mpc.getBank(), screen, input.source);

    if (input.source == ClientInput::Source::HostInputKeyboard)
    {
        if (const auto nameScreen = std::dynamic_pointer_cast<NameScreen>(mpc.getScreen()); nameScreen &&
                input.textInputKey)
        {
            if (input.textInputKey->isPress)
            {   
                nameScreen->typeCharacter(input.textInputKey->character);
                return;
            }
        }
    }

    if (std::dynamic_pointer_cast<NameScreen>(screen))
    {
        mpc.getPadAndButtonKeyboard()->pressHardwareComponent(input.componentId);
        return;
    }

    const auto programPadIndex = physicalPadIndex + (mpc.getBank() * 16);
    auto ctx = TriggerDrumContextFactory::buildTriggerDrumNoteOnContext(mpc, programPadIndex, clampedVelocity, screen);

    const bool isF4Pressed = mpc.getHardware()->getButton(ComponentId::F4)->isPressed();
    const bool isF6Pressed = mpc.getHardware()->getButton(ComponentId::F6)->isPressed();

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

    if (auto opensNameScreen = std::dynamic_pointer_cast<OpensNameScreen>(screen); opensNameScreen)
    {
        opensNameScreen->openNameScreen();
    }

    if (std::dynamic_pointer_cast<PopupScreen>(screen))
    {
        layeredScreen->closeCurrentScreen();
    }

    if (screengroups::isPadDoesNotTriggerNoteEventScreen(screen))
    {
        return;
    }

    TriggerDrumNoteOnCommand(ctx).execute();
}

void ClientInputController::handlePadRelease(const ClientInput &input)
{
    if (!input.index) return;

    const auto physicalPadIndex = *input.index;

    const auto info = registerPhysicalPadRelease(physicalPadIndex);

    mpc.getHardware()->getPad(physicalPadIndex)->release();

    if (screengroups::isPadDoesNotTriggerNoteEventScreen(info.screen))
    {
        return;
    }

    const auto programPadIndex = physicalPadIndex + (info.bankIndex * 16);
    auto ctx = TriggerDrumContextFactory::buildTriggerDrumNoteOffContext(mpc, programPadIndex, info.screen);
    TriggerDrumNoteOffCommand(ctx).execute();
}

void ClientInputController::handlePadAftertouch(const ClientInput &input)
{
    if (!input.index || !input.value) return;
    const auto padIndex = *input.index;
    const auto pressureToUse = std::clamp(*input.value * Aftertouchable::MAX_PRESSURE, (float) Aftertouchable::MIN_PRESSURE, (float) Aftertouchable::MAX_PRESSURE);
    mpc.getHardware()->getPad(padIndex)->aftertouch(pressureToUse);
}

void ClientInputController::handleDataWheel(const ClientInput &input)
{
    if (!input.deltaValue) return;

    int steps;

    if (input.source == ClientInput::Source::HostInputKeyboard)
    {
        steps = static_cast<int>(*input.deltaValue);
    }
    else
    {
        float& acc = deltaAccumulators[input.componentId];
        acc += *input.deltaValue;
        steps = static_cast<int>(acc);
        acc -= steps;
    }

    if (steps == 0)
    {
        return;
    }

    mpc.getHardware()->getDataWheel()->turn(steps);

    auto screen = mpc.getScreen();

    screen->turnWheel(steps);

    if (auto opensNameScreen = std::dynamic_pointer_cast<OpensNameScreen>(screen); opensNameScreen)
    {
        opensNameScreen->openNameScreen();
    }
}

void ClientInputController::handleSlider(const ClientInput &input)
{
    auto slider = mpc.getHardware()->getSlider();

    if (input.value)
    {
        slider->moveToNormalizedY(*input.value);
        mpc.getScreen()->setSlider(std::round(slider->getValue()));
    }
    else if (input.deltaValue && *input.deltaValue != 0.f)
    {
        slider->setValue(slider->getValue() + *input.deltaValue);
        mpc.getScreen()->setSlider(std::round(slider->getValue()));
    }
}

void ClientInputController::handlePot(const ClientInput &input)
{
    std::shared_ptr<Pot> pot =
        input.componentId == ComponentId::REC_GAIN_POT ? mpc.getHardware()->getRecPot() : mpc.getHardware()->getVolPot();

    auto audioMidiServices = mpc.getAudioMidiServices();

    pot->setValue(pot->getValue() + *input.deltaValue * 0.01f);

    if (input.componentId == ComponentId::REC_GAIN_POT)
    {
        audioMidiServices->setRecordLevel(std::round(pot->getValue() * 100.f));
    }
    else
    {
        audioMidiServices->setMainLevel(std::round(pot->getValue() * 100.f));
    }
}

void ClientInputController::handleButtonPress(const ClientInput &input)
{
    auto button = mpc.getHardware()->getButton(input.componentId);

    // The below check is necessary because the keyboard mapping routines in mpc::input may return
    // labels like "ctrl" and "alt" rather than component labels. After we've improved the keyboard
    // input handling, we can remove this check.
    if (!button)
    {
        return;
    }

    // Temporary hack. We actually want to synthesize repeat events ourselves, so we don't depend
    // on host-generated repeats. This way the behaviour is the same for keyboard, mouse, touch and
    // MIDI input.
    static const auto allowRepeat = std::vector<ComponentId>{
        ComponentId::CURSOR_UP, ComponentId::CURSOR_RIGHT_OR_DIGIT,
            ComponentId::CURSOR_DOWN, ComponentId::CURSOR_LEFT_OR_DIGIT
    };

    if (!button->press() && std::find(allowRepeat.begin(), allowRepeat.end(), input.componentId) == allowRepeat.end())
    {
        return;
    }

    if (input.source == ClientInput::Source::HostInputKeyboard)
    {
        if (const auto nameScreen = std::dynamic_pointer_cast<NameScreen>(mpc.getScreen()); nameScreen &&
                input.textInputKey)
        {
            if (input.textInputKey->isPress)
            {   
                nameScreen->typeCharacter(input.textInputKey->character);
                return;
            }
        }
    }

    auto screen = mpc.getScreen();
    auto layeredScreen = mpc.getLayeredScreen();

    using Id = ComponentId;

    const auto id = input.componentId;

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

    if (std::dynamic_pointer_cast<PopupScreen>(screen))
    {
        layeredScreen->closeCurrentScreen();
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
        if (screengroups::isScreenThatIsNotAllowedToOpenMainScreen(screen))
        {
            return;
        }

        if (auto vmpcKeyboardScreen = std::dynamic_pointer_cast<VmpcKeyboardScreen>(screen); vmpcKeyboardScreen)
        {
            if (vmpcKeyboardScreen->hasMappingChanged())
            {
                layeredScreen->openScreen<VmpcDiscardMappingChangesScreen>();
                return;
            }
        }
        else if (auto vmpcMidiScreen = std::dynamic_pointer_cast<VmpcMidiScreen>(screen); vmpcMidiScreen)
        {
            if (vmpcMidiScreen->hasMappingChanged())
            {
                layeredScreen->openScreen<VmpcDiscardMappingChangesScreen>();
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
        screen->openWindow();
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
    else if (id == Id::UNDO_SEQ) { PushUndoSeqCommand(mpc).execute(); }
    else if (id == Id::ERASE)
    {
        PushEraseCommand(mpc).execute();

        if (auto sequencerScreen = std::dynamic_pointer_cast<SequencerScreen>(screen); sequencerScreen)
        {
            if (mpc.getSequencer()->isOverdubbing())
            {
                // Pure UI update
                sequencerScreen->erase();
            }
        }
    }
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

void ClientInputController::handleButtonRelease(const ClientInput &input)
{
    auto button = mpc.getHardware()->getButton(input.componentId);

    // The below check is necessary because the keyboard mapping routines in mpc::input may return
    // labels like "ctrl" and "alt" rather than component labels. After we've improved the keyboard
    // input handling, we can remove this check.
    if (!button)
    {
        return;
    }

    button->release();

    using Id = ComponentId;

    auto id = input.componentId;

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

void ClientInputController::handleButtonDoublePress(const ClientInput &input)
{
    auto button = mpc.getHardware()->getButton(input.componentId);

    if (input.componentId == ComponentId::REC || input.componentId == ComponentId::OVERDUB)
    {
        if (!button->doublePress()) return;

        if (input.componentId == ComponentId::REC)
        {
            buttonLockTracker.toggle(input.componentId);

            if (buttonLockTracker.isLocked(ComponentId::REC))
            {
                buttonLockTracker.unlock(ComponentId::OVERDUB);
            }
        }
        else if (input.componentId == ComponentId::OVERDUB)
        {
            buttonLockTracker.toggle(ComponentId::OVERDUB);

            if (buttonLockTracker.isLocked(ComponentId::OVERDUB))
            {
                buttonLockTracker.unlock(ComponentId::REC);
            }
        }
    }
    else
    {
        handleButtonPress(input);
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
    using Id = ComponentId;

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

