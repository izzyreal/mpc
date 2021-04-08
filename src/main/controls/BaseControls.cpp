#include "BaseControls.hpp"

#include <Mpc.hpp>

#include <hardware/Hardware.hpp>
#include <hardware/Led.hpp>
#include <hardware/HwSlider.hpp>
#include <hardware/HwPad.hpp>
#include <hardware/TopPanel.hpp>

#include <Paths.hpp>

#include <audiomidi/AudioMidiServices.hpp>
#include <audiomidi/EventHandler.hpp>

#include <disk/AbstractDisk.hpp>

#include <lcdgui/Field.hpp>
#include <lcdgui/Layer.hpp>

#include <sampler/Pad.hpp>
#include <sampler/Program.hpp>
#include <sampler/Sampler.hpp>
#include <sampler/PgmSlider.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/NoteEvent.hpp>
#include <sequencer/Sequencer.hpp>

#include <lcdgui/screens/window/Assign16LevelsScreen.hpp>
#include <lcdgui/screens/window/TimingCorrectScreen.hpp>
#include <lcdgui/screens/window/EditSoundScreen.hpp>
#include <lcdgui/screens/window/DirectoryScreen.hpp>
#include <lcdgui/screens/window/NameScreen.hpp>
#include <lcdgui/screens/window/VmpcDirectToDiskRecorderScreen.hpp>
#include <lcdgui/screens/LoadScreen.hpp>
#include <lcdgui/Screens.hpp>

#include <Util.hpp>

#include <mpc/MpcSoundPlayerChannel.hpp>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::controls;
using namespace mpc::sequencer;
using namespace std;

BaseControls::BaseControls(mpc::Mpc& _mpc)
	: mpc (_mpc),
	ls (_mpc.getLayeredScreen()),
	sampler (_mpc.getSampler()),
	sequencer (_mpc.getSequencer())
{
}

void BaseControls::init()
{
	currentScreenName = ls.lock()->getCurrentScreenName();
	param = ls.lock()->getFocus();
	activeField = ls.lock()->getFocusedLayer().lock()->findField(param);
	
	if (sequencer.lock()->getActiveTrack().lock())
	{
		track = sequencer.lock()->getActiveTrack();

		if (track.lock()->getBus() != 0)
		{
			mpcSoundPlayerChannel = sampler.lock()->getDrum(track.lock()->getBus() - 1);
			program = sampler.lock()->getProgram(mpcSoundPlayerChannel->getProgram());
		}
	}
}

void BaseControls::left()
{
	init();
	
	if (!activeField.lock())
		return;
	
	if (param.compare("dummy") == 0)
		return;
	
	ls.lock()->transferLeft();
}

void BaseControls::right()
{
	init();
	
	if (!activeField.lock())
		return;
	
	if (param.compare("dummy") == 0)
		return;
	
	ls.lock()->transferRight();
}

void BaseControls::up()
{
    init();

	if (!activeField.lock())
		return;

	if (param.compare("dummy") == 0)
		return;

	ls.lock()->transferUp();
}

void BaseControls::down()
{
	init();

	if (!activeField.lock())
		return;

	if (param.compare("dummy") == 0)
		return;

	ls.lock()->transferDown();
}

