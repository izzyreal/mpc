#include "command/context/TriggerLocalNoteContextFactory.hpp"

#include "sequencer/Transport.hpp"

#include "controller/ClientEventController.hpp"
#include "controller/ClientHardwareEventController.hpp"
#include "lcdgui/ScreenGroups.hpp"

#include "hardware/Hardware.hpp"
#include "lcdgui/screens/SequencerScreen.hpp"
#include "lcdgui/screens/DrumScreen.hpp"
#include "lcdgui/screens/window/StepEditOptionsScreen.hpp"
#include "lcdgui/screens/window/TimingCorrectScreen.hpp"
#include "lcdgui/screens/window/Assign16LevelsScreen.hpp"

#include "sequencer/Sequence.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Bus.hpp"
#include "sequencer/SeqUtil.hpp"

#include "performance/PerformanceManager.hpp"
#include "sequencer/SequencerStateManager.hpp"
#include "sequencer/Track.hpp"

using namespace mpc::command::context;
using namespace mpc::controller;
using namespace mpc::lcdgui;
using namespace mpc::sequencer;
using namespace mpc::hardware;
using namespace mpc::performance;
using namespace mpc::audiomidi;
using namespace mpc::engine;

std::shared_ptr<TriggerLocalNoteOnContext>
TriggerLocalNoteContextFactory::buildTriggerLocalNoteOnContext(
    const PerformanceEventSource source,
    const performance::NoteOnEvent &registryNoteOnEvent, const NoteNumber note,
    const Velocity velocity, Track *track, const std::shared_ptr<Bus> &bus,
    const std::shared_ptr<ScreenComponent> &screen,
    const ProgramPadIndex programPadIndex,
    const std::shared_ptr<sampler::Program> &program,
    const std::weak_ptr<Sequencer> &sequencer,
    const std::weak_ptr<PerformanceManager> &performanceManager,
    const std::shared_ptr<ClientEventController> &controller,
    const std::shared_ptr<EventHandler> &eventHandler,
    const std::shared_ptr<Screens> &screens,
    const std::shared_ptr<Hardware> &hardware)
{
    const bool isSequencerScreen =
        std::dynamic_pointer_cast<SequencerScreen>(screen) != nullptr;
    const bool isSamplerScreen = screengroups::isSamplerScreen(screen);
    const bool allowCentralNoteAndPadUpdate =
        screengroups::isCentralNoteAndPadUpdateScreen(screen);
    const bool isFullLevelEnabled = controller->isFullLevelEnabled();
    const bool isSixteenLevelsEnabled = controller->isSixteenLevelsEnabled();
    const bool isNoteRepeatLockedOrPressed =
        controller->clientHardwareEventController->isNoteRepeatLocked() ||
        hardware->getButton(TAP_TEMPO_OR_NOTE_REPEAT)->isPressed();
    const bool isErasePressed = hardware->getButton(ERASE)->isPressed();

    const auto lockedSequencer = sequencer.lock();

    const bool isStepRecording =
        SeqUtil::isStepRecording(screen->getName(), lockedSequencer);

    const bool isRecMainWithoutPlaying = SeqUtil::isRecMainWithoutPlaying(
        lockedSequencer, screens->get<ScreenId::TimingCorrectScreen>(),
        screen->getName(), hardware->getButton(REC),
        controller->clientHardwareEventController);

    const auto timingCorrectScreen =
        screens->get<ScreenId::TimingCorrectScreen>();
    const auto assign16LevelsScreen =
        screens->get<ScreenId::Assign16LevelsScreen>();

    const std::function setSelectedNote = [controller](const DrumNoteNumber n)
    {
        controller->setSelectedNote(n);
    };

    const std::function setSelectedPad = [controller](const ProgramPadIndex p)
    {
        controller->setSelectedPad(p);
    };

    const auto hardwareSliderValue = hardware->getSlider()->getValueAs<int>();

    const auto drumScreenSelectedDrum =
        screens->get<ScreenId::DrumScreen>()->getDrum();

    const auto transportSnapshot = lockedSequencer->getStateManager()
                                   ->getSnapshot()
                                   .getTransportStateView();

    return std::make_shared<TriggerLocalNoteOnContext>(
        TriggerLocalNoteOnContext{source,
                                  performanceManager,
                                  registryNoteOnEvent,
                                  isSequencerScreen,
                                  programPadIndex,
                                  velocity,
                                  isFullLevelEnabled,
                                  isSixteenLevelsEnabled,
                                  isNoteRepeatLockedOrPressed,
                                  isErasePressed,
                                  isStepRecording,
                                  isRecMainWithoutPlaying,
                                  transportSnapshot.isRecordingOrOverdubbing(),
                                  bus,
                                  program,
                                  note,
                                  drumScreenSelectedDrum,
                                  isSamplerScreen,
                                  track,
                                  sequencer,
                                  timingCorrectScreen,
                                  assign16LevelsScreen,
                                  eventHandler,
                                  allowCentralNoteAndPadUpdate,
                                  screen,
                                  setSelectedNote,
                                  setSelectedPad,
                                  hardwareSliderValue,
                                  transportSnapshot.getMetronomeOnlyPositionTicks()});
}

