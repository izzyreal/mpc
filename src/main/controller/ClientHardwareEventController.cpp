#include "controller/ClientHardwareEventController.hpp"
#include "sequencer/Transport.hpp"

#include "engine/EngineHost.hpp"
#include "command/AllCommands.hpp"
#include "command/context/NoteInputScreenUpdateContext.hpp"
#include "command/context/PushPadScreenUpdateContext.hpp"
#include "controller/ClientEventController.hpp"
#include "engine/PreviewSoundPlayer.hpp"
#include "eventregistry/EventTypes.hpp"
#include "hardware/ComponentId.hpp"
#include "hardware/Component.hpp"
#include "client/event/ClientHardwareEvent.hpp"
#include "command/context/TriggerLocalNoteContextFactory.hpp"
#include "Mpc.hpp"
#include "eventregistry/EventRegistry.hpp"
#include "hardware/Hardware.hpp"
#include "input/PadAndButtonKeyboard.hpp"
#include "lcdgui/ScreenGroups.hpp"
#include "lcdgui/ScreenComponent.hpp"
#include "lcdgui/screens/SequencerScreen.hpp"
#include "lcdgui/screens/StepEditorScreen.hpp"
#include "lcdgui/screens/VmpcKeyboardScreen.hpp"
#include "lcdgui/screens/VmpcMidiScreen.hpp"
#include "lcdgui/screens/dialog2/PopupScreen.hpp"
#include "lcdgui/screens/window/KeepOrRetryScreen.hpp"
#include "lcdgui/screens/window/LoadASoundScreen.hpp"
#include "lcdgui/screens/window/NameScreen.hpp"
#include "sampler/Program.hpp"
#include "sampler/Sampler.hpp"
#include "sequencer/Sequencer.hpp"

#include <memory>

using namespace mpc::controller;
using namespace mpc::input;
using namespace mpc::client::event;
using namespace mpc::hardware;
using namespace mpc::command;
using namespace mpc::command::context;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens::dialog2;
using namespace mpc::eventregistry;

ClientHardwareEventController::ClientHardwareEventController(Mpc &mpcToUse)
    : mpc(mpcToUse)
{
}

bool ClientHardwareEventController::isNoteRepeatLockedOrPressed() const
{
    return isNoteRepeatLocked() ||
           mpc.getHardware()->getButton(TAP_TEMPO_OR_NOTE_REPEAT)->isPressed();
}

void ClientHardwareEventController::handleClientHardwareEvent(
    const ClientHardwareEvent &event)
{
    if (event.source == ClientHardwareEvent::Source::HostInputKeyboard)
    {
        if (const auto nameScreen =
                std::dynamic_pointer_cast<NameScreen>(mpc.getScreen());
            nameScreen && event.textInputKey)
        {
            if (event.textInputKey->isPress)
            {
                nameScreen->typeCharacter(event.textInputKey->character);
                return;
            }
        }
    }

    switch (event.type)
    {
        case ClientHardwareEvent::Type::PadPress:
            handlePadPress(event);
            break;
        case ClientHardwareEvent::Type::PadAftertouch:
            handlePadAftertouch(event);
            break;
        case ClientHardwareEvent::Type::PadRelease:
            handlePadRelease(event);
            break;
        case ClientHardwareEvent::Type::DataWheelTurn:
            handleDataWheel(event);
            break;
        case ClientHardwareEvent::Type::SliderMove:
            handleSlider(event);
            break;
        case ClientHardwareEvent::Type::PotMove:
            handlePot(event);
            break;
        case ClientHardwareEvent::Type::ButtonPress:
            handleButtonPress(event);
            break;
        case ClientHardwareEvent::Type::ButtonRelease:
            handleButtonRelease(event);
            break;
        case ClientHardwareEvent::Type::ButtonDoublePress:
            handleButtonDoublePress(event);
            break;
        case ClientHardwareEvent::Type::ButtonPressAndRelease:
            handleButtonPress(event);
            handleButtonRelease(event);
            break;
        default:
            break;
    }
}