void BaseControls::function(int i)
{
	init();

	switch (i)
	{
	case 3:
		if (ls.lock()->getFocusedLayerIndex() == 1)
		{
			if (currentScreenName.compare("sequence") == 0)
			{
				ls.lock()->setPreviousScreenName("sequencer");
			}
			else if (currentScreenName.compare("midi-input") == 0)
			{
				ls.lock()->setPreviousScreenName("sequencer");
			}
			else if (currentScreenName.compare("midi-output") == 0)
			{
				ls.lock()->setPreviousScreenName("sequencer");
			}
			else if (currentScreenName.compare("edit-sound") == 0)
			{
				auto editSoundScreen = mpc.screens->get<EditSoundScreen>("edit-sound");
				ls.lock()->setPreviousScreenName(editSoundScreen->getReturnToScreenName());
			}
			else if (currentScreenName.compare("sound") == 0)
			{
				ls.lock()->setPreviousScreenName(sampler.lock()->getPreviousScreenName());
			}
			else if (currentScreenName.compare("program") == 0)
			{
				ls.lock()->setPreviousScreenName(mpc.getPreviousSamplerScreenName());
			}
			else if (currentScreenName.compare("name") == 0)
			{
				auto nameScreen = mpc.screens->get<NameScreen>("name");
				nameScreen->editing = false;
				ls.lock()->setLastFocus("name", "0");
			}
			else if (currentScreenName.compare("directory") == 0)
			{
				auto directoryScreen = mpc.screens->get<DirectoryScreen>("directory");
				ls.lock()->setPreviousScreenName(directoryScreen->previousScreenName);
			}
			
			if (ls.lock()->getPreviousScreenName().compare("load") == 0)
			{
				auto loadScreen = mpc.screens->get<LoadScreen>("load");

				if (loadScreen->fileLoad + 1 > mpc.getDisk().lock()->getFiles().size())
					loadScreen->fileLoad = 0; // Can we avoid this? Who's leaving fileLoad in a bad state?
			}
		}
		
		if (ls.lock()->getFocusedLayerIndex() == 1 || ls.lock()->getFocusedLayerIndex() == 2 || ls.lock()->getFocusedLayerIndex() == 3)
			ls.lock()->openScreen(ls.lock()->getPreviousScreenName());
		break;
	}
}

void BaseControls::openWindow()
{
}

void BaseControls::turnWheel(int i)
{
}

void BaseControls::pad(int i, int velo, bool triggeredByRepeat, int tick)
{
	init();

	auto controls = mpc.getControls().lock();
    
	if (mpc.getHardware().lock()->getTopPanel().lock()->isFullLevelEnabled())
		velo = 127;

	if (controls->getPressedPads()->find(i) == controls->getPressedPads()->end())
	{
		controls->getPressedPads()->emplace(i);
		(*controls->getPressedPadVelos())[i] = velo;
	}
	else
	{
		if (!(controls->isTapPressed() && sequencer.lock()->isPlaying()))
			return;
	}
    
    if (sequencer.lock()->isRecordingOrOverdubbing() && mpc.getControls().lock()->isErasePressed())
        return;

    if (controls->isNoteRepeatLocked() && !triggeredByRepeat)
        return;
    
	auto note = track.lock()->getBus() > 0 ? program.lock()->getPad(i + (mpc.getBank() * 16))->getNote() : i + (mpc.getBank() * 16) + 35;
	auto velocity = velo;
	auto pad = i + (mpc.getBank() * 16);

	auto assign16LevelsScreen = mpc.screens->get<Assign16LevelsScreen>("assign-16-levels");

	if (mpc.getHardware().lock()->getTopPanel().lock()->isSixteenLevelsEnabled())
	{
		if (assign16LevelsScreen->getParameter() == 0)
		{
			note = assign16LevelsScreen->getNote();
			velocity = (int)(i * (127.0 / 16.0));
			
			if (velocity == 0)
				velocity = 1;
		}
	}
	else
	{
		if (currentScreenName.compare("program-params") == 0)
		{
			if (note > 34)
				mpc.setPadAndNote(pad, note);
		}
		else if (currentScreenName.compare("copy-note-parameters") != 0)
		{
			mpc.setPadAndNote(pad, note);
		}
	}

	if (controls->isTapPressed() && sequencer.lock()->isPlaying())
	{
		if (triggeredByRepeat)
			generateNoteOn(note, velocity, tick);
	}
	else
	{
		generateNoteOn(note, velocity, -1);
	}
}

