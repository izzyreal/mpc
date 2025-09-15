#include "GlobalReleaseControls.hpp"

#include <Mpc.hpp>
#include <audiomidi/AudioMidiServices.hpp>
#include <audiomidi/SoundPlayer.hpp>
#include <audiomidi/EventHandler.hpp>
#include <hardware/Hardware.hpp>
#include <hardware/Led.hpp>
#include <hardware/TopPanel.hpp>
#include <hardware/HwPad.hpp>

#include <sequencer/FrameSeq.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/NoteEvent.hpp>

#include <lcdgui/screens/SequencerScreen.hpp>
#include <lcdgui/screens/DrumScreen.hpp>
#include <lcdgui/screens/window/TimingCorrectScreen.hpp>
#include <lcdgui/screens/window/StepEditOptionsScreen.hpp>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens;
using namespace mpc::controls;
using namespace mpc::sequencer;

GlobalReleaseControls::GlobalReleaseControls(mpc::Mpc& mpc)
	: BaseControls(mpc)
{
}

void GlobalReleaseControls::goTo()
{
	const auto controls = mpc.getControls();
	controls->setGoToPressed(false);
}

void GlobalReleaseControls::function(const int i)
{
	init();
	const auto controls = mpc.getControls();

	switch (i)
	{
	case 0:
		if (currentScreenName == "step-timing-correct")
        {
            ls->openScreen("step-editor");
        }
		break;
	case 2:
		controls->setF3Pressed(false);
		break;
	case 3:
		controls->setF4Pressed(false);
        break;
	case 4:
		controls->setF5Pressed(false);

		if (ls->getPreviousScreenName() == "load" && currentScreenName == "popup")
		{
            if (ls->getLastFocus("load") == "file" || ls->getLastFocus("load") == "view")
            {
                ls->openScreen("load");
                mpc.getAudioMidiServices()->getSoundPlayer()->enableStopEarly();
            }
		}

		break;
	case 5:
		controls->setF6Pressed(false);

		if (!sequencer.lock()->isPlaying() && currentScreenName != "sequencer")
        {
            sampler->finishBasicVoice();
        }

		if (currentScreenName == "track-mute")
		{
			if (!sequencer.lock()->isSoloEnabled())
            {
                ls->setCurrentBackground("track-mute");
            }

			sequencer.lock()->setSoloEnabled(sequencer.lock()->isSoloEnabled());
		}
		else if (ls->getPreviousScreenName() == "directory" && currentScreenName == "popup")
		{
			ls->openScreen("directory");
			mpc.getAudioMidiServices()->getSoundPlayer()->enableStopEarly();
		}
		break;
	}
}