void ClientHardwareEventController::handlePadPress(
    const ClientHardwareEvent &event) const
{
    if (!event.index || !event.value)
    {
        return;
    }

    const auto layeredScreen = mpc.getLayeredScreen();

    if (layeredScreen->isCurrentScreen({ScreenId::NameScreen}) &&
        event.source == ClientHardwareEvent::Source::HostInputKeyboard)
    {
        return;
    }

    if (auto popupScreen = std::dynamic_pointer_cast<PopupScreen>(
            layeredScreen->getCurrentScreen());
        popupScreen &&
        popupScreen->isCloseUponButtonOrPadPressOrDataWheelTurnEnabled())
    {
        layeredScreen->closeCurrentScreen();
    }

    const auto screen = layeredScreen->getCurrentScreen();

    if (const auto opensNameScreen =
            std::dynamic_pointer_cast<OpensNameScreen>(screen);
        opensNameScreen)
    {
        opensNameScreen->openNameScreen();

        if (layeredScreen->isCurrentScreen({ScreenId::NameScreen}) &&
            event.source == ClientHardwareEvent::Source::HostInputKeyboard)
        {
            return;
        }
    }

    const auto physicalPadIndex = *event.index;

    const auto velocity =
        *event.value * static_cast<float>(VelocitySensitivePressable::MAX_VELO);

    const auto clampedVelocity = std::clamp(
        velocity, static_cast<float>(VelocitySensitivePressable::MIN_VELO),
        static_cast<float>(VelocitySensitivePressable::MAX_VELO));

    if (!mpc.getHardware()
             ->getPad(physicalPadIndex)
             ->pressWithVelocity(clampedVelocity))
    {
        return;
    }

    const auto track = mpc.getSequencer()->getActiveTrack();

    const auto program = screen->getProgram();

    const Bank activeBank = mpc.clientEventController->getActiveBank();
    const auto programPadIndex =
        physicalPadIndex + (static_cast<int>(activeBank) * 16);

    std::optional<int> note = std::nullopt;

    if (program)
    {
        constexpr std::optional<MidiChannel> noMidiChannel = std::nullopt;

        mpc.eventRegistry->registerProgramPadPress(
            Source::VirtualMpcHardware, screen, screen->getBus(), program,
            programPadIndex, clampedVelocity, track.get(), noMidiChannel);

        note = program->getNoteFromPad(programPadIndex);
    }

    const bool isF4Pressed = mpc.getHardware()->getButton(F4)->isPressed();
    const bool isF6Pressed = mpc.getHardware()->getButton(F6)->isPressed();

    PushPadScreenUpdateContext padPushScreenUpdateCtx{
        mpc.clientEventController->isSixteenLevelsEnabled(),
        screen,
        program,
        mpc.getSequencer(),
        isF4Pressed,
        isF6Pressed,
        mpc.clientEventController->getActiveBank(),
        [clientEventController = mpc.clientEventController](int p)
        {
            clientEventController->setSelectedPad(p);
        },
        screengroups::isCentralNoteAndPadUpdateScreen(screen)};

    PushPadScreenUpdateCommand(padPushScreenUpdateCtx, programPadIndex)
        .execute();

    NoteOnEventPtr registryNoteOnEvent;

    if (note)
    {
        NoteInputScreenUpdateContext noteInputScreenUpdateContext{
            mpc.clientEventController->isSixteenLevelsEnabled(),
            screengroups::isCentralNoteAndPadUpdateScreen(screen), screen,
            [clientEventController = mpc.clientEventController](int n)
            {
                clientEventController->setSelectedNote(n);
            },
            screen->isFocusedFieldName("fromnote")};

        NoteInputScreenUpdateCommand(noteInputScreenUpdateContext, *note)
            .execute();

        if (*note >= 0)
        {
            constexpr std::optional<MidiChannel> noMidiChannel = std::nullopt;
            registryNoteOnEvent = mpc.eventRegistry->registerNoteOn(
                Source::VirtualMpcHardware, screen, screen->getBus(), *note,
                clampedVelocity, track.get(), noMidiChannel, program,
                [](void *) {});
        }
    }

    std::function action = [](void *) {};

    if (layeredScreen->isCurrentScreen({ScreenId::NameScreen}))
    {
        mpc.getPadAndButtonKeyboard()->pressHardwareComponent(
            event.componentId);
    }
    else if (screengroups::isSoundScreen(screen))
    {
        mpc.getEngineHost()->getPreviewSoundPlayer()->playSound(
            sampler::PLAYX_SOUND, 127, 0);
    }
    else if (!screengroups::isPadDoesNotTriggerNoteEventScreen(screen))
    {
        if (program)
        {
            auto ctx =
                TriggerLocalNoteContextFactory::buildTriggerLocalNoteOnContext(
                    Source::VirtualMpcHardware, registryNoteOnEvent.get(),
                    program->getNoteFromPad(programPadIndex), clampedVelocity,
                    track.get(), screen->getBus(), screen, programPadIndex,
                    program, mpc.getSequencer(),
                    mpc.getEngineHost()->getSequencerPlaybackEngine(),
                    mpc.eventRegistry, mpc.clientEventController,
                    mpc.getEventHandler(), mpc.screens, mpc.getHardware());

            action = [ctx](void *)
            {
                TriggerLocalNoteOnCommand(ctx).execute();
            };
        }
    }

    mpc.eventRegistry->registerPhysicalPadPress(
        Source::VirtualMpcHardware, screen, screen->getBus(), physicalPadIndex,
        clampedVelocity, track.get(), static_cast<int>(activeBank), note,
        action);
}

