#include "command/context/TriggerDrumContextFactory.hpp"

#include "controller/ClientEventController.hpp"
#include "controller/ClientHardwareEventController.hpp"
#include "lcdgui/ScreenGroups.hpp"
#include "audiomidi/AudioMidiServices.hpp"

#include "hardware/Hardware.hpp"
#include "lcdgui/screens/SequencerScreen.hpp"
#include "lcdgui/screens/DrumScreen.hpp"
#include "lcdgui/screens/SongScreen.hpp"
#include "lcdgui/screens/window/StepEditOptionsScreen.hpp"
#include "lcdgui/screens/window/TimingCorrectScreen.hpp"
#include "lcdgui/screens/window/Assign16LevelsScreen.hpp"

#include "Mpc.hpp"
#include "sampler/Pad.hpp"
#include "sequencer/SeqUtil.hpp"
#include <memory>

using namespace mpc::command::context;
using namespace mpc::lcdgui;

int getDrumIndexForCurrentScreen(mpc::Mpc &mpc,
                                 const std::shared_ptr<ScreenComponent> screen)
{
    const bool isSamplerScreen = screengroups::isSamplerScreen(screen);
    const int result = isSamplerScreen
                           ? mpc.screens->get<screens::DrumScreen>()->getDrum()
                           : mpc.getSequencer()->getActiveTrack()->getBus() - 1;
    return result;
}

TriggerDrumNoteOnContext
TriggerDrumContextFactory::buildTriggerDrumNoteOnContext(
    mpc::Mpc &mpc, int programPadIndex, int velocity,
    const std::shared_ptr<ScreenComponent> screen)
{
    const bool isSequencerScreen =
        std::dynamic_pointer_cast<SequencerScreen>(screen) != nullptr;
    const bool isSongScreen =
        std::dynamic_pointer_cast<SongScreen>(screen) != nullptr;
    const bool isSamplerScreen = screengroups::isSamplerScreen(screen);
    const bool isSoundScreen = screengroups::isSoundScreen(screen);
    const bool allowCentralNoteAndPadUpdate =
        screengroups::isCentralNoteAndPadUpdateScreen(screen);
    const bool isFullLevelEnabled =
        mpc.clientEventController->isFullLevelEnabled();
    const bool isSixteenLevelsEnabled =
        mpc.clientEventController->isSixteenLevelsEnabled();
    const bool isNoteRepeatLockedOrPressed =
        mpc.clientEventController->clientHardwareEventController
            ->isNoteRepeatLocked() ||
        mpc.getHardware()
            ->getButton(hardware::ComponentId::TAP_TEMPO_OR_NOTE_REPEAT)
            ->isPressed();
    const bool isErasePressed =
        mpc.getHardware()->getButton(hardware::ComponentId::ERASE)->isPressed();
    const bool isStepRecording = sequencer::SeqUtil::isStepRecording(
        screen->getName(), mpc.getSequencer());

    const bool isRecMainWithoutPlaying =
        sequencer::SeqUtil::isRecMainWithoutPlaying(
            mpc.getSequencer(), mpc.screens->get<TimingCorrectScreen>(),
            screen->getName(),
            mpc.getHardware()->getButton(hardware::ComponentId::REC),
            mpc.clientEventController->clientHardwareEventController);

    auto timingCorrectScreen =
        mpc.screens->get<mpc::lcdgui::screens::window::TimingCorrectScreen>();
    auto assign16LevelsScreen =
        mpc.screens->get<mpc::lcdgui::screens::window::Assign16LevelsScreen>();

    auto activeTrack = mpc.getSequencer()->getActiveTrack();

    const int drumIndex = getDrumIndexForCurrentScreen(mpc, screen);
    std::shared_ptr<sampler::Program> program =
        drumIndex >= 0
            ? mpc.getSampler()->getProgram(mpc.getDrum(drumIndex).getProgram())
            : nullptr;

    std::function<void(int)> setSelectedNote =
        [controller = mpc.clientEventController](int n)
    {
        controller->setSelectedNote(n);
    };
    std::function<void(int)> setSelectedPad =
        [controller = mpc.clientEventController](int p)
    {
        controller->setSelectedPad(p);
    };

    const auto hardwareSliderValue =
        mpc.getHardware()->getSlider()->getValueAs<int>();
    const int drumScreenSelectedDrum =
        mpc.screens->get<mpc::lcdgui::screens::DrumScreen>()->getDrum();
    const auto note = activeTrack->getBus() > 0
                          ? program->getPad(programPadIndex)->getNote()
                          : programPadIndex + 35;

    return {
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
        mpc.clientEventController->getActiveBank(),
        mpc.getSequencer()->isPlaying(),
        mpc.getSequencer()->isRecordingOrOverdubbing(),
        mpc.getSequencer()->getCurrentBarIndex(),
        mpc.getSequencer()->getTickPosition(),
        isSongScreen,
        mpc.getAudioMidiServices()
            ->getFrameSequencer()
            ->getMetronomeOnlyTickPosition(),
        timingCorrectScreen->getNoteValueLengthInTicks(),
        timingCorrectScreen->getSwing(),
        activeTrack->getBus(),
        program,
        note,
        mpc.getSampler()->getSoundIndex(),
        drumScreenSelectedDrum,
        isSamplerScreen,
        activeTrack,
        mpc.getSampler(),
        mpc.getSequencer(),
        mpc.getAudioMidiServices(),
        mpc.getLayeredScreen(),
        timingCorrectScreen,
        assign16LevelsScreen,
        mpc.getEventHandler(),
        mpc.getAudioMidiServices()->getFrameSequencer(),
        mpc.getBasicPlayer(),
        allowCentralNoteAndPadUpdate,
        mpc.getScreen(),
        setSelectedNote,
        setSelectedPad,
        mpc.getLayeredScreen()->getFocusedFieldName(),
        hardwareSliderValue,
        mpc.clientEventController->clientHardwareEventController
            ->isPhysicallyPressed(programPadIndex % 16,
                                  mpc.clientEventController->getActiveBank())};
}