std::shared_ptr<TriggerLocalNoteOffContext>
TriggerLocalNoteContextFactory::buildTriggerLocalNoteOffContext(
    const PerformanceEventSource source, const NoteNumber noteNumber,
    const EventData *recordedNoteOnEvent, Track *track, const BusType busType,
    const std::shared_ptr<ScreenComponent> &screen,
    const ProgramPadIndex programPadIndex,
    const std::shared_ptr<sampler::Program> &program,
    const std::weak_ptr<Sequencer> &sequencer,
    const std::weak_ptr<PerformanceManager> &performanceManager,
    const std::shared_ptr<ClientEventController> &controller,
    const std::shared_ptr<EventHandler> &eventHandler,
    const std::shared_ptr<Screens> &screens,
    const std::shared_ptr<Hardware> &hardware)
{
    const bool isSamplerScreen = screengroups::isSamplerScreen(screen);

    EventData *sequencerRecordNoteOnEvent = nullptr;
    ;

    if (recordedNoteOnEvent)
    {
        sequencerRecordNoteOnEvent =
            track->findRecordingNoteOnEvent(recordedNoteOnEvent);
    }

    const auto stepEditOptionsScreen =
        screens->get<ScreenId::StepEditOptionsScreen>();
    const auto timingCorrectScreen =
        screens->get<ScreenId::TimingCorrectScreen>();

    const std::function getActiveSequenceLastTick = [sequencer]
    {
        return sequencer.lock()->getSelectedSequence()->getLastTick();
    };

    const std::function sequencerMoveToQuarterNotePosition =
        [sequencer = sequencer](const double quarterNotePosition)
    {
        sequencer.lock()->getTransport()->setPosition(quarterNotePosition);
    };

    const std::function sequencerStopMetronomeTrack = [sequencer = sequencer]
    {
        sequencer.lock()->getTransport()->stopMetronomeOnly();
    };

    const auto lockedSequencer = sequencer.lock();

    const bool isStepRecording =
        SeqUtil::isStepRecording(screen->getName(), lockedSequencer);

    const bool isRecMainWithoutPlaying = SeqUtil::isRecMainWithoutPlaying(
        lockedSequencer, screens->get<ScreenId::TimingCorrectScreen>(),
        screen->getName(), hardware->getButton(REC),
        controller->clientHardwareEventController);

    const auto transportSnapshot = lockedSequencer->getStateManager()
                                       ->getSnapshot()
                                       .getTransportStateView();

    return std::make_shared<TriggerLocalNoteOffContext>(
        TriggerLocalNoteOffContext{
            source,
            performanceManager,
            lockedSequencer->getBus(busType),
            program,
            programPadIndex,
            isSamplerScreen,
            noteNumber,
            eventHandler,
            sequencerRecordNoteOnEvent,
            transportSnapshot.isRecordingOrOverdubbing(),
            hardware->getButton(ERASE)->isPressed(),
            track,
            isStepRecording,
            transportSnapshot.getMetronomeOnlyPositionTicks(),
            isRecMainWithoutPlaying,
            transportSnapshot.getPositionTicks(),
            stepEditOptionsScreen->getTcValuePercentage(),
            timingCorrectScreen->getNoteValueLengthInTicks(),
            stepEditOptionsScreen->isDurationOfRecordedNotesTcValue(),
            stepEditOptionsScreen->isAutoStepIncrementEnabled(),
            lockedSequencer->getTransport()->getCurrentBarIndex(),
            timingCorrectScreen->getSwing(),
            getActiveSequenceLastTick,
            sequencerMoveToQuarterNotePosition,
            sequencerStopMetronomeTrack});
}