void BaseControls::generateNoteOn(int note, int padVelo, int tick)
{
	init();

	auto timingCorrectScreen = mpc.screens->get<TimingCorrectScreen>("timing-correct");
	auto assign16LevelsScreen = mpc.screens->get<Assign16LevelsScreen>("assign-16-levels");

	auto _16l_type = assign16LevelsScreen->getType();
	auto _16l_key = assign16LevelsScreen->getOriginalKeyPad();
	auto _16l_note = assign16LevelsScreen->getNote();
	auto _16l_param = assign16LevelsScreen->getParameter();

	auto pgm = program.lock();
	auto seq = sequencer.lock();
	auto trk = track.lock();

	auto padIndex = program.lock()->getPadIndexFromNote(note) - (mpc.getBank() * 16);

	bool isSliderNote = pgm && pgm->getSlider()->getNote() == note;

	bool posIsLastTick = seq->getTickPosition() == seq->getActiveSequence().lock()->getLastTick();

	bool step = currentScreenName.compare("step-editor") == 0 && !posIsLastTick;

	auto tc_note = timingCorrectScreen->getNoteValue();
	auto tc_swing = timingCorrectScreen->getSwing();

	bool recMainWithoutPlaying = currentScreenName.compare("sequencer") == 0 &&
		!seq->isPlaying() &&
		mpc.getControls().lock()->isRecPressed() &&
		tc_note != 0 &&
		!posIsLastTick;

	shared_ptr<NoteEvent> recordedEvent;

	auto setEventAttributes = [&](shared_ptr<NoteEvent> event)
	{
		event->setVelocity(padVelo);
		event->setDuration(step ? 1 : -1);

		if (mpc.getHardware().lock()->getTopPanel().lock()->isSixteenLevelsEnabled()) {
			if (_16l_param == 1)
			{
				if (_16l_type == 0)
				{
					auto diff = padIndex - _16l_key;
					auto candidate = 64 + (diff * 5);

					if (candidate > 124)
						candidate = 124;
					else if (candidate < 4)
						candidate = 4;

					event->setVariationValue(candidate);
				}
				else
				{
					event->setVariationValue(static_cast<int>(floor(100 / 16.0) * padIndex));
				}
			}

			event->setNote(_16l_note);
			event->setVariationTypeNumber(_16l_type);
		}

		if (isSliderNote)
			setSliderNoteVar(event.get(), program);
	};

	if (seq->isRecordingOrOverdubbing() || step || recMainWithoutPlaying)
	{
		if (step)
		{
			recordedEvent = trk->addNoteEvent(seq->getTickPosition(), note).lock();
		}
		else if (recMainWithoutPlaying)
		{
			recordedEvent = trk->addNoteEvent(seq->getTickPosition(), note).lock();
			int stepLength = seq->getTickValues()[tc_note];

			if (stepLength != 1)
			{
				int bar = seq->getCurrentBarIndex() + 1;
				trk->timingCorrect(0, bar, recordedEvent.get(), stepLength);

				vector<weak_ptr<Event>> events{ recordedEvent };
				trk->swing(events, tc_note, tc_swing, vector<int>{0, 127});

				if (recordedEvent->getTick() != seq->getTickPosition())
					seq->move(recordedEvent->getTick());
			}
		}
		else
		{
			recordedEvent = trk->recordNoteOn().lock();

			if (recordedEvent)
				recordedEvent->setNote(note);
		}

		if (recordedEvent)
			setEventAttributes(recordedEvent);

		if (step || recMainWithoutPlaying)
			seq->playMetronomeTrack();
	}

	auto playableEvent = make_shared<NoteEvent>(note);
	setEventAttributes(playableEvent);
	playableEvent->setDuration(0);
	playableEvent->setTick(tick);

	mpc.getEventHandler().lock()->handle(playableEvent, trk.get());
}

bool BaseControls::isTypable()
{
	if (typableParams.size() == 0)
		return false;

	for (auto str : typableParams)
	{
		if (str.compare(param) == 0)
			return true;
	}

	return false;
}

void BaseControls::numpad(int i)
{
	init();

	auto controls = mpc.getControls().lock();
	
	if (!controls->isShiftPressed())
	{
		auto field = ls.lock()->getFocusedLayer().lock()->findField(param).lock();

		if (isTypable())
		{
			if (!field->isTypeModeEnabled())
				field->enableTypeMode();

			field->type(i);
		}
	}

	auto disk = mpc.getDisk().lock();

	if (controls->isShiftPressed())
	{
		switch (i)
		{
		case 0:
			ls.lock()->openScreen("vmpc-settings");
			break;
		case 1:
			if (sequencer.lock()->isPlaying())
				return;

			ls.lock()->openScreen("song");
			break;
		case 2:
			ls.lock()->openScreen("punch");
			break;
		case 3:
		{
			if (sequencer.lock()->isPlaying())
				break;

			disk->initFiles();

			auto loadScreen = mpc.screens->get<LoadScreen>("load");

			if (loadScreen->fileLoad + 1 > (int)(disk->getFiles().size()))
				loadScreen->fileLoad = (int)(disk->getFiles().size() - 1); // Same here, can we avoid this?

			ls.lock()->openScreen("load");
			break;
		}
		case 4:
			if (sequencer.lock()->isPlaying())
				break;

			ls.lock()->openScreen("sample");
			break;
		case 5:
			if (sequencer.lock()->isPlaying())
				break;

			ls.lock()->openScreen("trim");
			break;
		case 6:
			ls.lock()->openScreen("select-drum");
			break;
		case 7:
			ls.lock()->openScreen("select-mixer-drum");
			break;
		case 8:
			if (sequencer.lock()->isPlaying())
				break;

			ls.lock()->openScreen("others");
			break;
		case 9:
			if (sequencer.lock()->isPlaying())
				break;

			ls.lock()->openScreen("sync");
			break;
		}

	}
}

