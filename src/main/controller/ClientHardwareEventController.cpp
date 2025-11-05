#include "controller/ClientHardwareEventController.hpp"

#include "audiomidi/AudioMidiServices.hpp"
#include "command/AllCommands.hpp"
#include "command/context/NoteInputScreenUpdateContext.hpp"
#include "command/context/PushPadScreenUpdateContext.hpp"
#include "controller/ClientEventController.hpp"
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
#include "sequencer/Bus.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Track.hpp"

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

ClientHardwareEventController::ClientHardwareEventController(mpc::Mpc &mpcToUse)
    : mpc(mpcToUse)
{
}

bool ClientHardwareEventController::isNoteRepeatLockedOrPressed()
{
    return isNoteRepeatLocked() ||
           mpc.getHardware()
               ->getButton(ComponentId::TAP_TEMPO_OR_NOTE_REPEAT)
               ->isPressed();
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
    const ClientHardwareEvent &event)
{
    if (!event.index || !event.value)
    {
        return;
    }

    auto layeredScreen = mpc.getLayeredScreen();

    if (layeredScreen->isCurrentScreen<ScreenId::NameScreen>() &&
        event.source == ClientHardwareEvent::Source::HostInputKeyboard)
    {
        return;
    }

    auto screen = layeredScreen->getCurrentScreen();

    if (auto opensNameScreen =
            std::dynamic_pointer_cast<OpensNameScreen>(screen);
        opensNameScreen)
    {
        opensNameScreen->openNameScreen();

        if (event.source == ClientHardwareEvent::Source::HostInputKeyboard)
        {
            return;
        }
    }

    const auto physicalPadIndex = *event.index;

    const auto velocity =
        *event.value * (float)VelocitySensitivePressable::MAX_VELO;

    const auto clampedVelocity =
        std::clamp(velocity, (float)VelocitySensitivePressable::MIN_VELO,
                   (float)VelocitySensitivePressable::MAX_VELO);

    auto pad = mpc.getHardware()->getPad(physicalPadIndex);

    if (!pad->pressWithVelocity(clampedVelocity))
    {
        return;
    }

    const Bank activeBank = mpc.clientEventController->getActiveBank();

    auto track = mpc.getSequencer()->getActiveTrack();

    const auto program = screen->getProgram();
    const auto programPadIndex =
        physicalPadIndex + (static_cast<int>(activeBank) * 16);

    std::optional<int> note = std::nullopt;

    if (program)
    {
        if (auto programNote = program->getNoteFromPad(programPadIndex);
            programNote >= 35)
        {
            note = programNote;
        }
    }

    auto ctx = TriggerLocalNoteContextFactory::buildTriggerDrumNoteOnContext(
        Source::VirtualMpcHardware, layeredScreen, mpc.clientEventController,
        mpc.getHardware(), mpc.getSequencer(), mpc.screens, mpc.getSampler(),
        mpc.eventRegistry, mpc.getEventHandler(),
        mpc.getSequencer()->getFrameSequencer(), &mpc.getBasicPlayer(),
        programPadIndex, clampedVelocity, screen);

    const bool isF4Pressed =
        mpc.getHardware()->getButton(ComponentId::F4)->isPressed();
    const bool isF6Pressed =
        mpc.getHardware()->getButton(ComponentId::F6)->isPressed();

    PushPadScreenUpdateContext padPushScreenUpdateCtx{
        ctx->isSixteenLevelsEnabled,
        ctx->screenComponent,
        ctx->program,
        ctx->sequencer,
        isF4Pressed,
        isF6Pressed,
        mpc.clientEventController->getActiveBank(),
        ctx->setSelectedPad,
        ctx->allowCentralNoteAndPadUpdate};

    PushPadScreenUpdateCommand(padPushScreenUpdateCtx, programPadIndex)
        .execute();

    NoteInputScreenUpdateContext noteInputScreenUpdateContext{
        ctx->isSixteenLevelsEnabled, ctx->allowCentralNoteAndPadUpdate,
        ctx->screenComponent, ctx->setSelectedNote, ctx->currentFieldName};

    NoteInputScreenUpdateCommand(noteInputScreenUpdateContext, ctx->note)
        .execute();

    std::function<void(void *)> action = [](void *) {};

    if (layeredScreen->isCurrentScreen<ScreenId::PopupScreen>())
    {
        layeredScreen->closeCurrentScreen();
    }

    if (layeredScreen->isCurrentScreen<ScreenId::NameScreen>())
    {
        mpc.getPadAndButtonKeyboard()->pressHardwareComponent(
            event.componentId);
    }
    else if (!screengroups::isPadDoesNotTriggerNoteEventScreen(screen))
    {
        action = [ctx](void *)
        {
            TriggerLocalNoteOnCommand(ctx).execute();
        };
    }

    mpc.eventRegistry->registerPhysicalPadPress(
        Source::VirtualMpcHardware, screen,
        mpc.getSequencer()->getBus<sequencer::Bus>(track->getBus()),
        physicalPadIndex, clampedVelocity, track.get(),
        static_cast<int>(activeBank), note, action);
}

