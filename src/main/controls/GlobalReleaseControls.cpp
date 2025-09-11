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
	auto controls = mpc.getControls();
	controls->setGoToPressed(false);
}

void GlobalReleaseControls::function(int i)
{
	init();
	auto controls = mpc.getControls();

	switch (i)
	{
	case 0:
		if (currentScreenName == "step-timing-correct")
			ls->openScreen("step-editor");

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
			sampler->finishBasicVoice();

		if (currentScreenName == "track-mute")
		{
			if (!sequencer.lock()->isSoloEnabled())
				ls->setCurrentBackground("track-mute");

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

void GlobalReleaseControls::simplePad(int padIndexWithBank)
{
	init();

	auto controls = mpc.getControls();

	controls->unpressPad(padIndexWithBank);

	auto playOnEvent = controls->retrievePlayNoteEvent(padIndexWithBank);
	if (!playOnEvent) return;
	handlePlayNoteOff(playOnEvent);
	auto recordOnEvent = controls->retrieveRecordNoteEvent(padIndexWithBank);
	if (!recordOnEvent) return;

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

void mpc::controls::GlobalReleaseControls::handlePlayNoteOff(const std::shared_ptr<mpc::sequencer::NoteOnEventPlayOnly>& onEvent)
{
	init();
	std::shared_ptr<mpc::sequencer::NoteOffEvent> off_event = onEvent->getNoteOff();

	off_event->setTick(-1);

	auto drumScreen = mpc.screens->get<DrumScreen>("drum");

    const auto drum = collectionContainsCurrentScreen(samplerScreens) ?
                      std::optional<uint8_t>(drumScreen->getDrum()) : std::optional<uint8_t>();

	mpc.getEventHandler()->handle(off_event, track.get(), drum);
}

void GlobalReleaseControls::overDub()
{
	auto controls = mpc.getControls();
	controls->setOverDubPressed(false);
}

void GlobalReleaseControls::rec()
{
	auto controls = mpc.getControls();
	controls->setRecPressed(false);
}

void GlobalReleaseControls::play()
{
    auto controls = mpc.getControls();
    controls->setPlayPressed(false);
}

void GlobalReleaseControls::tap()
{
	auto controls = mpc.getControls();
	controls->setTapPressed(false);

	if (sequencer.lock()->isRecordingOrOverdubbing())
		sequencer.lock()->flushTrackNoteCache();

    if (!controls->isNoteRepeatLocked())
    {
        auto sequencerScreen = mpc.screens->get<SequencerScreen>("sequencer");
        sequencerScreen->releaseTap();
    }
}

void GlobalReleaseControls::shift()
{
	auto controls = mpc.getControls();
	controls->setShiftPressed(false);
}

void GlobalReleaseControls::erase()
{
	auto controls = mpc.getControls();
	controls->setErasePressed(false);
    auto sequencerScreen = mpc.screens->get<SequencerScreen>("sequencer");
    sequencerScreen->releaseErase();
}
