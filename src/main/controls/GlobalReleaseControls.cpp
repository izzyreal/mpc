#include "GlobalReleaseControls.hpp"

#include <controls/Controls.hpp>

#include <Mpc.hpp>
#include <audiomidi/AudioMidiServices.hpp>
#include <audiomidi/SoundPlayer.hpp>
#include <audiomidi/EventHandler.hpp>
#include <hardware/Hardware.hpp>
#include <hardware/Led.hpp>
#include <hardware/TopPanel.hpp>

#include <sampler/Pad.hpp>
#include <sampler/Program.hpp>
#include <sampler/Sampler.hpp>

#include <sequencer/FrameSeq.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/NoteEvent.hpp>
#include <sequencer/Sequencer.hpp>

#include <lcdgui/Screens.hpp>
#include <lcdgui/screens/window/TimingCorrectScreen.hpp>
#include <lcdgui/screens/StepEditorScreen.hpp>
#include <lcdgui/screens/window/Assign16LevelsScreen.hpp>

#include <mpc/MpcSoundPlayerChannel.hpp>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens;
using namespace mpc::controls;
using namespace std;

GlobalReleaseControls::GlobalReleaseControls(mpc::Mpc& mpc)
	: BaseControls(mpc)
{
}

void GlobalReleaseControls::goTo()
{
	auto controls = mpc.getControls().lock();
	controls->setGoToPressed(false);
}

void GlobalReleaseControls::function(int i)
{
	init();
	auto controls = mpc.getControls().lock();
	
	switch (i)
	{
	case 0:
		if (currentScreenName.compare("step-timing-correct") == 0)
			ls.lock()->openScreen("step-editor");

		break;
	case 2:
		controls->setF3Pressed(false);
		
		if (currentScreenName.compare("load-a-sound") == 0)
			sampler.lock()->finishBasicVoice();

		break;
	case 3:
		controls->setF4Pressed(false);
		
		if (currentScreenName.compare("keep-or-retry") == 0)
			sampler.lock()->finishBasicVoice();

		break;
	case 4:
		controls->setF5Pressed(false);

		if (ls.lock()->getPreviousScreenName().compare("load") == 0 && currentScreenName.compare("popup") == 0)
		{
			ls.lock()->openScreen("load");
			mpc.getAudioMidiServices().lock()->getSoundPlayer().lock()->enableStopEarly();
		}

		break;
	case 5:
		controls->setF6Pressed(false);

		if (!sequencer.lock()->isPlaying() && currentScreenName.compare("sequencer") != 0)
			sampler.lock()->finishBasicVoice();
		
		if (currentScreenName.compare("track-mute") == 0)
		{
			if (!sequencer.lock()->isSoloEnabled())
				ls.lock()->setCurrentBackground("track-mute");

			sequencer.lock()->setSoloEnabled(sequencer.lock()->isSoloEnabled());
		}
		else if (ls.lock()->getPreviousScreenName().compare("directory") == 0 && currentScreenName.compare("popup") == 0)
		{
			ls.lock()->openScreen("directory");
			mpc.getAudioMidiServices().lock()->getSoundPlayer().lock()->enableStopEarly();
		}
		break;
	}
}