TriggerDrumNoteOffContext
TriggerDrumContextFactory::buildTriggerDrumNoteOffContext(
    mpc::Mpc &mpc, const int programPadIndex, std::optional<int> drumIndex,
    const std::shared_ptr<ScreenComponent> screen)
{
    std::function<void()> finishBasicVoiceIfSoundIsLooping =
        [basicPlayer = &mpc.getBasicPlayer()]()
    {
        basicPlayer->finishVoiceIfSoundIsLooping();
    };

    const bool isSamplerScreen = screengroups::isSamplerScreen(screen);
    const bool isSoundScreen = screengroups::isSoundScreen(screen);

    const auto playNoteEvent =
        mpc.getSequencer()->getNoteEventStore().retrievePlayNoteEvent(
            programPadIndex);

    auto eventHandler = mpc.getEventHandler();

    const auto recordNoteOnEvent =
        mpc.getSequencer()->getNoteEventStore().retrieveRecordNoteEvent(
            programPadIndex);

    std::function<bool()> isAnyProgramPadRegisteredAsPressed =
        [sampler = mpc.getSampler()]
    {
        return sampler->isAnyProgramPadRegisteredAsPressed();
    };

    const auto stepEditOptionsScreen =
        mpc.screens->get<mpc::lcdgui::screens::window::StepEditOptionsScreen>();
    const auto timingCorrectScreen =
        mpc.screens->get<mpc::lcdgui::screens::window::TimingCorrectScreen>();

    std::function<int()> getActiveSequenceLastTick =
        [sequencer = mpc.getSequencer()]
    {
        return sequencer->getActiveSequence()->getLastTick();
    };

    std::function<void(double)> sequencerMoveToQuarterNotePosition =
        [sequencer = mpc.getSequencer()](double quarterNotePosition)
    {
        sequencer->move(quarterNotePosition);
    };

    std::function<void()> sequencerStopMetronomeTrack =
        [sequencer = mpc.getSequencer()]
    {
        sequencer->stopMetronomeTrack();
    };

    const bool isStepRecording = sequencer::SeqUtil::isStepRecording(
        screen->getName(), mpc.getSequencer());

    const bool isRecMainWithoutPlaying =
        sequencer::SeqUtil::isRecMainWithoutPlaying(
            mpc.getSequencer(), mpc.screens->get<TimingCorrectScreen>(),
            screen->getName(),
            mpc.getHardware()->getButton(hardware::ComponentId::REC),
            mpc.clientEventController->clientHardwareEventController);

    return {
        programPadIndex,
        finishBasicVoiceIfSoundIsLooping,
        isSoundScreen,
        isSamplerScreen,
        playNoteEvent,
        drumIndex,
        eventHandler,
        recordNoteOnEvent,
        mpc.getSequencer()->isRecordingOrOverdubbing(),
        mpc.getHardware()->getButton(hardware::ComponentId::ERASE)->isPressed(),
        mpc.getSequencer()->getActiveTrack(),
        isStepRecording,
        isAnyProgramPadRegisteredAsPressed,
        mpc.getAudioMidiServices()
            ->getFrameSequencer()
            ->getMetronomeOnlyTickPosition(),
        isRecMainWithoutPlaying,
        mpc.getSequencer()->getTickPosition(),
        stepEditOptionsScreen->getTcValuePercentage(),
        timingCorrectScreen->getNoteValueLengthInTicks(),
        stepEditOptionsScreen->isDurationOfRecordedNotesTcValue(),
        stepEditOptionsScreen->isAutoStepIncrementEnabled(),
        mpc.getSequencer()->getCurrentBarIndex(),
        timingCorrectScreen->getSwing(),
        getActiveSequenceLastTick,
        sequencerMoveToQuarterNotePosition,
        sequencerStopMetronomeTrack};
}
