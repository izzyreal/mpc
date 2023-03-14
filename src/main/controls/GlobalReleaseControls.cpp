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
#include <lcdgui/screens/StepEditorScreen.hpp>
#include <lcdgui/screens/DrumScreen.hpp>
#include <lcdgui/screens/window/TimingCorrectScreen.hpp>
#include <lcdgui/screens/window/Assign16LevelsScreen.hpp>
#include <lcdgui/screens/window/StepEditOptionsScreen.hpp>

#include "engine/Drum.hpp"

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens;
using namespace mpc::controls;

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

		if (!sequencer->isPlaying() && currentScreenName != "sequencer")
			sampler->finishBasicVoice();

		if (currentScreenName == "track-mute")
		{
			if (!sequencer->isSoloEnabled())
				ls->setCurrentBackground("track-mute");

			sequencer->setSoloEnabled(sequencer->isSoloEnabled());
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

  if (sequencer->isRecordingOrOverdubbing() && mpc.getControls()->isErasePressed())
  {
    return;
  }

	auto note = track->getBus() > 0 ? program->getPad(padIndexWithBank)->getNote() : padIndexWithBank + 35;

	generateNoteOff(note);
	bool posIsLastTick = sequencer->getTickPosition() == sequencer->getActiveSequence()->getLastTick();

	bool maybeRecWithoutPlaying = currentScreenName == "sequencer" && !posIsLastTick;
	bool stepRec = currentScreenName == "step-editor" && !posIsLastTick;

	if (stepRec || maybeRecWithoutPlaying)
	{
		auto newDuration = static_cast<int>(mpc.getAudioMidiServices()->getFrameSequencer()->getTickPosition());

        const auto stepEditOptionsScreen = mpc.screens->get<StepEditOptionsScreen>("step-edit-options");
        const auto increment = stepEditOptionsScreen->isAutoStepIncrementEnabled();
        const auto durationIsTcValue = stepEditOptionsScreen->isDurationOfRecordedNotesTcValue();
        const auto tcValuePercentage = stepEditOptionsScreen->getTcValuePercentage();

        const auto timingCorrectScreen = mpc.screens->get<TimingCorrectScreen>("timing-correct");
        const int noteVal = timingCorrectScreen->getNoteValue();

        const int stepLength = sequencer->getTickValues()[noteVal];

        if (stepRec && durationIsTcValue)
        {
            newDuration = static_cast<int>(stepLength * (tcValuePercentage * 0.01));

            if (newDuration < 1)
            {
                newDuration = 1;
            }
        }

		sequencer->stopMetronomeTrack();
		bool durationHasBeenAdjusted = track->adjustDurLastEvent(newDuration);

		if ( (durationHasBeenAdjusted && maybeRecWithoutPlaying) || (stepRec && increment))
		{
			int nextPos = sequencer->getTickPosition() + stepLength;
			auto bar = sequencer->getCurrentBarIndex() + 1;
			nextPos = track->timingCorrectTick(0, bar, nextPos, stepLength);
			auto lastTick = sequencer->getActiveSequence()->getLastTick();

			if (nextPos != 0 && nextPos < lastTick)
			{
				nextPos = track->swingTick(nextPos, noteVal, timingCorrectScreen->getSwing());
				sequencer->move(nextPos);
			}
			else
			{
				sequencer->move(lastTick);
			}
		}
	}
}

void GlobalReleaseControls::generateNoteOff(int note)
{
    init();

	auto assign16LevelsScreen = mpc.screens->get<Assign16LevelsScreen>("assign-16-levels");

	if (mpc.getHardware()->getTopPanel()->isSixteenLevelsEnabled())
		note = assign16LevelsScreen->getNote();

	if (sequencer->isRecordingOrOverdubbing())
	{
        track->recordNoteOffNow(note);
	}

    auto noteEvent = std::make_shared<mpc::sequencer::NoteEvent>(note);
    noteEvent->setVelocity(0);
    noteEvent->setDuration(0);
    noteEvent->setTick(-1);

    char drum = -1;
    auto drumScreen = mpc.screens->get<DrumScreen>("drum");

    if (collectionContainsCurrentScreen(samplerScreens))
    {
        drum = drumScreen->drum;
    }

    mpc.getEventHandler()->handle(noteEvent, track.get(), drum);
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

	if (sequencer->isRecordingOrOverdubbing())
		sequencer->flushTrackNoteCache();

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
	init();

	if (currentScreenName == "step-editor" && param.length() == 2)
	{
		auto eventNumber = stoi(param.substr(1, 2));
		auto stepEditorScreen = mpc.screens->get<StepEditorScreen>("step-editor");
		auto res = eventNumber + stepEditorScreen->getYOffset();
		stepEditorScreen->finalizeSelection(res);
	}
}

void GlobalReleaseControls::erase()
{
	auto controls = mpc.getControls();
	controls->setErasePressed(false);
    auto sequencerScreen = mpc.screens->get<SequencerScreen>("sequencer");
    sequencerScreen->releaseErase();
}
