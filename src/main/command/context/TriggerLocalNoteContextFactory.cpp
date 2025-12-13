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

#include "sequencer/Sequencer.hpp"
#include "sequencer/Bus.hpp"
#include "sequencer/SeqUtil.hpp"

#include "performance/PerformanceManager.hpp"
#include "sequencer/SequencerStateManager.hpp"

using namespace mpc::command::context;
using namespace mpc::controller;
using namespace mpc::lcdgui;
using namespace mpc::sequencer;
using namespace mpc::hardware;
using namespace mpc::performance;
using namespace mpc::audiomidi;
using namespace mpc::engine;

TriggerLocalNoteOnContext
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
    const std::shared_ptr<Hardware> &hardware,
    const Tick metronomeOnlyPositionTicks, const Tick positionTicks)
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
        SeqUtil::isStepRecording(screen->getName(), lockedSequencer.get());

    const bool isRecMainWithoutPlaying = controller->isRecMainWithoutPlaying();

    const auto timingCorrectScreen =
        screens->get<ScreenId::TimingCorrectScreen>();
    const auto assign16LevelsScreen =
        screens->get<ScreenId::Assign16LevelsScreen>();

    const auto hardwareSliderValue = hardware->getSlider()->getValueAs<int>();

    const auto drumScreenSelectedDrum =
        screens->get<ScreenId::DrumScreen>()->getDrum();

    const auto transportSnapshot = lockedSequencer->getStateManager()
                                       ->getSnapshot()
                                       .getTransportStateView();

    return TriggerLocalNoteOnContext{
        source,
        performanceManager.lock().get(),
        lockedSequencer->getStateManager().get(),
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
        bus.get(),
        program.get(),
        note,
        drumScreenSelectedDrum,
        isSamplerScreen,
        track,
        sequencer.lock().get(),
        timingCorrectScreen.get(),
        assign16LevelsScreen.get(),
        eventHandler.get(),
        allowCentralNoteAndPadUpdate,
        screen.get(),
        controller->setSelectedNote,
        controller->setSelectedPad,
        hardwareSliderValue,
        metronomeOnlyPositionTicks,
        positionTicks};
}

TriggerLocalNoteOffContext
TriggerLocalNoteContextFactory::buildTriggerLocalNoteOffContext(
    const PerformanceEventSource source, const NoteNumber noteNumber,
    EventData *recordedNoteOnEvent, Track *track, const BusType busType,
    const ScreenComponent *screen, const bool isSamplerScreen,
    const ProgramPadIndex programPadIndex, sampler::Program *program,
    Sequencer *sequencer, PerformanceManager *performanceManager,
    const ClientEventController *controller, EventHandler *eventHandler,
    Screens *screens, Hardware *hardware, const Tick metronomeOnlyPositionTicks,
    const Tick positionTicks)
{
    const auto stepEditOptionsScreen =
        screens->get<ScreenId::StepEditOptionsScreen>();
    const auto timingCorrectScreen =
        screens->get<ScreenId::TimingCorrectScreen>();

    const std::function sequencerMoveToQuarterNotePosition =
        [sequencer = sequencer](const double quarterNotePosition)
    {
        sequencer->getTransport()->setPosition(quarterNotePosition);
    };

    const std::function sequencerStopMetronomeTrack = [sequencer = sequencer]
    {
        sequencer->getTransport()->stopMetronomeOnly();
    };

    const bool isStepRecording =
        SeqUtil::isStepRecording(screen->getName(), sequencer);

    const bool isRecMainWithoutPlaying = controller->isRecMainWithoutPlaying();

    const auto transportSnapshot =
        sequencer->getStateManager()->getSnapshot().getTransportStateView();

    return TriggerLocalNoteOffContext{
        source,
        performanceManager,
        sequencer->getStateManager().get(),
        sequencer->getBus(busType).get(),
        program,
        programPadIndex,
        isSamplerScreen,
        noteNumber,
        eventHandler,
        recordedNoteOnEvent,
        transportSnapshot.isRecordingOrOverdubbing(),
        hardware->getButton(ERASE)->isPressed(),
        track,
        isStepRecording,
        metronomeOnlyPositionTicks,
        isRecMainWithoutPlaying,
        positionTicks,
        stepEditOptionsScreen->getTcValuePercentage(),
        timingCorrectScreen->getNoteValueLengthInTicks(),
        stepEditOptionsScreen->isDurationOfRecordedNotesTcValue(),
        stepEditOptionsScreen->isAutoStepIncrementEnabled(),
        sequencer->getTransport()->getCurrentBarIndex(),
        timingCorrectScreen->getSwing(),
        sequencer,
        positionTicks};
}