void ClientHardwareEventController::handlePadRelease(
    const ClientHardwareEvent &event) const
{
    if (!event.index)
    {
        return;
    }

    if (mpc.getLayeredScreen()->isCurrentScreen({ScreenId::NameScreen}) &&
        event.source == ClientHardwareEvent::Source::HostInputKeyboard)
    {
        return;
    }

    const auto physicalPadIndex = *event.index;

    if (!mpc.getHardware()->getPad(physicalPadIndex)->release())
    {
        return;
    }

    auto action =
        [eventRegistry = mpc.eventRegistry,
         eventHandler = mpc.getEventHandler(), screens = mpc.screens,
         sequencer = mpc.getSequencer(), hardware = mpc.getHardware(),
         clientEventController = mpc.clientEventController,
         sequencerPlaybackEngine = mpc.getEngineHost()->getSequencerPlaybackEngine(),
         previewSoundPlayer =
             mpc.getEngineHost()->getPreviewSoundPlayer()](void *userData)
    {
        const auto p = static_cast<PhysicalPadPressEvent *>(userData);

        if (screengroups::isSoundScreen(p->screen))
        {
            previewSoundPlayer->finishVoiceIfSoundIsLooping();
        }
        else if (screengroups::isPadDoesNotTriggerNoteEventScreen(p->screen))
        {
            return;
        }

        const auto programPadIndex =
            p->padIndex.get() + static_cast<int>(p->bank) * 16;

        if (p->note)
        {
            const auto ctx =
                TriggerLocalNoteContextFactory::buildTriggerLocalNoteOffContext(
                    Source::VirtualMpcHardware, *p->note, p->track, p->bus,
                    p->screen, programPadIndex, p->program, sequencer,
                    sequencerPlaybackEngine, eventRegistry, clientEventController,
                    eventHandler, screens, hardware);

            TriggerLocalNoteOffCommand(ctx).execute();

            if (*p->note >= 0)
            {
                constexpr std::optional<int> noMidiChannel = std::nullopt;
                eventRegistry->registerNoteOff(Source::VirtualMpcHardware,
                                               *p->note, noMidiChannel,
                                               [](void *) {});
            }
        }

        if (p->program)
        {
            constexpr std::optional<int> noMidiChannel = std::nullopt;
            eventRegistry->registerProgramPadRelease(
                Source::VirtualMpcHardware, p->bus, p->program, programPadIndex,
                p->track, noMidiChannel, [](void *) {});
        }
    };

    mpc.eventRegistry->registerPhysicalPadRelease(
        physicalPadIndex, Source::VirtualMpcHardware, action);
}

void ClientHardwareEventController::handlePadAftertouch(
    const ClientHardwareEvent &event) const
{
    if (!event.index || !event.value)
    {
        return;
    }

    const auto padIndex = *event.index;
    const auto pressureToUse =
        std::clamp(*event.value * Aftertouchable::MAX_PRESSURE,
                   static_cast<float>(Aftertouchable::MIN_PRESSURE),
                   static_cast<float>(Aftertouchable::MAX_PRESSURE));

    mpc.getHardware()->getPad(padIndex)->aftertouch(pressureToUse);

    const std::function action =
        [eventRegistry = mpc.eventRegistry, pressureToUse](void *userData)
    {
        const auto padPress = static_cast<PhysicalPadPressEvent *>(userData);

        if (padPress->program)
        {
            eventRegistry->registerProgramPadAftertouch(
                Source::VirtualMpcHardware, padPress->bus, padPress->program,
                padPress->padIndex.get() + (padPress->bank * 16), pressureToUse,
                padPress->track);
        }

        if (padPress->note)
        {
            eventRegistry->registerNoteAftertouch(Source::VirtualMpcHardware,
                                                  *padPress->note,
                                                  pressureToUse, std::nullopt);
        }
    };

    mpc.eventRegistry->registerPhysicalPadAftertouch(
        padIndex, pressureToUse, Source::VirtualMpcHardware, action);
}