void GlobalReleaseControls::simplePad(const int padIndexWithBank)
{
	init();

    // In case we've started playing a sound in one of the `soundScreens`, and
    // this sound is looping, we stop playing it.
    // We do it outside the body of the below if-statement, because the user may
    // have left the screen by the time they release the pad.
    mpc.getBasicPlayer().finishVoiceIfSoundIsLooping();

    if (collectionContainsCurrentScreen(soundScreens))
    {
        // We're certain we don't need further processing in this case, so we
        // return.
        return;
    }

	const auto controls = mpc.getControls();

	controls->unpressPad(padIndexWithBank);

	const auto playOnEvent = controls->retrievePlayNoteEvent(padIndexWithBank);
	
    if (!playOnEvent)
    {
        return;
    }

	handlePlayNoteOff(playOnEvent);
	
    const auto recordOnEvent = controls->retrieveRecordNoteEvent(padIndexWithBank);

	if (!recordOnEvent)
    {
        return;
    }

	if (sequencer.lock()->isRecordingOrOverdubbing() && controls->isErasePressed())
	{
		return;
	}

	if (sequencer.lock()->isRecordingOrOverdubbing())
	{
		track->finalizeNoteEventASync(recordOnEvent);
	}

	const bool recWithoutPlaying = controls->isRecMainWithoutPlaying();
	const bool stepRec = controls->isStepRecording();

	if (controls->isStepRecording()|| controls->isRecMainWithoutPlaying())
	{
        const auto metronomeOnlyTickPos = mpc.getAudioMidiServices()->getFrameSequencer()->getMetronomeOnlyTickPosition();
		auto newDuration = metronomeOnlyTickPos - recordOnEvent->getTick();

        recordOnEvent->setTick(mpc.getSequencer()->getTickPosition());

        const auto stepEditOptionsScreen = mpc.screens->get<StepEditOptionsScreen>("step-edit-options");
        const bool increment = stepEditOptionsScreen->isAutoStepIncrementEnabled();
        const bool durationIsTcValue = stepEditOptionsScreen->isDurationOfRecordedNotesTcValue();
        const int tcValuePercentage = stepEditOptionsScreen->getTcValuePercentage();

        const auto timingCorrectScreen = mpc.screens->get<TimingCorrectScreen>("timing-correct");

        const int stepLength = timingCorrectScreen->getNoteValueLengthInTicks();

        if (stepRec && durationIsTcValue)
        {
            newDuration = static_cast<int>(stepLength * (tcValuePercentage * 0.01));

            if (newDuration < 1)
            {
                newDuration = 1;
            }
        }
		
		bool durationHasBeenAdjusted = track->finalizeNoteEventSynced(recordOnEvent, newDuration);

		if ((durationHasBeenAdjusted && recWithoutPlaying) || (stepRec && increment))
		{
            if (!controls->arePadsPressed())
            {
                int nextPos = sequencer.lock()->getTickPosition() + stepLength;
                auto bar = sequencer.lock()->getCurrentBarIndex() + 1;
                nextPos = track->timingCorrectTick(0, bar, nextPos, stepLength, timingCorrectScreen->getSwing());
                auto lastTick = sequencer.lock()->getActiveSequence()->getLastTick();

                if (nextPos != 0 && nextPos < lastTick)
                {
                    sequencer.lock()->move(Sequencer::ticksToQuarterNotes(nextPos));
                }
                else
                {
                    sequencer.lock()->move(Sequencer::ticksToQuarterNotes(lastTick));
                }
            }
		}
	}

    if (!controls->arePadsPressed())
    {
        sequencer.lock()->stopMetronomeTrack();
    }
}

void GlobalReleaseControls::handlePlayNoteOff(const std::shared_ptr<mpc::sequencer::NoteOnEventPlayOnly>& onEvent)
{
	init();
	const auto noteOff = onEvent->getNoteOff();

	noteOff->setTick(-1);

	const auto drumScreen = mpc.screens->get<DrumScreen>("drum");

    const auto drum = collectionContainsCurrentScreen(samplerScreens) ?
                      std::optional<uint8_t>(drumScreen->getDrum()) : std::optional<uint8_t>();

	mpc.getEventHandler()->handle(noteOff, track.get(), drum);
}

void GlobalReleaseControls::overDub()
{
	const auto controls = mpc.getControls();
	controls->setOverDubPressed(false);
}

void GlobalReleaseControls::rec()
{
	const auto controls = mpc.getControls();
	controls->setRecPressed(false);
}

void GlobalReleaseControls::play()
{
    const auto controls = mpc.getControls();
    controls->setPlayPressed(false);
}

void GlobalReleaseControls::tap()
{
	const auto controls = mpc.getControls();
	controls->setTapPressed(false);

	if (sequencer.lock()->isRecordingOrOverdubbing())
    {
        sequencer.lock()->flushTrackNoteCache();
    }

    if (!controls->isNoteRepeatLocked())
    {
        const auto sequencerScreen = mpc.screens->get<SequencerScreen>("sequencer");
        sequencerScreen->releaseTap();
    }
}

void GlobalReleaseControls::shift()
{
	const auto controls = mpc.getControls();
	controls->setShiftPressed(false);
}

void GlobalReleaseControls::erase()
{
	const auto controls = mpc.getControls();
	controls->setErasePressed(false);
    const auto sequencerScreen = mpc.screens->get<SequencerScreen>("sequencer");
    sequencerScreen->releaseErase();
}

void GlobalReleaseControls::handlePadHitInTrimLoopZoneParamsScreens()
{
}