void ClientHardwareEventController::handlePadRelease(
    const ClientHardwareEvent &event)
{
    if (!event.index)
    {
        return;
    }

    if (mpc.getLayeredScreen()->isCurrentScreen<ScreenId::NameScreen>() &&
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
         previewSoundPlayer = &mpc.getBasicPlayer(),
         eventHandler = mpc.getEventHandler(), screens = mpc.screens,
         sequencer = mpc.getSequencer(), hardware = mpc.getHardware(),
         clientEventController = mpc.clientEventController,
         frameSequencer =
             mpc.getSequencer()->getFrameSequencer()](void *userData)
    {
        auto p = (PhysicalPadPressEvent *)userData;

        if (screengroups::isPadDoesNotTriggerNoteEventScreen(p->screen))
        {
            return;
        }

        const auto programPadIndex =
            p->padIndex.get() + static_cast<int>(p->bank) * 16;

        std::optional<int> drumIndex = std::nullopt;

        if (auto drumBus =
                std::dynamic_pointer_cast<sequencer::DrumBus>(p->bus);
            drumBus)
        {
            drumIndex = drumBus->getIndex();
        }

        if (p->note)
        {
            assert(drumIndex);

            auto ctx =
                TriggerLocalNoteContextFactory::buildTriggerDrumNoteOffContext(
                    Source::VirtualMpcHardware, previewSoundPlayer,
                    eventRegistry, eventHandler, screens, sequencer, hardware,
                    clientEventController, frameSequencer, programPadIndex,
                    *drumIndex, p->screen, *p->note, p->program, p->track);

            TriggerLocalNoteOffCommand(ctx).execute();
        }
    };

    mpc.eventRegistry->registerPhysicalPadRelease(
        physicalPadIndex, Source::VirtualMpcHardware, action);
}

