#include "command/context/TriggerDrumContextFactory.h"

#include "controller/ClientInputControllerBase.h"
#include "lcdgui/ScreenGroups.h"
#include "audiomidi/AudioMidiServices.hpp"

#include "hardware/Hardware.h"
#include "lcdgui/screens/DrumScreen.hpp"
#include "lcdgui/screens/window/StepEditOptionsScreen.hpp"
#include "lcdgui/screens/window/TimingCorrectScreen.hpp"
#include "lcdgui/screens/window/Assign16LevelsScreen.hpp"

#include "Mpc.hpp"
#include "sampler/Pad.hpp"
#include "sequencer/SeqUtil.hpp"

using namespace mpc::command::context;
using namespace mpc::lcdgui;

int getDrumIndexForCurrentScreen(mpc::Mpc &mpc, const std::string &currentScreenName)
{
    const bool isSamplerScreen = screengroups::isSamplerScreen(currentScreenName);
    const int result = isSamplerScreen ?
        mpc.screens->get<screens::DrumScreen>()->getDrum() :
        mpc.getSequencer()->getActiveTrack()->getBus() - 1;
    return result;
}

TriggerDrumNoteOnContext TriggerDrumContextFactory::buildTriggerDrumNoteOnContext(mpc::Mpc& mpc, int programPadIndex, int velocity, const std::string currentScreenName)
{
    const bool isSamplerScreen = screengroups::isSamplerScreen(currentScreenName);
    const bool isSoundScreen = screengroups::isSoundScreen(currentScreenName);
    const bool allowCentralNoteAndPadUpdate = screengroups::isCentralNoteAndPadUpdateScreen(currentScreenName);
    const bool isFullLevelEnabled = mpc.isFullLevelEnabled();
    const bool isSixteenLevelsEnabled = mpc.isSixteenLevelsEnabled();
    const bool isNoteRepeatLockedOrPressed = mpc.inputController->isNoteRepeatLocked() ||
                                             mpc.getHardware()->getButton(hardware::ComponentId::TAP_TEMPO_OR_NOTE_REPEAT)->isPressed();
    const bool isErasePressed = mpc.getHardware()->getButton(hardware::ComponentId::ERASE)->isPressed();
    const bool isStepRecording = sequencer::SeqUtil::isStepRecording(mpc);
    const bool isRecMainWithoutPlaying = sequencer::SeqUtil::isRecMainWithoutPlaying(mpc);

    auto timingCorrectScreen = mpc.screens->get<mpc::lcdgui::screens::window::TimingCorrectScreen>();
    auto assign16LevelsScreen = mpc.screens->get<mpc::lcdgui::screens::window::Assign16LevelsScreen>();

    auto activeTrack = mpc.getSequencer()->getActiveTrack();

    const int drumIndex = getDrumIndexForCurrentScreen(mpc, currentScreenName);
    std::shared_ptr<sampler::Program> program = drumIndex >= 0 ? mpc.getSampler()->getProgram(mpc.getDrum(drumIndex).getProgram()) : nullptr;
    
    std::function<void(int)> setMpcNote = [mpc = &mpc] (int n) { mpc->setNote(n); };
    std::function<void(int)> setMpcPad = [mpc = &mpc] (int p) { mpc->setPad(p); };

    const auto hardwareSliderValue = mpc.getHardware()->getSlider()->getValueAs<int>();
    const int drumScreenSelectedDrum = mpc.screens->get<mpc::lcdgui::screens::DrumScreen>()->getDrum();
    const auto note = activeTrack->getBus() > 0 ? program->getPad(programPadIndex)->getNote() : programPadIndex + 35;  
    
    return {
        programPadIndex,
        velocity,
        currentScreenName,
        isSoundScreen,
        isFullLevelEnabled,
        isSixteenLevelsEnabled,
        isNoteRepeatLockedOrPressed,
        isErasePressed,
        isStepRecording,
        isRecMainWithoutPlaying,
        mpc.getBank(),
        mpc.getSequencer()->isPlaying(),
        mpc.getSequencer()->isRecordingOrOverdubbing(),
        mpc.getSequencer()->getCurrentBarIndex(),
        mpc.getSequencer()->getTickPosition(),
        currentScreenName == "song",
        mpc.getAudioMidiServices()->getFrameSequencer()->getMetronomeOnlyTickPosition(),
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
        setMpcNote,
        setMpcPad,
        mpc.getLayeredScreen()->getFocus(),
        hardwareSliderValue,
        mpc.inputController->isPhysicallyPressed(programPadIndex % 16, mpc.getBank())
    }; 
}

