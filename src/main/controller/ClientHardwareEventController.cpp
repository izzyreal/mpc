#include "controller/ClientHardwareEventController.hpp"
#include "sequencer/Transport.hpp"

#include "engine/EngineHost.hpp"
#include "command/AllCommands.hpp"
#include "command/context/NoteInputScreenUpdateContext.hpp"
#include "command/context/PushPadScreenUpdateContext.hpp"
#include "controller/ClientEventController.hpp"
#include "engine/PreviewSoundPlayer.hpp"
#include "performance/EventTypes.hpp"
#include "hardware/ComponentId.hpp"
#include "hardware/Component.hpp"
#include "client/event/ClientHardwareEvent.hpp"
#include "command/context/TriggerLocalNoteContextFactory.hpp"
#include "Mpc.hpp"
#include "performance/PerformanceManager.hpp"
#include "hardware/Hardware.hpp"
#include "input/PadAndButtonKeyboard.hpp"
#include "lcdgui/ScreenGroups.hpp"
#include "lcdgui/ScreenIdGroups.hpp"
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
#include "sequencer/Sequence.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/SequencerStateManager.hpp"
#include "sequencer/Track.hpp"

#include "utils/TimeUtils.hpp"

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
using namespace mpc::performance;

ClientHardwareEventController::ClientHardwareEventController(Mpc &mpcToUse)
    : mpc(mpcToUse)
{
    buttonConsumptionTracker.addConsumptionRule(GO_TO, PREV_STEP_OR_EVENT);
    buttonConsumptionTracker.addConsumptionRule(GO_TO, NEXT_STEP_OR_EVENT);
    buttonConsumptionTracker.addConsumptionRule(GO_TO, PREV_BAR_START);
    buttonConsumptionTracker.addConsumptionRule(GO_TO, NEXT_BAR_END);
}

bool ClientHardwareEventController::isNoteRepeatLockedOrPressed() const
{
    return isNoteRepeatLocked() ||
           mpc.getHardware()->getButton(TAP_TEMPO_OR_NOTE_REPEAT)->isPressed();
}

bool ClientHardwareEventController::isRecLockedOrPressed() const
{
    return isRecLocked() || isRecPressed();
}

bool ClientHardwareEventController::isRecPressed() const
{
    return mpc.getHardware()->getButton(REC)->isPressed();
}

bool ClientHardwareEventController::isOverdubLockedOrPressed() const
{
    return isOverdubLocked() || isOverdubPressed();
}

bool ClientHardwareEventController::isOverdubPressed() const
{
    return mpc.getHardware()->getButton(OVERDUB)->isPressed();
}

mpc::TimeInMilliseconds
ClientHardwareEventController::getMostRecentPhysicalPadPressTime() const
{
    return mostRecentPhysicalPadPressTimeMs.load();
}