void ClientHardwareEventController::handlePadAftertouch(
    const ClientHardwareEvent &event)
{
    if (!event.index || !event.value)
    {
        return;
    }

    const auto padIndex = *event.index;
    const auto pressureToUse =
        std::clamp(*event.value * Aftertouchable::MAX_PRESSURE,
                   (float)Aftertouchable::MIN_PRESSURE,
                   (float)Aftertouchable::MAX_PRESSURE);

    mpc.getHardware()->getPad(padIndex)->aftertouch(pressureToUse);

    std::function<void(void *)> action =
        [eventRegistry = mpc.eventRegistry, pressureToUse](void *userData)
    {
        auto padPress = (PhysicalPadPressEvent *)userData;

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

    auto screen = mpc.getScreen();

    screen->turnWheel(steps);

    if (auto opensNameScreen =
            std::dynamic_pointer_cast<OpensNameScreen>(screen);
        opensNameScreen)
    {
        opensNameScreen->openNameScreen();
    }
}

void ClientHardwareEventController::handleSlider(
    const ClientHardwareEvent &event)
{
    auto slider = mpc.getHardware()->getSlider();

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

void ClientHardwareEventController::handlePot(const ClientHardwareEvent &event)
{
    std::shared_ptr<Pot> pot = event.componentId == ComponentId::REC_GAIN_POT
                                   ? mpc.getHardware()->getRecPot()
                                   : mpc.getHardware()->getVolPot();

    auto audioMidiServices = mpc.getAudioMidiServices();

    pot->setValue(pot->getValue() + *event.deltaValue * 0.01f);

    if (event.componentId == ComponentId::REC_GAIN_POT)
    {
        audioMidiServices->setRecordLevel(std::round(pot->getValue() * 100.f));
    }
    else
    {
        audioMidiServices->setMainLevel(std::round(pot->getValue() * 100.f));
    }
}

void ClientHardwareEventController::handleButtonPress(
    const ClientHardwareEvent &event)
{
    auto button = mpc.getHardware()->getButton(event.componentId);

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
    static const auto allowRepeat = std::vector<ComponentId>{
        ComponentId::CURSOR_UP, ComponentId::CURSOR_RIGHT_OR_DIGIT,
        ComponentId::CURSOR_DOWN, ComponentId::CURSOR_LEFT_OR_DIGIT};

    if (!button->press() && std::find(allowRepeat.begin(), allowRepeat.end(),
                                      event.componentId) == allowRepeat.end())
    {
        return;
    }

    auto screen = mpc.getScreen();
    auto layeredScreen = mpc.getLayeredScreen();

    using Id = ComponentId;

    const auto id = event.componentId;

    if (auto stepEditorScreen =
            std::dynamic_pointer_cast<StepEditorScreen>(screen);
        stepEditorScreen)
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

    if (id == Id::CURSOR_LEFT_OR_DIGIT)
    {
        screen->left();
    }
    else if (id == Id::CURSOR_RIGHT_OR_DIGIT)
    {
        screen->right();
    }
    else if (id == Id::CURSOR_UP)
    {
        screen->up();
    }
    else if (id == Id::CURSOR_DOWN)
    {
        screen->down();
    }
    else if (id == Id::REC)
    {
        screen->rec();
    }
    else if (id == Id::OVERDUB)
    {
        screen->overDub();
    }
    else if (id == Id::STOP)
    {
        screen->stop();
    }
    else if (id == Id::PLAY)
    {
        screen->play();
    }
    else if (id == Id::PLAY_START)
    {
        screen->playStart();
    }
    else if (id == Id::MAIN_SCREEN)
    {
        if (screengroups::isScreenThatIsNotAllowedToOpenMainScreen(screen))
        {
            return;
        }

        if (auto vmpcKeyboardScreen =
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
        else if (auto vmpcMidiScreen =
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
        else if (auto loadASoundScreen =
                     std::dynamic_pointer_cast<LoadASoundScreen>(screen);
                 loadASoundScreen)
        {
            mpc.getSampler()->deleteSound(mpc.getSampler()->getPreviewSound());
        }
        else if (auto nameScreen =
                     std::dynamic_pointer_cast<NameScreen>(screen);
                 nameScreen)
        {
            nameScreen->mainScreenAction();
        }
        else if (auto keepOrRetryScreen =
                     std::dynamic_pointer_cast<KeepOrRetryScreen>(screen);
                 keepOrRetryScreen)
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

        if (auto sequencerScreen =
                std::dynamic_pointer_cast<SequencerScreen>(screen);
            sequencerScreen)
        {
            // Pure UI update
            sequencerScreen->tap();
        }
    }
    else if (id == Id::NEXT_SEQ)
    {
        PushNextSeqCommand(mpc).execute();
    }
    else if (id == Id::TRACK_MUTE)
    {
        PushTrackMuteCommand(mpc).execute();
    }
    else if (id == Id::FULL_LEVEL_OR_CASE_SWITCH)
    {
        PushFullLevelCommand(layeredScreen, mpc.getPadAndButtonKeyboard(),
                             mpc.getHardware(), mpc.clientEventController)
            .execute();
    }
    else if (id == Id::SIXTEEN_LEVELS_OR_SPACE)
    {
        PushSixteenLevelsCommand(layeredScreen, mpc.clientEventController,
                                 mpc.getHardware())
            .execute();
    }
    else if (id == Id::F1)
    {
        screen->function(0);
    }
    else if (id == Id::F2)
    {
        screen->function(1);
    }
    else if (id == Id::F3)
    {
        screen->function(2);
    }
    else if (id == Id::F4)
    {
        screen->function(3);
    }
    else if (id == Id::F5)
    {
        screen->function(4);
    }
    else if (id == Id::F6)
    {
        screen->function(5);
    }
    else if (id == Id::SHIFT)
    {
        PushShiftCommand(mpc).execute();

        if (auto stepEditorScreen =
                std::dynamic_pointer_cast<StepEditorScreen>(screen);
            stepEditorScreen)
        {
            // Start/reset event selection and update UI
            stepEditorScreen->shift();
        }
    }
    else if (id == Id::ENTER_OR_SAVE)
    {
        screen->pressEnter();
    }
    else if (id == Id::UNDO_SEQ)
    {
        PushUndoSeqCommand(mpc).execute();
    }
    else if (id == Id::ERASE)
    {
        PushEraseCommand(mpc).execute();

        if (auto sequencerScreen =
                std::dynamic_pointer_cast<SequencerScreen>(screen);
            sequencerScreen)
        {
            if (mpc.getSequencer()->isOverdubbing())
            {
                // Pure UI update
                sequencerScreen->erase();
            }
        }
    }
    else if (id == Id::AFTER_OR_ASSIGN)
    {
        PushAfterCommand(mpc.clientEventController, layeredScreen,
                         mpc.getHardware())
            .execute();
    }
    else if (id == Id::BANK_A)
    {
        PushBankCommand(mpc.clientEventController, Bank::A).execute();
    }
    else if (id == Id::BANK_B)
    {
        PushBankCommand(mpc.clientEventController, Bank::B).execute();
    }
    else if (id == Id::BANK_C)
    {
        PushBankCommand(mpc.clientEventController, Bank::C).execute();
    }
    else if (id == Id::BANK_D)
    {
        PushBankCommand(mpc.clientEventController, Bank::D).execute();
    }
    else if (id == Id::NUM_0_OR_VMPC)
    {
        screen->numpad(0);
    }
    else if (id == Id::NUM_1_OR_SONG)
    {
        screen->numpad(1);
    }
    else if (id == Id::NUM_2_OR_MISC)
    {
        screen->numpad(2);
    }
    else if (id == Id::NUM_3_OR_LOAD)
    {
        screen->numpad(3);
    }
    else if (id == Id::NUM_4_OR_SAMPLE)
    {
        screen->numpad(4);
    }
    else if (id == Id::NUM_5_OR_TRIM)
    {
        screen->numpad(5);
    }
    else if (id == Id::NUM_6_OR_PROGRAM)
    {
        screen->numpad(6);
    }
    else if (id == Id::NUM_7_OR_MIXER)
    {
        screen->numpad(7);
    }
    else if (id == Id::NUM_8_OR_OTHER)
    {
        screen->numpad(8);
    }
    else if (id == Id::NUM_9_OR_MIDI_SYNC)
    {
        screen->numpad(9);
    }
}

void ClientHardwareEventController::handleButtonRelease(
    const ClientHardwareEvent &event)
{
    auto button = mpc.getHardware()->getButton(event.componentId);

    // The below check is necessary because the keyboard mapping routines in
    // mpc::event may return labels like "ctrl" and "alt" rather than component
    // labels. After we've improved the keyboard event handling, we can remove
    // this check.
    if (!button)
    {
        return;
    }

    button->release();

    using Id = ComponentId;

    auto id = event.componentId;

    if (id == Id::ERASE)
    {
        ReleaseEraseCommand(mpc).execute();
    }
    else if (id == Id::F1)
    {
        ReleaseFunctionCommand(mpc, 0).execute();
    }
    else if (id == Id::F3)
    {
        ReleaseFunctionCommand(mpc, 2).execute();
    }
    else if (id == Id::F4)
    {
        ReleaseFunctionCommand(mpc, 3).execute();
    }
    else if (id == Id::F5)
    {
        ReleaseFunctionCommand(mpc, 4).execute();
    }
    else if (id == Id::F6)
    {
        ReleaseFunctionCommand(mpc, 5).execute();
    }
    else if (id == Id::REC)
    {
        ReleaseRecCommand(mpc).execute();
    }
    else if (id == Id::OVERDUB)
    {
        ReleaseOverdubCommand(mpc).execute();
    }
    else if (id == Id::TAP_TEMPO_OR_NOTE_REPEAT)
    {
        ReleaseTapCommand(mpc).execute();
    }
}

void ClientHardwareEventController::handleButtonDoublePress(
    const ClientHardwareEvent &event)
{
    auto button = mpc.getHardware()->getButton(event.componentId);

    if (event.componentId == ComponentId::REC ||
        event.componentId == ComponentId::OVERDUB)
    {
        if (!button->doublePress())
        {
            return;
        }

        if (event.componentId == ComponentId::REC)
        {
            buttonLockTracker.toggle(event.componentId);

            if (buttonLockTracker.isLocked(ComponentId::REC))
            {
                buttonLockTracker.unlock(ComponentId::OVERDUB);
            }
        }
        else if (event.componentId == ComponentId::OVERDUB)
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
        handleButtonPress(event);
    }
}