TriggerDrumNoteOffContext TriggerDrumContextFactory::buildTriggerDrumNoteOffContext(mpc::Mpc &mpc, const int programPadIndex, const std::string currentScreenName)
{
    std::function<void()> finishBasicVoiceIfSoundIsLooping = [basicPlayer = &mpc.getBasicPlayer()]() { basicPlayer->finishVoiceIfSoundIsLooping(); };

    const bool isSamplerScreen = screengroups::isSamplerScreen(currentScreenName);
    const bool isSoundScreen = screengroups::isSoundScreen(currentScreenName);

    const int drumIndex = getDrumIndexForCurrentScreen(mpc, currentScreenName);

    const auto playNoteEvent = mpc.getSequencer()->getNoteEventStore().retrievePlayNoteEvent(programPadIndex);

    const int drumScreenSelectedDrum = mpc.screens->get<screens::DrumScreen>()->getDrum();

    auto eventHandler = mpc.getEventHandler();

    const auto recordNoteOnEvent = mpc.getSequencer()->getNoteEventStore().retrieveRecordNoteEvent(programPadIndex);

    std::function<bool()> isAnyProgramPadRegisteredAsPressed = [sampler = mpc.getSampler()] { return sampler->isAnyProgramPadRegisteredAsPressed(); };

    const auto stepEditOptionsScreen = mpc.screens->get<mpc::lcdgui::screens::window::StepEditOptionsScreen>();
    const auto timingCorrectScreen = mpc.screens->get<mpc::lcdgui::screens::window::TimingCorrectScreen>();

    std::function<int()> getActiveSequenceLastTick = [sequencer = mpc.getSequencer()] { return sequencer->getActiveSequence()->getLastTick(); };

    std::function<void(double)> sequencerMoveToQuarterNotePosition = [sequencer = mpc.getSequencer()](double quarterNotePosition) { sequencer->move(quarterNotePosition); };

    std::function<void()> sequencerStopMetronomeTrack = [sequencer = mpc.getSequencer()] { sequencer->stopMetronomeTrack(); };

    return {
        programPadIndex,
        finishBasicVoiceIfSoundIsLooping,
        isSoundScreen,
        isSamplerScreen,
        playNoteEvent,
        drumScreenSelectedDrum,
        eventHandler,
        recordNoteOnEvent,
        mpc.getSequencer()->isRecordingOrOverdubbing(),
        mpc.getHardware()->getButton(hardware::ComponentId::ERASE)->isPressed(),
        mpc.getSequencer()->getActiveTrack(),
        sequencer::SeqUtil::isStepRecording(mpc),
        isAnyProgramPadRegisteredAsPressed,
        mpc.getAudioMidiServices()->getFrameSequencer()->getMetronomeOnlyTickPosition(),
        sequencer::SeqUtil::isRecMainWithoutPlaying(mpc),
        mpc.getSequencer()->getTickPosition(),
        stepEditOptionsScreen->getTcValuePercentage(),
        timingCorrectScreen->getNoteValueLengthInTicks(),
        stepEditOptionsScreen->isDurationOfRecordedNotesTcValue(),
        stepEditOptionsScreen->isAutoStepIncrementEnabled(),
        mpc.getSequencer()->getCurrentBarIndex(),
        timingCorrectScreen->getSwing(),
        getActiveSequenceLastTick,
        sequencerMoveToQuarterNotePosition,
        sequencerStopMetronomeTrack
    };
}