void BaseControls::pressEnter()
{
	init();

	auto controls = mpc.getControls().lock();

	if (controls->isShiftPressed())
		ls.lock()->openScreen("save");
}

void BaseControls::rec()
{
	auto controls = mpc.getControls().lock();

	if (controls->isRecPressed())
		return;
	
	controls->setRecPressed(true);
    
	init();

	if (allowPlay()) return;

	auto hw = mpc.getHardware().lock();

	if (!sequencer.lock()->isPlaying())
	{
		hw->getLed("rec").lock()->light(true);
    }
	else
	{
        if (sequencer.lock()->isRecordingOrOverdubbing())
		{
            sequencer.lock()->setRecording(false);
            sequencer.lock()->setOverdubbing(false);
			hw->getLed("rec").lock()->light(false);
			hw->getLed("overdub").lock()->light(false);
        }
    }

	if (find(begin(allowTransportScreens), end(allowTransportScreens), currentScreenName) == end(allowTransportScreens))
		ls.lock()->openScreen("sequencer");
}

void BaseControls::overDub()
{
	auto controls = mpc.getControls().lock();
	controls->setOverDubPressed(true);
	init();

	if (allowPlay())
		return;

	auto hw = mpc.getHardware().lock();

	if (!sequencer.lock()->isPlaying())
	{
		hw->getLed("overdub").lock()->light(true);
	}
	else
	{
		if (sequencer.lock()->isRecordingOrOverdubbing())
		{
			sequencer.lock()->setRecording(false);
			sequencer.lock()->setOverdubbing(false);
			hw->getLed("rec").lock()->light(false);
			hw->getLed("overdub").lock()->light(false);
		}
	}

	if (find(begin(allowTransportScreens), end(allowTransportScreens), currentScreenName) == end(allowTransportScreens))
		ls.lock()->openScreen("sequencer");
}

void BaseControls::stop()
{
	init();
    
	auto vmpcDirectToDiskRecorderScreen = mpc.screens->get<VmpcDirectToDiskRecorderScreen>("vmpc-direct-to-disk-recorder");
	auto ams = mpc.getAudioMidiServices().lock();
	auto controls = mpc.getControls().lock();
    
    if (controls->isNoteRepeatLocked())
        controls->setNoteRepeatLocked(false);

	if (ams->isBouncing() && (vmpcDirectToDiskRecorderScreen->record != 4 || controls->isShiftPressed()))
		ams->stopBouncingEarly();

	sequencer.lock()->stop();

	if (!allowTransport() && !allowPlay())
	{
		ls.lock()->openScreen("sequencer");
	}
}

