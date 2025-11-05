#include "command/context/TriggerLocalNoteContextFactory.hpp"

#include "controller/ClientEventController.hpp"
#include "controller/ClientHardwareEventController.hpp"
#include "lcdgui/LayeredScreen.hpp"
#include "lcdgui/ScreenGroups.hpp"
#include "audiomidi/AudioMidiServices.hpp"

#include "hardware/Hardware.hpp"
#include "lcdgui/screens/SequencerScreen.hpp"
#include "lcdgui/screens/DrumScreen.hpp"
#include "lcdgui/screens/window/StepEditOptionsScreen.hpp"
#include "lcdgui/screens/window/TimingCorrectScreen.hpp"
#include "lcdgui/screens/window/Assign16LevelsScreen.hpp"

#include "sampler/Pad.hpp"
#include "sampler/Sampler.hpp"
#include "sequencer/Bus.hpp"
#include "sequencer/FrameSeq.hpp"
#include "sequencer/SeqUtil.hpp"
#include <memory>

#include "eventregistry/EventRegistry.hpp"
#include "lcdgui/screens/window/EditMultipleScreen.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Track.hpp"

using namespace mpc::command::context;
using namespace mpc::controller;
using namespace mpc::lcdgui;
using namespace mpc::sequencer;
using namespace mpc::hardware;
using namespace mpc::sampler;
using namespace mpc::eventregistry;
using namespace mpc::audiomidi;
using namespace mpc::engine;

int getDrumIndexForCurrentScreen(std::shared_ptr<Sequencer> sequencer,
                                 const std::shared_ptr<ScreenComponent> screen,
                                 std::shared_ptr<Screens> screens)
{
    const bool isSamplerScreen = screengroups::isSamplerScreen(screen);
    const int result = isSamplerScreen
                           ? screens->get<ScreenId::DrumScreen>()->getDrum()
                           : sequencer->getActiveTrack()->getBus() - 1;
    return result;
}

std::shared_ptr<TriggerLocalNoteOnContext>
TriggerLocalNoteContextFactory::buildTriggerDrumNoteOnContext(
    eventregistry::Source source, std::shared_ptr<LayeredScreen> layeredScreen,
    std::shared_ptr<ClientEventController> controller,
    std::shared_ptr<Hardware> hardware, std::shared_ptr<Sequencer> sequencer,
    std::shared_ptr<Screens> screens, std::shared_ptr<Sampler> sampler,
    std::shared_ptr<EventRegistry> eventRegistry,
    std::shared_ptr<EventHandler> eventHandler,
    std::shared_ptr<FrameSeq> frameSequencer,
    PreviewSoundPlayer *previewSoundPlayer, int programPadIndex, int velocity,
    const std::shared_ptr<ScreenComponent> screen)
{
    const bool isSequencerScreen =
        layeredScreen->isCurrentScreen<ScreenId::SequencerScreen>();
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

    auto track = sequencer->getActiveTrack().get();

    const int drumIndex =
        getDrumIndexForCurrentScreen(sequencer, screen, screens);
    const auto drumBus = sequencer->getDrumBus(drumIndex);
    std::shared_ptr<sampler::Program> program =
        drumIndex >= 0 ? sampler->getProgram(drumBus->getProgram()) : nullptr;

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
    const auto note = track->getBus() > 0
                          ? program->getPad(programPadIndex)->getNote()
                          : programPadIndex + 35;

    return std::make_shared<TriggerLocalNoteOnContext>(
        TriggerLocalNoteOnContext{source,
                                  eventRegistry,
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
                                  track->getBus(),
                                  program,
                                  note,
                                  drumScreenSelectedDrum,
                                  isSamplerScreen,
                                  track,
                                  sampler,
                                  sequencer,
                                  timingCorrectScreen,
                                  assign16LevelsScreen,
                                  eventHandler,
                                  frameSequencer,
                                  previewSoundPlayer,
                                  allowCentralNoteAndPadUpdate,
                                  screen,
                                  setSelectedNote,
                                  setSelectedPad,
                                  layeredScreen->getFocusedFieldName(),
                                  hardwareSliderValue});
}

std::shared_ptr<TriggerLocalNoteOffContext>
TriggerLocalNoteContextFactory::buildTriggerDrumNoteOffContext(
    eventregistry::Source source, PreviewSoundPlayer *previewSoundPlayer,
    std::shared_ptr<EventRegistry> eventRegistry,
    std::shared_ptr<EventHandler> eventHandler,
    std::shared_ptr<Screens> screens, std::shared_ptr<Sequencer> sequencer,
    std::shared_ptr<Hardware> hardware,
    std::shared_ptr<ClientEventController> controller,
    std::shared_ptr<FrameSeq> frameSequencer, const int programPadIndex,
    int drumIndex, const std::shared_ptr<ScreenComponent> screen,
    const int note, std::shared_ptr<sampler::Program> program, Track *track)
{
    std::function<void()> finishBasicVoiceIfSoundIsLooping =
        [previewSoundPlayer]()
    {
        previewSoundPlayer->finishVoiceIfSoundIsLooping();
    };

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

    std::shared_ptr<sequencer::DrumBus> drumBus =
        sequencer->getDrumBus(drumIndex);

    return std::make_shared<TriggerLocalNoteOffContext>(
        TriggerLocalNoteOffContext{
            source,
            eventRegistry,
            drumBus,
            program,
            programPadIndex,
            finishBasicVoiceIfSoundIsLooping,
            isSoundScreen,
            isSamplerScreen,
            std::make_shared<sequencer::NoteOffEvent>(note),
            drumIndex,
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