void ClientHardwareEventController::handleDataWheel(
    const ClientHardwareEvent &event)
{
    if (!event.deltaValue)
    {
        return;
    }

    int steps;

    if (event.source == ClientHardwareEvent::Source::HostInputKeyboard)
    {
        steps = static_cast<int>(*event.deltaValue);
    }
    else
    {
        float &acc = deltaAccumulators[event.componentId];
        acc += *event.deltaValue;
        steps = static_cast<int>(acc);
        acc -= steps;
    }

    if (steps == 0)
    {
        return;
    }

    mpc.getHardware()->getDataWheel()->turn(steps);

    const auto screen = mpc.getScreen();

    if (auto popupScreen = std::dynamic_pointer_cast<PopupScreen>(screen);
        popupScreen &&
        popupScreen->isCloseUponButtonOrPadPressOrDataWheelTurnEnabled())
    {
        mpc.getLayeredScreen()->closeCurrentScreen();
        return;
    }

    screen->turnWheel(steps);

    if (const auto opensNameScreen =
            std::dynamic_pointer_cast<OpensNameScreen>(screen);
        opensNameScreen)
    {
        opensNameScreen->openNameScreen();
    }
}

void ClientHardwareEventController::handleSlider(
    const ClientHardwareEvent &event) const
{
    const auto slider = mpc.getHardware()->getSlider();

    if (event.value)
    {
        slider->moveToNormalizedY(*event.value);
        mpc.getScreen()->setSlider(std::round(slider->getValue()));
    }
    else if (event.deltaValue && *event.deltaValue != 0.f)
    {
        slider->setValue(slider->getValue() + *event.deltaValue);
        mpc.getScreen()->setSlider(std::round(slider->getValue()));
    }
}

void ClientHardwareEventController::handlePot(
    const ClientHardwareEvent &event) const
{
    const std::shared_ptr<Pot> pot = event.componentId == REC_GAIN_POT
                                         ? mpc.getHardware()->getRecPot()
                                         : mpc.getHardware()->getVolPot();

    const auto engineHost = mpc.getEngineHost();

    pot->setValue(pot->getValue() + *event.deltaValue * 0.01f);

    if (event.componentId == REC_GAIN_POT)
    {
        engineHost->setRecordLevel(std::round(pot->getValue() * 100.f));
    }
    else
    {
        engineHost->setMainLevel(std::round(pot->getValue() * 100.f));
    }
}

