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
#include "sequencer/FrameSeq.hpp"
#include "sequencer/SeqUtil.hpp"
#include "sequencer/NoteEvent.hpp"

#include "eventregistry/EventRegistry.hpp"

using namespace mpc::command::context;
using namespace mpc::controller;
using namespace mpc::lcdgui;
using namespace mpc::sequencer;
using namespace mpc::hardware;
using namespace mpc::sampler;
using namespace mpc::eventregistry;
using namespace mpc::audiomidi;

std::shared_ptr<TriggerLocalNoteOnContext>
TriggerLocalNoteContextFactory::buildTriggerLocalNoteOnContext(
    const Source source, eventregistry::NoteOnEvent *registryNoteOnEvent,
    const int note, const int velocity, Track *track,
    const std::shared_ptr<Bus> &bus,
    const std::shared_ptr<ScreenComponent> &screen,
    const std::optional<int> programPadIndex,
    const std::shared_ptr<Program> &program,
    const std::shared_ptr<Sequencer> &sequencer,
    const std::shared_ptr<FrameSeq> &frameSequencer,
    const std::shared_ptr<EventRegistry> &eventRegistry,
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
    const bool isStepRecording =
        SeqUtil::isStepRecording(screen->getName(), sequencer);

    const bool isRecMainWithoutPlaying = SeqUtil::isRecMainWithoutPlaying(
        sequencer, screens->get<ScreenId::TimingCorrectScreen>(),
        screen->getName(), hardware->getButton(REC),
        controller->clientHardwareEventController);

    const auto timingCorrectScreen =
        screens->get<ScreenId::TimingCorrectScreen>();
    const auto assign16LevelsScreen =
        screens->get<ScreenId::Assign16LevelsScreen>();

    const std::function setSelectedNote = [controller](int n)
    {
        controller->setSelectedNote(n);
    };

    std::function setSelectedPad = [controller](int p)
    {
        controller->setSelectedPad(p);
    };

    const auto hardwareSliderValue = hardware->getSlider()->getValueAs<int>();

    const int drumScreenSelectedDrum =
        screens->get<ScreenId::DrumScreen>()->getDrum();

    return std::make_shared<TriggerLocalNoteOnContext>(
        TriggerLocalNoteOnContext{source,
                                  eventRegistry,
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
                                  sequencer->getTransport()->isRecordingOrOverdubbing(),
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
                                  frameSequencer,
                                  allowCentralNoteAndPadUpdate,
                                  screen,
                                  setSelectedNote,
                                  setSelectedPad,
                                  hardwareSliderValue});
}

std::shared_ptr<TriggerLocalNoteOffContext>
TriggerLocalNoteContextFactory::buildTriggerLocalNoteOffContext(
    const Source source, const int note, Track *track,
    const std::shared_ptr<Bus> &bus,
    const std::shared_ptr<ScreenComponent> &screen,
    const std::optional<int> programPadIndex,
    const std::shared_ptr<Program> &program,
    const std::shared_ptr<Sequencer> &sequencer,
    const std::shared_ptr<FrameSeq> &frameSequencer,
    const std::shared_ptr<EventRegistry> &eventRegistry,
    const std::shared_ptr<ClientEventController> &controller,
    const std::shared_ptr<EventHandler> &eventHandler,
    const std::shared_ptr<Screens> &screens,
    const std::shared_ptr<Hardware> &hardware)
{
    const bool isSamplerScreen = screengroups::isSamplerScreen(screen);

    const auto registrySnapshot = eventRegistry->getSnapshot();
    const std::shared_ptr<sequencer::NoteOnEvent> sequencerRecordNoteOnEvent =
        registrySnapshot.retrieveRecordNoteEvent(note);

    const auto stepEditOptionsScreen =
        screens->get<ScreenId::StepEditOptionsScreen>();
    const auto timingCorrectScreen =
        screens->get<ScreenId::TimingCorrectScreen>();

    const std::function getActiveSequenceLastTick = [sequencer]
    {
        return sequencer->getActiveSequence()->getLastTick();
    };

    const std::function sequencerMoveToQuarterNotePosition =
        [sequencer = sequencer](double quarterNotePosition)
    {
        sequencer->getTransport()->setPosition(quarterNotePosition);
    };

    const std::function sequencerStopMetronomeTrack = [sequencer = sequencer]
    {
        sequencer->getTransport()->stopMetronomeTrack();
    };

    const bool isStepRecording =
        SeqUtil::isStepRecording(screen->getName(), sequencer);

    const bool isRecMainWithoutPlaying = SeqUtil::isRecMainWithoutPlaying(
        sequencer, screens->get<ScreenId::TimingCorrectScreen>(),
        screen->getName(), hardware->getButton(REC),
        controller->clientHardwareEventController);

    return std::make_shared<TriggerLocalNoteOffContext>(
        TriggerLocalNoteOffContext{
            source,
            eventRegistry,
            bus,
            program,
            programPadIndex,
            isSamplerScreen,
            std::make_shared<sequencer::NoteOffEvent>(note),
            eventHandler,
            sequencerRecordNoteOnEvent,
            sequencer->getTransport()->isRecordingOrOverdubbing(),
            hardware->getButton(ERASE)->isPressed(),
            track,
            isStepRecording,
            frameSequencer->getMetronomeOnlyTickPosition(),
            isRecMainWithoutPlaying,
            sequencer->getTransport()->getTickPosition(),
            stepEditOptionsScreen->getTcValuePercentage(),
            timingCorrectScreen->getNoteValueLengthInTicks(),
            stepEditOptionsScreen->isDurationOfRecordedNotesTcValue(),
            stepEditOptionsScreen->isAutoStepIncrementEnabled(),
            sequencer->getTransport()->getCurrentBarIndex(),
            timingCorrectScreen->getSwing(),
            getActiveSequenceLastTick,
            sequencerMoveToQuarterNotePosition,
            sequencerStopMetronomeTrack});
}