void BaseControls::play()
{
	init();
	auto controls = mpc.getControls().lock();
	auto hw = mpc.getHardware().lock();

	if (sequencer.lock()->isPlaying())
	{
		if (controls->isRecPressed() && !sequencer.lock()->isOverDubbing())
		{
			sequencer.lock()->setOverdubbing(false);
			sequencer.lock()->setRecording(true);
			hw->getLed("overdub").lock()->light(false);
			hw->getLed("rec").lock()->light(true);
		}
		else if (controls->isOverDubPressed() && !sequencer.lock()->isRecording())
		{
			sequencer.lock()->setOverdubbing(true);
			sequencer.lock()->setRecording(false);
			hw->getLed("overdub").lock()->light(true);
			hw->getLed("rec").lock()->light(false);
		}
	}
	else
	{
		if (controls->isRecPressed())
		{
			if (find(begin(allowTransportScreens), end(allowTransportScreens), currentScreenName) == end(allowTransportScreens))
				ls.lock()->openScreen("sequencer");

			sequencer.lock()->rec();
		}
		else if (controls->isOverDubPressed())
		{
			if (find(begin(allowTransportScreens), end(allowTransportScreens), currentScreenName) == end(allowTransportScreens))
				ls.lock()->openScreen("sequencer");

			sequencer.lock()->overdub();
		}
		else {
			if (controls->isShiftPressed() && !mpc.getAudioMidiServices().lock()->isBouncing())
			{
				ls.lock()->openScreen("vmpc-direct-to-disk-recorder");
			}
			else
			{
				if (!allowTransport() && !allowPlay())
				{
					ls.lock()->openScreen("sequencer");
				}

				sequencer.lock()->setSongModeEnabled(currentScreenName.compare("song") == 0);
				sequencer.lock()->play();
			}
		}
	}
}

void BaseControls::playStart()
{
	init();
	auto hw = mpc.getHardware().lock();
	auto controls = mpc.getControls().lock();

	if (sequencer.lock()->isPlaying())
		return;

	if (controls->isRecPressed())
	{
		if (find(begin(allowTransportScreens), end(allowTransportScreens), currentScreenName) == end(allowTransportScreens))
			ls.lock()->openScreen("sequencer");

		sequencer.lock()->recFromStart();

		if (!sequencer.lock()->isRecording())
			return;
	}
	else if (controls->isOverDubPressed())
	{
		if (find(begin(allowTransportScreens), end(allowTransportScreens), currentScreenName) == end(allowTransportScreens))
			ls.lock()->openScreen("sequencer");

		sequencer.lock()->overdubFromStart();

		if (!sequencer.lock()->isOverDubbing())
			return;
	}
	else
	{
		if (controls->isShiftPressed())
		{
			ls.lock()->openScreen("vmpc-direct-to-disk-recorder");
		}
		else
		{
			if (!allowTransport() && !allowPlay())
			{
				ls.lock()->openScreen("sequencer");
			}
			
			sequencer.lock()->setSongModeEnabled(currentScreenName.compare("song") == 0);
			sequencer.lock()->playFromStart();
		}
	}
	
	hw->getLed("play").lock()->light(sequencer.lock()->isPlaying());
	hw->getLed("rec").lock()->light(sequencer.lock()->isRecording());
	hw->getLed("overdub").lock()->light(sequencer.lock()->isOverDubbing());
}

void BaseControls::mainScreen()
{
	init();

	auto ams = mpc.getAudioMidiServices().lock();

	if (ams->isRecordingSound())
		ams->stopSoundRecorder();

	ls.lock()->openScreen("sequencer");
	sequencer.lock()->setSoloEnabled(sequencer.lock()->isSoloEnabled());

	auto hw = mpc.getHardware().lock();
	hw->getLed("next-seq").lock()->light(false);
	hw->getLed("track-mute").lock()->light(false);
}

void BaseControls::tap()
{
	init();
	auto controls = mpc.getControls().lock();
	controls->setTapPressed(true);
	sequencer.lock()->tap();
}

void BaseControls::prevStepEvent()
{
}

void BaseControls::nextStepEvent()
{
}

void BaseControls::goTo()
{
	init();
	auto controls = mpc.getControls().lock();
	controls->setGoToPressed(true);
}

void BaseControls::prevBarStart()
{
}

void BaseControls::nextBarEnd()
{
}

void BaseControls::nextSeq()
{
	init();

	if (currentScreenName.compare("next-seq") == 0 ||
		currentScreenName.compare("next-seq-pad") == 0)
	{
		ls.lock()->openScreen("sequencer");
		mpc.getHardware().lock()->getLed("next-seq").lock()->light(false);
	}
	else if (currentScreenName.compare("sequencer") == 0 ||
		currentScreenName.compare("track-mute") == 0)
	{
		Util::initSequence(mpc);
		ls.lock()->openScreen("next-seq");
		mpc.getHardware().lock()->getLed("next-seq").lock()->light(true);
		mpc.getHardware().lock()->getLed("track-mute").lock()->light(false);
	}
}

