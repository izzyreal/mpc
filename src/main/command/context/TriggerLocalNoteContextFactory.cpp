#include "command/context/TriggerLocalNoteContextFactory.hpp"

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
#include "sequencer/Track.hpp"
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
    eventregistry::Source source,
    eventregistry::NoteOnEvent *registryNoteOnEvent, const int note,
    const int velocity, sequencer::Track *track,
    std::shared_ptr<sequencer::Bus> bus,
    const std::shared_ptr<lcdgui::ScreenComponent> screen,
    std::optional<int> programPadIndex,
    std::shared_ptr<sampler::Program> program,
    std::shared_ptr<sequencer::Sequencer> sequencer,
    std::shared_ptr<sequencer::FrameSeq> frameSequencer,
    std::shared_ptr<eventregistry::EventRegistry> eventRegistry,
    std::shared_ptr<controller::ClientEventController> controller,
    std::shared_ptr<audiomidi::EventHandler> eventHandler,
    std::shared_ptr<lcdgui::Screens> screens,
    std::shared_ptr<hardware::Hardware> hardware)
{
    const bool isSequencerScreen =
        std::dynamic_pointer_cast<SequencerScreen>(screen) != nullptr;
    const bool isSamplerScreen = screengroups::isSamplerScreen(screen);
    const bool isSoundScreen = screengroups::isSoundScreen(screen);
    const bool allowCentralNoteAndPadUpdate =
        screengroups::isCentralNoteAndPadUpdateScreen(screen);
    const bool isFullLevelEnabled = controller->isFullLevelEnabled();
    const bool isSixteenLevelsEnabled = controller->isSixteenLevelsEnabled();
    const bool isNoteRepeatLockedOrPressed =
        controller->clientHardwareEventController->isNoteRepeatLocked() ||
        hardware->getButton(hardware::ComponentId::TAP_TEMPO_OR_NOTE_REPEAT)
            ->isPressed();
    const bool isErasePressed =
        hardware->getButton(hardware::ComponentId::ERASE)->isPressed();
    const bool isStepRecording =
        sequencer::SeqUtil::isStepRecording(screen->getName(), sequencer);

    const bool isRecMainWithoutPlaying =
        sequencer::SeqUtil::isRecMainWithoutPlaying(
            sequencer, screens->get<ScreenId::TimingCorrectScreen>(),
            screen->getName(), hardware->getButton(hardware::ComponentId::REC),
            controller->clientHardwareEventController);

    auto timingCorrectScreen = screens->get<ScreenId::TimingCorrectScreen>();
    auto assign16LevelsScreen = screens->get<ScreenId::Assign16LevelsScreen>();

    std::function<void(int)> setSelectedNote = [controller](int n)
    {
        controller->setSelectedNote(n);
    };

    std::function<void(int)> setSelectedPad = [controller](int p)
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
                                  isSoundScreen,
                                  isFullLevelEnabled,
                                  isSixteenLevelsEnabled,
                                  isNoteRepeatLockedOrPressed,
                                  isErasePressed,
                                  isStepRecording,
                                  isRecMainWithoutPlaying,
                                  sequencer->isRecordingOrOverdubbing(),
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
    eventregistry::Source source, const int note, Track *track,
    std::shared_ptr<sequencer::Bus> bus,
    const std::shared_ptr<ScreenComponent> screen,
    std::optional<int> programPadIndex,
    std::shared_ptr<sampler::Program> program,
    std::shared_ptr<Sequencer> sequencer,
    std::shared_ptr<FrameSeq> frameSequencer,
    std::shared_ptr<EventRegistry> eventRegistry,
    std::shared_ptr<ClientEventController> controller,
    std::shared_ptr<EventHandler> eventHandler,
    std::shared_ptr<Screens> screens, std::shared_ptr<Hardware> hardware)
{
    const bool isSamplerScreen = screengroups::isSamplerScreen(screen);
    const bool isSoundScreen = screengroups::isSoundScreen(screen);

    const auto registrySnapshot = eventRegistry->getSnapshot();
    const std::shared_ptr<sequencer::NoteOnEvent> sequencerRecordNoteOnEvent =
        registrySnapshot.retrieveRecordNoteEvent(note);

    const auto stepEditOptionsScreen =
        screens->get<ScreenId::StepEditOptionsScreen>();
    const auto timingCorrectScreen =
        screens->get<ScreenId::TimingCorrectScreen>();

    std::function<int()> getActiveSequenceLastTick = [sequencer]
    {
        return sequencer->getActiveSequence()->getLastTick();
    };

    std::function<void(double)> sequencerMoveToQuarterNotePosition =
        [sequencer = sequencer](double quarterNotePosition)
    {
        sequencer->move(quarterNotePosition);
    };

    std::function<void()> sequencerStopMetronomeTrack = [sequencer = sequencer]
    {
        sequencer->stopMetronomeTrack();
    };

    const bool isStepRecording =
        sequencer::SeqUtil::isStepRecording(screen->getName(), sequencer);

    const bool isRecMainWithoutPlaying =
        sequencer::SeqUtil::isRecMainWithoutPlaying(
            sequencer, screens->get<ScreenId::TimingCorrectScreen>(),
            screen->getName(), hardware->getButton(hardware::ComponentId::REC),
            controller->clientHardwareEventController);

    return std::make_shared<TriggerLocalNoteOffContext>(
        TriggerLocalNoteOffContext{
            source,
            eventRegistry,
            bus,
            program,
            programPadIndex,
            isSoundScreen,
            isSamplerScreen,
            std::make_shared<sequencer::NoteOffEvent>(note),
            eventHandler,
            sequencerRecordNoteOnEvent,
            sequencer->isRecordingOrOverdubbing(),
            hardware->getButton(hardware::ComponentId::ERASE)->isPressed(),
            track,
            isStepRecording,
            frameSequencer->getMetronomeOnlyTickPosition(),
            isRecMainWithoutPlaying,
            sequencer->getTickPosition(),
            stepEditOptionsScreen->getTcValuePercentage(),
            timingCorrectScreen->getNoteValueLengthInTicks(),
            stepEditOptionsScreen->isDurationOfRecordedNotesTcValue(),
            stepEditOptionsScreen->isAutoStepIncrementEnabled(),
            sequencer->getCurrentBarIndex(),
            timingCorrectScreen->getSwing(),
            getActiveSequenceLastTick,
            sequencerMoveToQuarterNotePosition,
            sequencerStopMetronomeTrack});
}