mpc::Velocity
ClientHardwareEventController::getMostRecentPhysicalPadPressVelocity() const
{
    return mostRecentPhysicalPadPressVelocity.load();
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
        case ClientHardwareEvent::Type::MpcButtonPress:
            handleButtonPress(event);
            break;
        case ClientHardwareEvent::Type::MpcButtonRelease:
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

void ClientHardwareEventController::updateMostRecentPhysicalPadPressVelocity(
    const Velocity newVelocity)
{
    if (newVelocity <= 0)
    {
        return;
    }

    mostRecentPhysicalPadPressVelocity.store(newVelocity);
    mostRecentPhysicalPadPressTimeMs.store(utils::nowInMilliseconds());
}

void ClientHardwareEventController::handlePadPress(
    const ClientHardwareEvent &event)
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
    const auto screenId = mpc.getLayeredScreen()->getCurrentScreenId();

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

    updateMostRecentPhysicalPadPressVelocity(Velocity(clampedVelocity));

    const auto track = mpc.getSequencer()->getSelectedTrack();

    const auto programIndex = screen->getProgramIndex();
    const auto program = screen->getProgram();

    const Bank activeBank = mpc.clientEventController->getActiveBank();
    const auto programPadIndex =
        physicalPadIndex + static_cast<int>(activeBank) * 16;

    std::optional<NoteNumber> note = std::nullopt;

    if (program)
    {
        assert(programIndex);
        constexpr std::optional<MidiChannel> noMidiChannel = std::nullopt;

        mpc.getPerformanceManager().lock()->registerProgramPadPress(
            PerformanceEventSource::VirtualMpcHardware, noMidiChannel, screenId,
            track->getIndex(), screen->getBus()->busType,
            ProgramPadIndex(programPadIndex), Velocity(clampedVelocity),
            *programIndex, PhysicalPadIndex(physicalPadIndex));

        note = NoteNumber(
            program->getNoteFromPad(ProgramPadIndex(programPadIndex)));
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
        [clientEventController =
             mpc.clientEventController](const ProgramPadIndex p)
        {
            clientEventController->setSelectedPad(p);
        },
        screengroups::isCentralNoteAndPadUpdateScreen(screen)};

    PushPadScreenUpdateCommand(padPushScreenUpdateCtx, programPadIndex)
        .execute();

    std::optional<NoteOnEvent> registryNoteOnEvent = std::nullopt;

    if (note)
    {
        NoteInputScreenUpdateContext noteInputScreenUpdateContext{
            mpc.clientEventController->isSixteenLevelsEnabled(),
            screengroups::isCentralNoteAndPadUpdateScreen(screen), screen,
            [clientEventController =
                 mpc.clientEventController](const DrumNoteNumber n)
            {
                clientEventController->setSelectedNote(n);
            },
            screen->isFocusedFieldName("fromnote")};

        NoteInputScreenUpdateCommand(noteInputScreenUpdateContext, *note)
            .execute();

        if (*note >= 0)
        {
            constexpr std::optional<MidiChannel> noMidiChannel = std::nullopt;
            registryNoteOnEvent =
                mpc.getPerformanceManager().lock()->registerNoteOn(
                    PerformanceEventSource::VirtualMpcHardware, noMidiChannel,
                    screenId, track->getIndex(), screen->getBus()->busType,
                    *note, Velocity(clampedVelocity), programIndex);
        }
    }

    std::optional<utils::SimpleAction> action = std::nullopt;

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
        const auto transport = mpc.getSequencer()
                                   ->getStateManager()
                                   ->getSnapshot()
                                   .getTransportStateView();

        const bool isNoteRepeatMode = screenId == ScreenId::SequencerScreen &&
                                      isNoteRepeatLockedOrPressed() &&
                                      transport.isSequencerRunning();

        const bool isLiveEraseMode =
            transport.isRecordingOrOverdubbing() &&
            mpc.clientEventController->isEraseButtonPressed();

        if (program && registryNoteOnEvent && !isNoteRepeatMode &&
            !isLiveEraseMode)
        {
            const auto metronomeOnlyPositionTicks =
                transport.getMetronomeOnlyPositionTicks();

            const auto positionTicks = transport.getPositionTicks();

            action = utils::SimpleAction(
                [noteEventInfo = *registryNoteOnEvent, clampedVelocity,
                 tr = track.get(), screen, program, programPadIndex, this,
                 metronomeOnlyPositionTicks, positionTicks]
                {
                    const DrumNoteNumber drumNoteNumber =
                        program->getNoteFromPad(
                            ProgramPadIndex(programPadIndex));

                    const auto ctx = TriggerLocalNoteContextFactory::
                        buildTriggerLocalNoteOnContext(
                            PerformanceEventSource::VirtualMpcHardware,
                            noteEventInfo, drumNoteNumber,
                            Velocity(clampedVelocity), tr, screen->getBus(),
                            screen, ProgramPadIndex(programPadIndex), program,
                            mpc.getSequencer(), mpc.getPerformanceManager(),
                            mpc.clientEventController, mpc.getEventHandler(),
                            mpc.screens, mpc.getHardware(),
                            metronomeOnlyPositionTicks, positionTicks);

                    TriggerLocalNoteOnCommand(ctx).execute();
                });
        }
    }

    mpc.getPerformanceManager().lock()->registerPhysicalPadPress(
        PerformanceEventSource::VirtualMpcHardware, screenId,
        screen->getBus()->busType, PhysicalPadIndex(physicalPadIndex),
        Velocity(clampedVelocity), track->getIndex(), activeBank,
        screen->getProgramIndex(), note);

    if (action)
    {
        mpc.getPerformanceManager().lock()->enqueueCallback(std::move(*action));
    }
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

    const auto transport = mpc.getSequencer()
                               ->getStateManager()
                               ->getSnapshot()
                               .getTransportStateView();

    const auto metronomeOnlyPositionTicks =
        transport.getMetronomeOnlyPositionTicks();
    const auto positionTicks = transport.getPositionTicks();

    std::optional<utils::SimpleAction> action = std::nullopt;

    const std::optional<PhysicalPadPressEvent> physicalPadPressEvent =
        mpc.getPerformanceManager().lock()->getSnapshot().findPhysicalPadPress(
            PhysicalPadIndex(physicalPadIndex));

    if (physicalPadPressEvent)
    {
        action = utils::SimpleAction(
            [this, p = *physicalPadPressEvent, metronomeOnlyPositionTicks,
             positionTicks]
            {
                const auto performanceManager = mpc.getPerformanceManager();
                const auto sampler = mpc.getSampler();
                const auto eventHandler = mpc.getEventHandler();
                const auto screens = mpc.screens;
                const auto sequencer = mpc.getSequencer();
                const auto hardware = mpc.getHardware();
                const auto clientEventController = mpc.clientEventController;
                const auto previewSoundPlayer =
                    mpc.getEngineHost()->getPreviewSoundPlayer();

                if (screengroups::isSoundScreen(p.screenId))
                {
                    previewSoundPlayer->finishVoiceIfSoundIsLooping();
                }
                else if (screengroups::isPadDoesNotTriggerNoteEventScreen(
                             p.screenId))
                {
                    return;
                }

                const auto programPadIndex =
                    physicalPadAndBankToProgramPadIndex(p.padIndex, p.bank);

                const auto programPadPressEvent =
                    performanceManager.lock()
                        ->getSnapshot()
                        .findProgramPadPress(
                            PerformanceEventSource::VirtualMpcHardware,
                            p.padIndex, p.programIndex);

                const auto quantizedLockActivated =
                    programPadPressEvent.has_value() &&
                    programPadPressEvent->quantizedLockActivated;

                if (p.noteNumber != NoNoteNumber && !quantizedLockActivated)
                {
                    const auto selectedSequence =
                        sequencer->getSelectedSequence();

                    const auto recordingNoteOnEvent =
                        sequencer->getStateManager()->findRecordingNoteOnEvent(
                            selectedSequence->getSequenceIndex(), p.trackIndex,
                            p.noteNumber);

                    NoteOffEvent msg{
                        p.noteNumber, NoMidiChannel,
                        PerformanceEventSource::VirtualMpcHardware};

                    performanceManager.lock()->applyMessageImmediate(
                        std::move(msg));

                    const bool isSamplerScreen =
                        screengroups::isSamplerScreen(p.screenId);

                    auto screenComponent =
                        screens->getScreenById(p.screenId).get();

                    const auto track =
                        selectedSequence->getTrack(p.trackIndex).get();

                    utils::SimpleAction noteOffAction(
                        [this, recordingNoteOnEvent, track, p, isSamplerScreen,
                         screenComponent, programPadIndex,
                         program = sampler->getProgram(p.programIndex).get(),
                         metronomeOnlyPositionTicks, positionTicks]
                        {
                            const auto ctx = TriggerLocalNoteContextFactory::
                                buildTriggerLocalNoteOffContext(
                                    PerformanceEventSource::VirtualMpcHardware,
                                    p.noteNumber, recordingNoteOnEvent, track,
                                    p.busType, screenComponent, isSamplerScreen,
                                    programPadIndex, program,
                                    mpc.getSequencer().get(),
                                    mpc.getPerformanceManager().lock().get(),
                                    mpc.clientEventController.get(),
                                    mpc.getEventHandler().get(),
                                    mpc.screens.get(), mpc.getHardware().get(),
                                    metronomeOnlyPositionTicks, positionTicks);

                            TriggerLocalNoteOffCommand(ctx).execute();
                        });

                    performanceManager.lock()->enqueueCallback(
                        std::move(noteOffAction));
                }

                if (p.programIndex != NoProgramIndex)
                {
                    performanceManager.lock()->registerProgramPadRelease(
                        PerformanceEventSource::VirtualMpcHardware,
                        programPadIndex, p.programIndex);
                }
            });
    }

    mpc.getPerformanceManager().lock()->registerPhysicalPadRelease(
        PhysicalPadIndex(physicalPadIndex));

    if (action)
    {
        mpc.getPerformanceManager().lock()->enqueueCallback(std::move(*action));
    }
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

    const std::optional<PhysicalPadPressEvent> padPressEvent =
        mpc.getPerformanceManager().lock()->getSnapshot().findPhysicalPadPress(
            PhysicalPadIndex(padIndex));

    std::optional<utils::SimpleAction> action = std::nullopt;

    if (padPressEvent)
    {
        action = utils::SimpleAction(
            [performanceManager = mpc.getPerformanceManager(), pressureToUse,
             padPress = *padPressEvent]
            {
                if (padPress.programIndex != NoProgramIndex)
                {
                    performanceManager.lock()->registerProgramPadAftertouch(
                        PerformanceEventSource::VirtualMpcHardware,
                        physicalPadAndBankToProgramPadIndex(padPress.padIndex,
                                                            padPress.bank),
                        padPress.programIndex, Pressure(pressureToUse));
                }

                if (padPress.noteNumber != NoNoteNumber)
                {
                    performanceManager.lock()->registerNoteAftertouch(
                        PerformanceEventSource::VirtualMpcHardware,
                        padPress.noteNumber, Pressure(pressureToUse),
                        std::nullopt);
                }
            });
    }

    mpc.getPerformanceManager().lock()->registerPhysicalPadAftertouch(
        PhysicalPadIndex(padIndex), Pressure(pressureToUse),
        PerformanceEventSource::VirtualMpcHardware);

    if (action)
    {
        mpc.getPerformanceManager().lock()->enqueueCallback(std::move(*action));
    }
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

    if (const auto popupScreen = std::dynamic_pointer_cast<PopupScreen>(screen);
        popupScreen &&
        popupScreen->isCloseUponButtonOrPadPressOrDataWheelTurnEnabled())
    {
        mpc.getLayeredScreen()->closeCurrentScreen();
        return;
    }

    if (const auto focusedField = mpc.getLayeredScreen()->getFocusedField();
        focusedField && focusedField->isTypeModeEnabled())
    {
        focusedField->disableTypeMode();
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
    const ClientHardwareEvent &event)
{
    const auto button = mpc.getHardware()->getButton(event.componentId);

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

    buttonConsumptionTracker.onPress(event.componentId);

    const auto screen = mpc.getScreen();

    const auto layeredScreen = mpc.getLayeredScreen();

    if (const auto popupScreen = std::dynamic_pointer_cast<PopupScreen>(screen);
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

    if (const auto withLocate =
            std::dynamic_pointer_cast<WithLocateStepEventBarSequence>(screen);
        withLocate)
    {
        if (id == PREV_STEP_OR_EVENT)
        {
            withLocate->prevStepEvent();
        }
        else if (id == NEXT_STEP_OR_EVENT)
        {
            withLocate->nextStepEvent();
        }
        else if (id == PREV_BAR_START)
        {
            withLocate->prevBarStart();
        }
        else if (id == NEXT_BAR_END)
        {
            withLocate->nextBarEnd();
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
    const ClientHardwareEvent &event)
{
    const auto button = mpc.getHardware()->getButton(event.componentId);

    if (!button->release())
    {
        return;
    }

    buttonConsumptionTracker.onRelease(event.componentId);

    if (buttonConsumptionTracker.isConsumed(event.componentId))
    {
        buttonConsumptionTracker.reset(event.componentId);
        return;
    }

    if (const auto id = event.componentId; id == ERASE)
    {
        ReleaseEraseCommand(mpc).execute();
    }
    else if (id == GO_TO)
    {
        ReleaseGoToCommand(mpc).execute();
    }
    else if (id == REC)
    {
        const auto positionToReturnTo =
            mpc.getSequencer()
                ->getStateManager()
                ->getSnapshot()
                .getTransportStateView()
                .getPositionTicksToReturnToWhenReleasingRec();

        if (positionToReturnTo != NoTick)
        {
            mpc.getSequencer()
                ->getTransport()
                ->setPositionTicksToReturnToWhenReleasingRec(NoTick);
            mpc.getSequencer()->getTransport()->setPosition(
                sequencer::Sequencer::ticksToQuarterNotes(positionToReturnTo));
        }
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
        else /*if (event.componentId == OVERDUB)*/
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
