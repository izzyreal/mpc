#include "controller/PadContextFactory.h"

#include "lcdgui/ScreenGroups.h"
#include "audiomidi/AudioMidiServices.hpp"

#include "hardware2/Hardware2.h"
#include "lcdgui/screens/DrumScreen.hpp"
#include "lcdgui/screens/window/StepEditOptionsScreen.hpp"
#include "lcdgui/screens/window/TimingCorrectScreen.hpp"
#include "lcdgui/screens/window/Assign16LevelsScreen.hpp"

#include "Mpc.hpp"
#include "sampler/Pad.hpp"

using namespace mpc::controller;
using namespace mpc::controls;
using namespace mpc::lcdgui;

PushPadContext PadContextFactory::buildPushPadContext(mpc::Mpc& mpc, int padIndexWithBank, int velocity, const std::string currentScreenName)
{
    const bool isSamplerScreen = screengroups::isSamplerScreen(currentScreenName);
    const bool isSoundScreen = screengroups::isSoundScreen(currentScreenName);
    const bool allowCentralNoteAndPadUpdate = screengroups::isCentralNoteAndPadUpdateScreen(currentScreenName);
    const bool isFullLevelEnabled = mpc.isFullLevelEnabled();
    const bool isSixteenLevelsEnabled = mpc.isSixteenLevelsEnabled();
    const bool isTapPressed = mpc.getControls()->isTapPressed();
    const bool isNoteRepeatLocked = mpc.getControls()->isNoteRepeatLocked();
    const bool isErasePressed = mpc.getControls()->isErasePressed();
    const bool isStepRecording = mpc.getControls()->isStepRecording();
    const bool isRecMainWithoutPlaying = mpc.getControls()->isRecMainWithoutPlaying();

    auto timingCorrectScreen = mpc.screens->get<mpc::lcdgui::screens::window::TimingCorrectScreen>("timing-correct");
    auto assign16LevelsScreen = mpc.screens->get<mpc::lcdgui::screens::window::Assign16LevelsScreen>("assign-16-levels");

    auto activeTrack = mpc.getSequencer()->getActiveTrack();

    std::shared_ptr<mpc::sampler::Program> program;
    mpc::engine::Drum *activeDrum = nullptr;

    const auto drumScreen = mpc.screens->get<mpc::lcdgui::screens::DrumScreen>("drum");
    
    const auto drumIndex = isSamplerScreen ? drumScreen->getDrum() : activeTrack->getBus() - 1;

    int programNote = -1;
    
    if (drumIndex != -1)
    {
        activeDrum = &mpc.getDrum(drumIndex);
        program = mpc.getSampler()->getProgram(activeDrum->getProgram());
        programNote = program->getPad(padIndexWithBank)->getNote();
    }

    std::function<void(int)> setMpcNote = [mpc = &mpc] (int n) { mpc->setNote(n); };
    std::function<void(int)> setMpcPad = [mpc = &mpc] (int p) { mpc->setPad(p); };

    const auto hardwareSliderValue = mpc.getHardware2()->getSlider()->getValueAs<int>();

    return {
        currentScreenName,
        isSoundScreen,
        isFullLevelEnabled,
        isSixteenLevelsEnabled,
        isTapPressed,
        isNoteRepeatLocked,
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
        programNote,
        mpc.getSampler()->getSoundIndex(),
        drumScreen->getDrum(),
        isSamplerScreen,
        activeTrack,
        mpc.getSampler(),
        mpc.getSequencer(),
        mpc.getAudioMidiServices(),
        mpc.getControls(),
        mpc.getLayeredScreen(),
        timingCorrectScreen,
        assign16LevelsScreen,
        mpc.getEventHandler(),
        mpc.getAudioMidiServices()->getFrameSequencer(),
        mpc.getBasicPlayer(),
        allowCentralNoteAndPadUpdate,
        mpc.getActiveControls(),
        setMpcNote,
        setMpcPad,
        mpc.getLayeredScreen()->getFocus(),
        hardwareSliderValue
    }; 
}

PadReleaseContext PadContextFactory::buildPadReleaseContext(mpc::Mpc &mpc, const int padIndexWithBank, const std::string currentScreenName)
{
    std::function<void()> finishBasicVoiceIfSoundIsLooping = [basicPlayer = &mpc.getBasicPlayer()]() { basicPlayer->finishVoiceIfSoundIsLooping(); };

    const bool isSamplerScreen = screengroups::isSamplerScreen(currentScreenName);
    const bool isSoundScreen = screengroups::isSoundScreen(currentScreenName);

    std::function<void(int)> controlsUnpressPad = [controls = mpc.getControls()] (int p) { controls->unpressPad(p); };

    const auto playNoteEvent = mpc.getSequencer()->getNoteEventStore().retrievePlayNoteEvent(padIndexWithBank);

    const int drumScreenSelectedDrum = mpc.screens->get<mpc::lcdgui::screens::DrumScreen>("drum")->getDrum();

    auto eventHandler = mpc.getEventHandler();

    const auto recordNoteOnEvent = mpc.getSequencer()->getNoteEventStore().retrieveRecordNoteEvent(padIndexWithBank);

    std::function<bool()> arePadsPressed = [controls = mpc.getControls()] { return controls->arePadsPressed(); };

    const auto stepEditOptionsScreen = mpc.screens->get<mpc::lcdgui::screens::window::StepEditOptionsScreen>("step-edit-options");
    const auto timingCorrectScreen = mpc.screens->get<mpc::lcdgui::screens::window::TimingCorrectScreen>("timing-correct");

    std::function<int()> getActiveSequenceLastTick = [sequencer = mpc.getSequencer()] { return sequencer->getActiveSequence()->getLastTick(); };

    std::function<void(double)> sequencerMoveToQuarterNotePosition = [sequencer = mpc.getSequencer()](double quarterNotePosition) { sequencer->move(quarterNotePosition); };

    std::function<void()> sequencerStopMetronomeTrack = [sequencer = mpc.getSequencer()] { sequencer->stopMetronomeTrack(); };

    return {
        padIndexWithBank,
        finishBasicVoiceIfSoundIsLooping,
        isSoundScreen,
        isSamplerScreen,
        controlsUnpressPad,
        playNoteEvent,
        drumScreenSelectedDrum,
        eventHandler,
        recordNoteOnEvent,
        mpc.getSequencer()->isRecordingOrOverdubbing(),
        mpc.getControls()->isErasePressed(),
        mpc.getSequencer()->getActiveTrack(),
        mpc.getControls()->isStepRecording(),
        arePadsPressed,
        mpc.getAudioMidiServices()->getFrameSequencer()->getMetronomeOnlyTickPosition(),
        mpc.getControls()->isRecMainWithoutPlaying(),
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

