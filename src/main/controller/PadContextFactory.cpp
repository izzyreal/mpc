#include "controller/PadContextFactory.h"

#include "controller/ClientInputControllerBase.h"
#include "lcdgui/ScreenGroups.h"
#include "audiomidi/AudioMidiServices.hpp"

#include "hardware2/Hardware2.h"
#include "lcdgui/screens/DrumScreen.hpp"
#include "lcdgui/screens/window/StepEditOptionsScreen.hpp"
#include "lcdgui/screens/window/TimingCorrectScreen.hpp"
#include "lcdgui/screens/window/Assign16LevelsScreen.hpp"

#include "Mpc.hpp"
#include "sampler/Pad.hpp"
#include "sequencer/SeqUtil.hpp"

using namespace mpc::controller;
using namespace mpc::controls;
using namespace mpc::lcdgui;

int getDrumIndexForCurrentScreen(mpc::Mpc &mpc, const std::string &currentScreenName)
{
    const bool isSamplerScreen = screengroups::isSamplerScreen(currentScreenName);
    const int result = isSamplerScreen ?
        mpc.screens->get<screens::DrumScreen>("drum")->getDrum() :
        mpc.getSequencer()->getActiveTrack()->getBus() - 1;
    return result;
}

PushPadContext PadContextFactory::buildPushPadContext(mpc::Mpc& mpc, int padIndexWithBank, int velocity, const std::string currentScreenName)
{
    const bool isSamplerScreen = screengroups::isSamplerScreen(currentScreenName);
    const bool isSoundScreen = screengroups::isSoundScreen(currentScreenName);
    const bool allowCentralNoteAndPadUpdate = screengroups::isCentralNoteAndPadUpdateScreen(currentScreenName);
    const bool isFullLevelEnabled = mpc.isFullLevelEnabled();
    const bool isSixteenLevelsEnabled = mpc.isSixteenLevelsEnabled();
    const bool isNoteRepeatLockedOrPressed = mpc.inputController->isNoteRepeatLocked() ||
                                             mpc.getHardware2()->getButton("tap")->isPressed();
    const bool isErasePressed = mpc.getHardware2()->getButton("erase")->isPressed();
    const bool isStepRecording = mpc.getControls()->isStepRecording();
    const bool isRecMainWithoutPlaying = sequencer::SeqUtil::isRecMainWithoutPlaying(mpc);

    auto timingCorrectScreen = mpc.screens->get<mpc::lcdgui::screens::window::TimingCorrectScreen>("timing-correct");
    auto assign16LevelsScreen = mpc.screens->get<mpc::lcdgui::screens::window::Assign16LevelsScreen>("assign-16-levels");

    auto activeTrack = mpc.getSequencer()->getActiveTrack();

    const int drumIndex = getDrumIndexForCurrentScreen(mpc, currentScreenName);
    std::shared_ptr<sampler::Program> program = drumIndex >= 0 ? mpc.getSampler()->getProgram(mpc.getDrum(drumIndex).getProgram()) : nullptr;
    
    std::function<void(int)> setMpcNote = [mpc = &mpc] (int n) { mpc->setNote(n); };
    std::function<void(int)> setMpcPad = [mpc = &mpc] (int p) { mpc->setPad(p); };

    const auto hardwareSliderValue = mpc.getHardware2()->getSlider()->getValueAs<int>();
    const int drumScreenSelectedDrum = mpc.screens->get<mpc::lcdgui::screens::DrumScreen>("drum")->getDrum();

    return {
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
        /*programNote*/ program != nullptr ? program->getPad(padIndexWithBank)->getNote() : -1,
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

    const int drumIndex = getDrumIndexForCurrentScreen(mpc, currentScreenName);

    // Ideally we'd know the program that was associated with the pad-push that is associated with the pad-release that we're building
    // the context for. But at the moment, VMPC2000XL doesn't have a notion of which pad-push belongs to which pad-release.
    // The key here is which screen the user was in when the pad-push occurred. So for now we'll assume this stays the same for the
    // duration of the event.
    std::shared_ptr<sampler::Program> program = drumIndex >= 0 ? mpc.getSampler()->getProgram(mpc.getDrum(drumIndex).getProgram()) : nullptr;

    std::function<void(int)> registerProgramPadRelease = [program = program] (int padIndexWithBank) {
        if (program)
        {
            program->registerPadRelease(padIndexWithBank);
        }
    };

    const auto playNoteEvent = mpc.getSequencer()->getNoteEventStore().retrievePlayNoteEvent(padIndexWithBank);

    const int drumScreenSelectedDrum = mpc.screens->get<screens::DrumScreen>("drum")->getDrum();

    auto eventHandler = mpc.getEventHandler();

    const auto recordNoteOnEvent = mpc.getSequencer()->getNoteEventStore().retrieveRecordNoteEvent(padIndexWithBank);

    std::function<bool()> isAnyProgramPadRegisteredAsPressed = [sampler = mpc.getSampler()] { return sampler->isAnyProgramPadRegisteredAsPressed(); };

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
        registerProgramPadRelease,
        playNoteEvent,
        drumScreenSelectedDrum,
        eventHandler,
        recordNoteOnEvent,
        mpc.getSequencer()->isRecordingOrOverdubbing(),
        mpc.getHardware2()->getButton("erase")->isPressed(),
        mpc.getSequencer()->getActiveTrack(),
        mpc.getControls()->isStepRecording(),
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