void ClientHardwareEventController::handleButtonPress(
    const ClientHardwareEvent &event) const
{
    const auto button = mpc.getHardware()->getButton(event.componentId);

    // The below check is necessary because the keyboard mapping routines in
    // mpc::event may return labels like "ctrl" and "alt" rather than component
    // labels. After we've improved the keyboard event handling, we can remove
    // this check.
    if (!button)
    {
        return;
    }

    // Temporary hack. We actually want to synthesize repeat events ourselves,
    // so we don't depend on host-generated repeats. This way the behaviour is
    // the same for keyboard, mouse, touch and MIDI event.
    static const auto allowRepeat = std::vector{
        CURSOR_UP, CURSOR_RIGHT_OR_DIGIT, CURSOR_DOWN, CURSOR_LEFT_OR_DIGIT};

    if (!button->press() && std::find(allowRepeat.begin(), allowRepeat.end(),
                                      event.componentId) == allowRepeat.end())
    {
        return;
    }

    const auto screen = mpc.getScreen();

    const auto layeredScreen = mpc.getLayeredScreen();

    if (auto popupScreen = std::dynamic_pointer_cast<PopupScreen>(screen);
        popupScreen)
    {
        if (popupScreen->isCloseUponButtonOrPadPressOrDataWheelTurnEnabled())
        {
            layeredScreen->closeCurrentScreen();
        }
        else
        {
            return;
        }
    }

    const auto id = event.componentId;

    if (const auto stepEditorScreen =
            std::dynamic_pointer_cast<StepEditorScreen>(screen);
        stepEditorScreen)
    {
        if (id == PREV_STEP_OR_EVENT)
        {
            stepEditorScreen->prevStepEvent();
            return;
        }
        else if (id == NEXT_STEP_OR_EVENT)
        {
            stepEditorScreen->nextStepEvent();
            return;
        }
        else if (id == PREV_BAR_START)
        {
            stepEditorScreen->prevBarStart();
            return;
        }
        else if (id == NEXT_BAR_END)
        {
            stepEditorScreen->nextBarEnd();
            return;
        }
    }

    if (id == CURSOR_LEFT_OR_DIGIT)
    {
        screen->left();
    }
    else if (id == CURSOR_RIGHT_OR_DIGIT)
    {
        screen->right();
    }
    else if (id == CURSOR_UP)
    {
        screen->up();
    }
    else if (id == CURSOR_DOWN)
    {
        screen->down();
    }
    else if (id == REC)
    {
        screen->rec();
    }
    else if (id == OVERDUB)
    {
        screen->overDub();
    }
    else if (id == STOP)
    {
        screen->stop();
    }
    else if (id == PLAY)
    {
        screen->play();
    }
    else if (id == PLAY_START)
    {
        screen->playStart();
    }
    else if (id == MAIN_SCREEN)
    {
        if (screengroups::isScreenThatIsNotAllowedToOpenMainScreen(screen))
        {
            return;
        }

        if (const auto vmpcKeyboardScreen =
                std::dynamic_pointer_cast<VmpcKeyboardScreen>(screen);
            vmpcKeyboardScreen)
        {
            if (vmpcKeyboardScreen->hasMappingChanged())
            {
                layeredScreen->openScreenById(
                    ScreenId::VmpcDiscardMappingChangesScreen);
                return;
            }
        }
        else if (const auto vmpcMidiScreen =
                     std::dynamic_pointer_cast<VmpcMidiScreen>(screen);
                 vmpcMidiScreen)
        {
            if (vmpcMidiScreen->hasMappingChanged())
            {
                layeredScreen->openScreenById(
                    ScreenId::VmpcDiscardMappingChangesScreen);
                return;
            }
        }
        else if (const auto loadASoundScreen =
                     std::dynamic_pointer_cast<LoadASoundScreen>(screen);
                 loadASoundScreen)
        {
            mpc.getSampler()->deleteSound(mpc.getSampler()->getPreviewSound());
        }
        else if (const auto nameScreen =
                     std::dynamic_pointer_cast<NameScreen>(screen);
                 nameScreen)
        {
            nameScreen->mainScreenAction();
        }
        else if (const auto keepOrRetryScreen =
                     std::dynamic_pointer_cast<KeepOrRetryScreen>(screen);
                 keepOrRetryScreen)
        {
            mpc.getSampler()->deleteSound(mpc.getSampler()->getPreviewSound());
        }

        PushMainScreenCommand(mpc).execute();
    }
    else if (id == OPEN_WINDOW)
    {
        screen->openWindow();
    }
    else if (id == GO_TO)
    {
        PushGoToCommand(mpc).execute();
    }
    else if (id == TAP_TEMPO_OR_NOTE_REPEAT)
    {
        PushTapCommand(mpc).execute();

        if (const auto sequencerScreen =
                std::dynamic_pointer_cast<SequencerScreen>(screen);
            sequencerScreen)
        {
            // Pure UI update
            sequencerScreen->tap();
        }
    }
    else if (id == NEXT_SEQ)
    {
        PushNextSeqCommand(mpc).execute();
    }
    else if (id == TRACK_MUTE)
    {
        PushTrackMuteCommand(mpc).execute();
    }
    else if (id == FULL_LEVEL_OR_CASE_SWITCH)
    {
        PushFullLevelCommand(layeredScreen, mpc.getPadAndButtonKeyboard(),
                             mpc.getHardware(), mpc.clientEventController)
            .execute();
    }
    else if (id == SIXTEEN_LEVELS_OR_SPACE)
    {
        PushSixteenLevelsCommand(layeredScreen, mpc.clientEventController,
                                 mpc.getHardware())
            .execute();
    }
    else if (id == F1)
    {
        screen->function(0);
    }
    else if (id == F2)
    {
        screen->function(1);
    }
    else if (id == F3)
    {
        screen->function(2);
    }
    else if (id == F4)
    {
        screen->function(3);
    }
    else if (id == F5)
    {
        screen->function(4);
    }
    else if (id == F6)
    {
        screen->function(5);
    }
    else if (id == SHIFT)
    {
        PushShiftCommand(mpc).execute();

        if (const auto stepEditorScreen =
                std::dynamic_pointer_cast<StepEditorScreen>(screen);
            stepEditorScreen)
        {
            // Start/reset event selection and update UI
            stepEditorScreen->shift();
        }
    }
    else if (id == ENTER_OR_SAVE)
    {
        screen->pressEnter();
    }
    else if (id == UNDO_SEQ)
    {
        PushUndoSeqCommand(mpc).execute();
    }
    else if (id == ERASE)
    {
        PushEraseCommand(mpc).execute();

        if (const auto sequencerScreen =
                std::dynamic_pointer_cast<SequencerScreen>(screen);
            sequencerScreen)
        {
            if (mpc.getSequencer()->getTransport()->isOverdubbing())
            {
                // Pure UI update
                sequencerScreen->erase();
            }
        }
    }
    else if (id == AFTER_OR_ASSIGN)
    {
        PushAfterCommand(mpc.clientEventController, layeredScreen,
                         mpc.getHardware())
            .execute();
    }
    else if (id == BANK_A)
    {
        PushBankCommand(mpc.clientEventController, Bank::A).execute();
    }
    else if (id == BANK_B)
    {
        PushBankCommand(mpc.clientEventController, Bank::B).execute();
    }
    else if (id == BANK_C)
    {
        PushBankCommand(mpc.clientEventController, Bank::C).execute();
    }
    else if (id == BANK_D)
    {
        PushBankCommand(mpc.clientEventController, Bank::D).execute();
    }
    else if (id == NUM_0_OR_VMPC)
    {
        screen->numpad(0);
    }
    else if (id == NUM_1_OR_SONG)
    {
        screen->numpad(1);
    }
    else if (id == NUM_2_OR_MISC)
    {
        screen->numpad(2);
    }
    else if (id == NUM_3_OR_LOAD)
    {
        screen->numpad(3);
    }
    else if (id == NUM_4_OR_SAMPLE)
    {
        screen->numpad(4);
    }
    else if (id == NUM_5_OR_TRIM)
    {
        screen->numpad(5);
    }
    else if (id == NUM_6_OR_PROGRAM)
    {
        screen->numpad(6);
    }
    else if (id == NUM_7_OR_MIXER)
    {
        screen->numpad(7);
    }
    else if (id == NUM_8_OR_OTHER)
    {
        screen->numpad(8);
    }
    else if (id == NUM_9_OR_MIDI_SYNC)
    {
        screen->numpad(9);
    }
}