void BaseControls::trackMute()
{
	init();

	if (currentScreenName.compare("track-mute") == 0)
	{
		auto previous = ls.lock()->getPreviousScreenName();
		if (previous.compare("next-seq") == 0 || previous.compare("next-seq-pad") == 0)
			ls.lock()->openScreen("next-seq");
		else
			ls.lock()->openScreen("sequencer");

		mpc.getHardware().lock()->getLed("track-mute").lock()->light(false);
	}
	else if
		(currentScreenName.compare("next-seq") == 0 ||
		currentScreenName.compare("next-seq-pad") == 0 ||
		currentScreenName.compare("sequencer") == 0)
	{
		Util::initSequence(mpc);
		ls.lock()->openScreen("track-mute");
		mpc.getHardware().lock()->getLed("track-mute").lock()->light(true);
	}
}

void BaseControls::bank(int i)
{
	init();
	
	auto oldBank = mpc.getBank();
		
	auto diff = 16 * (i - oldBank);
	auto newPadIndex = mpc.getPad() + diff;
	
	mpc.setBank(i);

	auto newNote = program.lock()->getPad(newPadIndex)->getNote();
	
	mpc.setPadAndNote(newPadIndex, newNote);
	
	for (int i = 0; i < 16; i++)
		mpc.getHardware().lock()->getPad(i).lock()->notifyObservers(255);
}

void BaseControls::fullLevel()
{
    init();
	auto hardware = mpc.getHardware().lock();
	auto topPanel = hardware->getTopPanel().lock();
    
	topPanel->setFullLevelEnabled(!topPanel->isFullLevelEnabled());

	hardware->getLed("full-level").lock()->light(topPanel->isFullLevelEnabled());
}

void BaseControls::sixteenLevels()
{
	init();

	if (currentScreenName.compare("sequencer") != 0 &&
		currentScreenName.compare("assign-16-levels") != 0)
	{
		return;
	}

	auto hardware = mpc.getHardware().lock();
	auto topPanel = hardware->getTopPanel().lock();

	if (topPanel->isSixteenLevelsEnabled()) {
		topPanel->setSixteenLevelsEnabled(false);
		hardware->getLed("sixteen-levels").lock()->light(false);
	}
	else {
		ls.lock()->openScreen("assign-16-levels");
	}
}

void BaseControls::after()
{
	init();
	auto hw = mpc.getHardware().lock();
	auto topPanel = hw->getTopPanel().lock();
	auto controls = mpc.getControls().lock();

	if (controls->isShiftPressed())
	{
		ls.lock()->openScreen("assign");
	}
	else {
		topPanel->setAfterEnabled(!topPanel->isAfterEnabled());
		hw->getLed("after").lock()->light(topPanel->isAfterEnabled());
	}
}

void BaseControls::shift()
{
	auto controls = mpc.getControls().lock();
	
	if (controls->isShiftPressed())
		return;
	
	controls->setShiftPressed(true);

	init();
	auto focus = ls.lock()->getFocusedLayer().lock()->findField(param).lock();

	if (focus && focus->isTypeModeEnabled())
	{
		focus->disableTypeMode();
		auto split = focus->getActiveSplit();

		if (split != -1)
		{
			focus->setSplit(true);
			focus->setActiveSplit(split);
		}
	}
}

void BaseControls::undoSeq()
{
	sequencer.lock()->undoSeq();
}

