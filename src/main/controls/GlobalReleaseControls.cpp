#include "GlobalReleaseControls.hpp"

#include <Mpc.hpp>

#include "controls/PadReleaseContext.h"
#include "hardware2/Hardware2.h"

#include <audiomidi/AudioMidiServices.hpp>
#include <audiomidi/SoundPlayer.hpp>
#include <audiomidi/EventHandler.hpp>

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

void GlobalReleaseControls::goTo(mpc::Mpc &mpc)
{
	const auto controls = mpc.getControls();
	controls->setGoToPressed(false);
}

void GlobalReleaseControls::function(mpc::Mpc &mpc, const int i)
{
	const auto controls = mpc.getControls();
    const auto currentScreenName = BaseControls::getCurrentScreenName(mpc);

	switch (i)
	{
	case 0:
		if (currentScreenName == "step-timing-correct")
        {
            mpc.getLayeredScreen()->openScreen("step-editor");
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

		if (BaseControls::getPreviousScreenName(mpc) == "load" && currentScreenName == "popup")
		{
            if (const auto lastFocusedField = BaseControls::getLastFocusedField(mpc, "load"); lastFocusedField == "file" || lastFocusedField == "view")
            {
                mpc.getLayeredScreen()->openScreen("load");
                mpc.getAudioMidiServices()->getSoundPlayer()->enableStopEarly();
            }
		}

		break;
	case 5:
    {
        auto sequencer = mpc.getSequencer();
        auto sampler = mpc.getSampler();

		controls->setF6Pressed(false);

		if (!sequencer->isPlaying() && currentScreenName != "sequencer")
        {
            sampler->finishBasicVoice();
        }

		if (currentScreenName == "track-mute")
		{
			if (!sequencer->isSoloEnabled())
            {
                mpc.getLayeredScreen()->setCurrentBackground("track-mute");
            }

			sequencer->setSoloEnabled(sequencer->isSoloEnabled());
		}
		else if (mpc.getLayeredScreen()->getPreviousScreenName() == "directory" && currentScreenName == "popup")
		{
			mpc.getLayeredScreen()->openScreen("directory");
			mpc.getAudioMidiServices()->getSoundPlayer()->enableStopEarly();
		}
		break;
    }
	}
}

void GlobalReleaseControls::simplePad(PadReleaseContext &ctx)
{
    // In case we've started playing a sound in one of the `soundScreens`, and
    // this sound is looping, we stop playing it.
    // We do it outside the body of the below if-statement, because the user may
    // have left the screen by the time they release the pad.
    ctx.finishBasicVoiceIfSoundIsLooping();

    if (ctx.currentScreenIsSoundScreen)
    {
        // We're certain we don't need further processing in this case, so we
        // return.
        return;
    }

    ctx.controlsUnpressPad(ctx.padIndexWithBank);

    if (!ctx.playNoteEvent)
    {
        return;
    }

	const auto noteOff = ctx.playNoteEvent->getNoteOff();

	noteOff->setTick(-1);

    const auto drum = ctx.currentScreenIsSamplerScreen ?
                      std::optional<uint8_t>(ctx.drumScreenSelectedDrum) : std::optional<uint8_t>();

	ctx.eventHandler->handle(noteOff, ctx.activeTrack.get(), drum);

	if (!ctx.recordOnEvent)
    {
        return;
    }

	if (ctx.sequencerIsRecordingOrOverdubbing && ctx.isErasePressed)
	{
		return;
	}

	if (ctx.sequencerIsRecordingOrOverdubbing)
	{
		ctx.activeTrack->finalizeNoteEventASync(ctx.recordOnEvent);
	}

	if (ctx.isStepRecording || ctx.isRecMainWithoutPlaying)
	{
		auto newDuration = ctx.metronomeOnlyTickPosition - ctx.recordOnEvent->getTick();

        ctx.recordOnEvent->setTick(ctx.sequencerTickPosition);

        if (ctx.isStepRecording && ctx.isDurationOfRecordedNotesTcValue)
        {
            newDuration = static_cast<int>(ctx.noteValueLengthInTicks * (ctx.tcValuePercentage * 0.01));

            if (newDuration < 1)
            {
                newDuration = 1;
            }
        }
		
		const bool durationHasBeenAdjusted = ctx.activeTrack->finalizeNoteEventSynced(ctx.recordOnEvent, newDuration);

		if ((durationHasBeenAdjusted && ctx.isRecMainWithoutPlaying) || (ctx.isStepRecording && ctx.isAutoStepIncrementEnabled))
		{
            if (!ctx.arePadsPressed())
            {
                int nextPos = ctx.sequencerTickPosition + ctx.noteValueLengthInTicks;
                auto bar = ctx.currentBarIndex + 1;
                nextPos = ctx.activeTrack->timingCorrectTick(0, bar, nextPos, ctx.noteValueLengthInTicks, ctx.swing);
                auto lastTick = ctx.sequencerGetActiveSequenceLastTick();

                if (nextPos != 0 && nextPos < lastTick)
                {
                    ctx.sequencerMoveToQuarterNotePosition(Sequencer::ticksToQuarterNotes(nextPos));
                }
                else
                {
                    ctx.sequencerMoveToQuarterNotePosition(Sequencer::ticksToQuarterNotes(lastTick));
                }
            }
		}
	}

    if (!ctx.arePadsPressed())
    {
        ctx.sequencerStopMetronomeTrack();
    }
}

void GlobalReleaseControls::overDub(mpc::Mpc &mpc)
{
	const auto controls = mpc.getControls();
	controls->setOverDubPressed(false);
    mpc.getHardware2()->getLed("overdub")->setEnabled(mpc.getSequencer()->isOverDubbing());
}

void GlobalReleaseControls::rec(mpc::Mpc &mpc)
{
	const auto controls = mpc.getControls();
	controls->setRecPressed(false);
    mpc.getHardware2()->getLed("rec")->setEnabled(mpc.getSequencer()->isRecording());
}

void GlobalReleaseControls::play(mpc::Mpc &mpc)
{
    const auto controls = mpc.getControls();
    controls->setPlayPressed(false);
}

void GlobalReleaseControls::tap(mpc::Mpc &mpc)
{
	const auto controls = mpc.getControls();
	controls->setTapPressed(false);

	if (mpc.getSequencer()->isRecordingOrOverdubbing())
    {
        mpc.getSequencer()->flushTrackNoteCache();
    }

    if (!controls->isNoteRepeatLocked())
    {
        const auto sequencerScreen = mpc.screens->get<SequencerScreen>("sequencer");
        sequencerScreen->releaseTap();
    }
}

void GlobalReleaseControls::shift(mpc::Mpc &mpc)
{
	const auto controls = mpc.getControls();
	controls->setShiftPressed(false);
}

void GlobalReleaseControls::erase(mpc::Mpc &mpc)
{
	const auto controls = mpc.getControls();
	controls->setErasePressed(false);
    const auto sequencerScreen = mpc.screens->get<SequencerScreen>("sequencer");
    sequencerScreen->releaseErase();
}