void ClientHardwareEventController::handleButtonRelease(
    const ClientHardwareEvent &event) const
{
    const auto button = mpc.getHardware()->getButton(event.componentId);

    // The below check is necessary because the keyboard mapping routines in
    // mpc::event may return labels like "ctrl" and "alt" rather than component
    // labels. After we've improved the keyboard event handling, we can remove
    // this check.
    if (!button)
    {
        return;
    }

    if (!button->release())
    {
        return;
    }

    if (const auto id = event.componentId; id == ERASE)
    {
        ReleaseEraseCommand(mpc).execute();
    }
    else if (id == F1)
    {
        ReleaseFunctionCommand(mpc, 0).execute();
    }
    else if (id == F3)
    {
        ReleaseFunctionCommand(mpc, 2).execute();
    }
    else if (id == F4)
    {
        ReleaseFunctionCommand(mpc, 3).execute();
    }
    else if (id == F5)
    {
        ReleaseFunctionCommand(mpc, 4).execute();
    }
    else if (id == F6)
    {
        ReleaseFunctionCommand(mpc, 5).execute();
    }
    else if (id == REC)
    {
        ReleaseRecCommand(mpc).execute();
    }
    else if (id == OVERDUB)
    {
        ReleaseOverdubCommand(mpc).execute();
    }
    else if (id == TAP_TEMPO_OR_NOTE_REPEAT)
    {
        ReleaseTapCommand(mpc).execute();
    }
}

void ClientHardwareEventController::handleButtonDoublePress(
    const ClientHardwareEvent &event)
{
    const auto button = mpc.getHardware()->getButton(event.componentId);

    if (event.componentId == REC || event.componentId == OVERDUB)
    {
        if (!button->doublePress())
        {
            return;
        }

        if (event.componentId == REC)
        {
            buttonLockTracker.toggle(event.componentId);

            if (buttonLockTracker.isLocked(REC))
            {
                buttonLockTracker.unlock(OVERDUB);
            }
        }
        else if (event.componentId == OVERDUB)
        {
            buttonLockTracker.toggle(OVERDUB);

            if (buttonLockTracker.isLocked(OVERDUB))
            {
                buttonLockTracker.unlock(REC);
            }
        }
    }
    else
    {
        handleButtonPress(event);
    }
}