void BaseControls::setSliderNoteVar(NoteEvent* n, weak_ptr<mpc::sampler::Program> program)
{
	auto lProgram = program.lock();

	if (n->getNote() != lProgram->getSlider()->getNote())
	{
		return;
	}

	auto sliderParam = lProgram->getSlider()->getParameter();
	int rangeLow = 0, rangeHigh = 0, sliderRange = 0;
	n->setVariationTypeNumber(sliderParam);
	int sliderValue = mpc.getHardware().lock()->getSlider().lock()->getValue();
	double sliderRangeRatio = 0;
	int tuneValue;
	int decayValue;
	int attackValue;
	int filterValue;
	switch (sliderParam)
	{
	case 0:
		rangeLow = lProgram->getSlider()->getTuneLowRange();
		rangeHigh = lProgram->getSlider()->getTuneHighRange();
		sliderRange = rangeHigh - rangeLow;
		sliderRangeRatio = sliderRange / 128.0;
		tuneValue = (int)(sliderValue * sliderRangeRatio * 0.5);
		tuneValue += (120 - rangeHigh) / 2;
		n->setVariationValue(tuneValue);
		break;
	case 1:
		rangeLow = lProgram->getSlider()->getDecayLowRange();
		rangeHigh = lProgram->getSlider()->getDecayHighRange();
		sliderRange = rangeHigh - rangeLow;
		sliderRangeRatio = sliderRange / 128.0;
		decayValue = (int)(sliderValue * sliderRangeRatio);
		n->setVariationValue(decayValue);
		break;
	case 2:
		rangeLow = lProgram->getSlider()->getAttackLowRange();
		rangeHigh = lProgram->getSlider()->getAttackHighRange();
		sliderRange = rangeHigh - rangeLow;
		sliderRangeRatio = sliderRange / 128.0;
		attackValue = (int)(sliderValue * sliderRangeRatio);
		n->setVariationValue(attackValue);
		break;
	case 3:
		rangeLow = lProgram->getSlider()->getFilterLowRange();
		rangeHigh = lProgram->getSlider()->getFilterHighRange();
		sliderRange = rangeHigh - rangeLow;
		sliderRangeRatio = sliderRange / 128.0;
		filterValue = (int)(sliderValue * sliderRangeRatio);
		n->setVariationValue(filterValue);
		break;
	}
}

void BaseControls::erase()
{
	init();
	auto controls = mpc.getControls().lock();
	controls->setErasePressed(true);

	if (!sequencer.lock()->getActiveSequence().lock()->isUsed())
		return;

	if (!sequencer.lock()->isRecordingOrOverdubbing()) {
		ls.lock()->openScreen("erase");
	}
}

int BaseControls::getSoundIncrement(int notch_inc)
{
	auto soundInc = notch_inc;

	if (abs(notch_inc) != 1)
		soundInc *= (int)(ceil(sampler.lock()->getSound().lock()->getFrameCount() / 15000.0));

	return soundInc;
}

void BaseControls::splitLeft()
{
	init();

	auto field = ls.lock()->getFocusedLayer().lock()->findField(param).lock();
	auto controls = mpc.getControls().lock();

	if (!controls->isShiftPressed())
	{
		BaseControls::left();
		return;
	}

	if (!splittable)
		return;

	if (field->isSplit())
	{
		field->setActiveSplit(field->getActiveSplit() - 1);
	}
	else
	{
		field->setSplit(true);
	}
}

void BaseControls::splitRight()
{
	init();
	auto field = ls.lock()->getFocusedLayer().lock()->findField(param).lock();
	auto controls = mpc.getControls().lock();
	
	if (!controls->isShiftPressed())
	{
		BaseControls::right();
		return;
	}
	
	if (splittable && field->isSplit())
	{
		if (!field->setActiveSplit(field->getActiveSplit() + 1))
		{
			field->setSplit(false);
		}
	}
}

const std::vector<std::string> BaseControls::allowPlayScreens {
	"song",
	"track-mute",
	"next-seq",
	"next-seq-pad"
};

bool BaseControls::allowPlay()
{
	return find(
		begin(BaseControls::allowPlayScreens),
		end(BaseControls::allowPlayScreens),
		ls.lock()->getCurrentScreenName()
	) != end(BaseControls::allowPlayScreens);
}

const std::vector<std::string> BaseControls::allowTransportScreens {
	"sequencer",
	"select-drum",
	"select-mixer-drum",
	"program-assign",
	"program-params",
	"drum", "purge",
	"program",
	"create-new-program",
	"name",
	"delete-program",
	"delete-all-programs",
	"assignment-view",
	"initialize-pad-assign",
	"copy-note-parameters",
	"velocity-modulation",
	"velo-env-filter",
	"velo-pitch",
	"mute-assign",
	"trans"
};

bool BaseControls::allowTransport()
{
	return find(
		begin(BaseControls::allowTransportScreens),
		end(BaseControls::allowTransportScreens),
		ls.lock()->getCurrentScreenName()
	) != end(BaseControls::allowTransportScreens);
}