void GlobalReleaseControls::simplePad(int i)
{
	init();
	auto bank = mpc.getBank();
	
	auto controls = mpc.getControls().lock();
	
	if (controls->getPressedPads()->find(i) == controls->getPressedPads()->end())
		return;
	
	controls->getPressedPads()->erase(controls->getPressedPads()->find(i));

	auto lTrk = track.lock();
	auto note = lTrk->getBus() > 0 ? program.lock()->getPad(i + (bank * 16))->getNote() : i + (bank * 16) + 35;

	generateNoteOff(note);
	bool posIsLastTick = sequencer.lock()->getTickPosition() == sequencer.lock()->getActiveSequence().lock()->getLastTick();

	bool maybeRecWithoutPlaying = currentScreenName.compare("sequencer") == 0 && !posIsLastTick;
	bool stepRec = currentScreenName.compare("step-editor") == 0 && !posIsLastTick;
	
	if (stepRec || maybeRecWithoutPlaying)
	{
		auto newDur = static_cast<int>(mpc.getAudioMidiServices().lock()->getFrameSequencer().lock()->getTickPosition());
		sequencer.lock()->stopMetronomeTrack();
		bool durationHasBeenAdjusted = lTrk->adjustDurLastEvent(newDur);
		
		if (durationHasBeenAdjusted && maybeRecWithoutPlaying)
		{
			auto timingCorrectScreen = mpc.screens->get<TimingCorrectScreen>("timing-correct");
			auto noteValue = timingCorrectScreen->getNoteValue();

			int noteVal = timingCorrectScreen->getNoteValue();
			int stepLength = sequencer.lock()->getTickValues()[noteVal];
			int nextPos = sequencer.lock()->getTickPosition() + stepLength;
			auto bar = sequencer.lock()->getCurrentBarIndex() + 1;
			nextPos = lTrk->timingCorrectTick(0, bar, nextPos, stepLength);
			auto lastTick = sequencer.lock()->getActiveSequence().lock()->getLastTick();
			
			if (nextPos != 0 && nextPos < lastTick)
			{
				nextPos = lTrk->swingTick(nextPos, noteVal, timingCorrectScreen->getSwing());
				sequencer.lock()->move(nextPos);
			}
			else
			{
				sequencer.lock()->move(lastTick);
			}
		}
	}
}

void GlobalReleaseControls::generateNoteOff(int note)
{
    init();
	auto lTrk = track.lock();
    
	auto assign16LevelsScreen = mpc.screens->get<Assign16LevelsScreen>("assign-16-levels");

	if (mpc.getHardware().lock()->getTopPanel().lock()->isSixteenLevelsEnabled())
		note = assign16LevelsScreen->getNote();

	if (sequencer.lock()->isRecordingOrOverdubbing())
	{
		mpc::sequencer::NoteEvent noteOff;
		noteOff.setNote(note);
		noteOff.setVelocity(0);
		noteOff.setTick(sequencer.lock()->getTickPosition());
		lTrk->recordNoteOff(noteOff);
	}

    auto noteEvent = make_shared<mpc::sequencer::NoteEvent>(note);
    noteEvent->setVelocity(0);
    noteEvent->setDuration(0);
    noteEvent->setTick(-1);
    mpc.getEventHandler().lock()->handle(noteEvent, lTrk.get());
}

void GlobalReleaseControls::overDub()
{
	auto controls = mpc.getControls().lock();
	controls->setOverDubPressed(false);
    init();
	auto hw = mpc.getHardware().lock();
	hw->getLed("overdub").lock()->light(sequencer.lock()->isOverDubbing());
}

void GlobalReleaseControls::rec()
{
	auto controls = mpc.getControls().lock();
	controls->setRecPressed(false);
    init();
	auto hw = mpc.getHardware().lock();
	hw->getLed("rec").lock()->light(sequencer.lock()->isRecording());
}

void GlobalReleaseControls::tap()
{
	auto controls = mpc.getControls().lock();
	controls->setTapPressed(false);

	if (sequencer.lock()->isRecordingOrOverdubbing())
		sequencer.lock()->flushTrackNoteCache();
}

void GlobalReleaseControls::shift()
{
	auto controls = mpc.getControls().lock();
	controls->setShiftPressed(false);
	init();

	if (currentScreenName.compare("step-editor") == 0 && param.length() == 2)
	{
		auto eventNumber = stoi(param.substr(1, 2));
		auto stepEditorScreen = dynamic_pointer_cast<StepEditorScreen>(mpc.screens->getScreenComponent("step-editor"));
		auto res = eventNumber + stepEditorScreen->getYOffset();
		stepEditorScreen->finalizeSelection(res);
	}
}

void GlobalReleaseControls::erase()
{
	auto controls = mpc.getControls().lock();
	controls->setErasePressed(false);
}
