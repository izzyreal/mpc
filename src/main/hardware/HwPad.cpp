#include "audiomidi/AudioMidiServices.hpp"
#include "engine/PreviewSoundPlayer.hpp"
#include "lcdgui/screens/DrumScreen.hpp"
#include "lcdgui/screens/window/StepEditOptionsScreen.hpp"
#include "lcdgui/screens/window/TimingCorrectScreen.hpp"
#include <hardware/HwPad.hpp>

#include <Mpc.hpp>

#include <lcdgui/screens/OpensNameScreen.hpp>
#include <lcdgui/ScreenComponent.hpp>
#include <controls/GlobalReleaseControls.hpp>

using namespace mpc::hardware;
using namespace mpc::lcdgui::screens;

HwPad::HwPad(mpc::Mpc& mpc, int index)
        : HwComponent(mpc, "pad-" + std::to_string(index + 1))
{
    this->index = index;
}

int HwPad::getIndex()
{
    return index;
}

void HwPad::push(int velo)
{
    if (auto opensNameScreen = std::dynamic_pointer_cast<OpensNameScreen>(mpc.getActiveControls()))
    {
        opensNameScreen->openNameScreen();
    }

    HwComponent::push(velo);

    if (isPressed())
    {
        return;
    }

    pressure = velo;

    padIndexWithBankWhenLastPressed = index + (mpc.getBank() * 16);

    auto c = mpc.getActiveControls();
    if (!c)
        return;
    c->pad(padIndexWithBankWhenLastPressed, velo);
}

void HwPad::release()
{
    if (!isPressed())
    {
        return;
    }

    pressure = 0;

    auto c = mpc.getReleaseControls();

    if (!c || padIndexWithBankWhenLastPressed == -1)
    {
        padIndexWithBankWhenLastPressed = -1;
        return;
    }

    const auto currentScreenName = mpc.getLayeredScreen()->getCurrentScreenName();
    std::function<void()> finishBasicVoiceIfSoundIsLooping = [basicPlayer = &mpc.getBasicPlayer()]() { basicPlayer->finishVoiceIfSoundIsLooping(); };

    const bool currentScreenIsSoundScreen = std::find(
            mpc::controls::BaseControls::soundScreens.begin(),
            mpc::controls::BaseControls::soundScreens.end(),
            currentScreenName) != mpc::controls::BaseControls::soundScreens.end();
    
    const bool currentScreenIsSamplerScreen = std::find(
            mpc::controls::BaseControls::samplerScreens.begin(),
            mpc::controls::BaseControls::samplerScreens.end(),
            currentScreenName) != mpc::controls::BaseControls::samplerScreens.end();

    std::function<void(int)> controlsUnpressPad = [controls = mpc.getControls()] (int p) { controls->unpressPad(p); };

    const auto playNoteEvent = mpc.getControls()->retrievePlayNoteEvent(padIndexWithBankWhenLastPressed);

    const int drumScreenSelectedDrum = mpc.screens->get<mpc::lcdgui::screens::DrumScreen>("drum")->getDrum();

    auto eventHandler = mpc.getEventHandler();

    const auto recordNoteOnEvent = mpc.getControls()->retrieveRecordNoteEvent(padIndexWithBankWhenLastPressed);

    std::function<bool()> arePadsPressed = [controls = mpc.getControls()] { return controls->arePadsPressed(); };

    const auto stepEditOptionsScreen = mpc.screens->get<mpc::lcdgui::screens::window::StepEditOptionsScreen>("step-edit-options");
    const auto timingCorrectScreen = mpc.screens->get<mpc::lcdgui::screens::window::TimingCorrectScreen>("timing-correct");

    std::function<int()> getActiveSequenceLastTick = [sequencer = mpc.getSequencer()] { return sequencer->getActiveSequence()->getLastTick(); };

    std::function<void(double)> sequencerMoveToQuarterNotePosition = [sequencer = mpc.getSequencer()](double quarterNotePosition) { sequencer->move(quarterNotePosition); };

    std::function<void()> sequencerStopMetronomeTrack = [sequencer = mpc.getSequencer()] { sequencer->stopMetronomeTrack(); };

    controls::PadReleaseContext ctx {
        padIndexWithBankWhenLastPressed,
        finishBasicVoiceIfSoundIsLooping,
        currentScreenIsSoundScreen,
        currentScreenIsSamplerScreen,
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

    controls::GlobalReleaseControls::simplePad(ctx);

    padIndexWithBankWhenLastPressed = -1;
}

bool HwPad::isPressed()
{
    return pressure > 0;
}

void HwPad::setPressure(unsigned char newPressure)
{
    pressure = newPressure;
}

char HwPad::getPadIndexWithBankWhenLastPressed()
{
    return padIndexWithBankWhenLastPressed;
}

unsigned char HwPad::getPressure()
{
    return pressure;
}

void HwPad::setPadIndexWithBankWhenLastPressed(char padIndexWithBank)
{
  padIndexWithBankWhenLastPressed = padIndexWithBank;
}
