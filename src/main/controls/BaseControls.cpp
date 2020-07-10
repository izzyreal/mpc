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
#include <lcdgui/screens/LoadScreen.hpp>
#include <lcdgui/Screens.hpp>

#include <mpc/MpcSoundPlayerChannel.hpp>
#include <audio/server/NonRealTimeAudioServer.hpp>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::controls;
using namespace std;

BaseControls::BaseControls()
	: mpc(mpc::Mpc::instance())
{
	ls = mpc.getLayeredScreen();
	sampler = mpc.getSampler();
	sequencer = mpc.getSequencer();
}

vector<int> BaseControls::splitInc = vector<int>{ 10000000, 1000000, 100000, 10000, 1000, 100, 10, 1 };

void BaseControls::init()
{
	sampler = mpc.getSampler();
	sequencer = mpc.getSequencer();

	currentScreenName = ls.lock()->getCurrentScreenName();
	param = ls.lock()->getFocus();
	activeField = ls.lock()->getFocusedLayer().lock()->findField(param);
	
	
	if (sequencer.lock()->getActiveTrack().lock())
	{
		track = sequencer.lock()->getActiveTrack();

		if (track.lock()->getBusNumber() != 0)
		{
			mpcSoundPlayerChannel = sampler.lock()->getDrum(track.lock()->getBusNumber() - 1);
			program = dynamic_pointer_cast<mpc::sampler::Program>(sampler.lock()->getProgram(mpcSoundPlayerChannel->getProgram()).lock());
		}
	}
}

void BaseControls::left()
{
	init();
	
	if (!activeField.lock())
	{
		return;
	}
	
	if (param.compare("dummy") == 0)
	{
		return;
	}
	
	ls.lock()->transferLeft();
}

void BaseControls::right()
{
	init();
	
	if (!activeField.lock())
	{
		return;
	}
	
	if (param.compare("dummy") == 0)
	{
		return;
	}
	
	ls.lock()->transferRight();
}

void BaseControls::up()
{
    init();

	if (!activeField.lock())
	{
		return;
	}

	if (param.compare("dummy") == 0)
	{
		return;
	}

	ls.lock()->transferUp();
}

void BaseControls::down()
{
	init();

	if (!activeField.lock())
	{
		return;
	}

	if (param.compare("dummy") == 0)
	{
		return;
	}

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
				auto editSoundScreen = dynamic_pointer_cast<EditSoundScreen>(Screens::getScreenComponent("edit-sound"));
				ls.lock()->setPreviousScreenName(editSoundScreen->getPreviousScreenName());
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
				auto nameScreen = dynamic_pointer_cast<NameScreen>(Screens::getScreenComponent("name"));
				nameScreen->editing = false;
				ls.lock()->setLastFocus("name", "0");
			}
			else if (currentScreenName.compare("directory") == 0)
			{
				auto directoryScreen = dynamic_pointer_cast<DirectoryScreen>(Screens::getScreenComponent("directory"));
				ls.lock()->setPreviousScreenName(directoryScreen->previousScreenName);
			}
			
			if (ls.lock()->getPreviousScreenName().compare("load") == 0)
			{
				auto loadScreen = dynamic_pointer_cast<LoadScreen>(Screens::getScreenComponent("load"));

				if (loadScreen->fileLoad + 1 > mpc.getDisk().lock()->getFiles().size())
				{
					loadScreen->fileLoad = 0; // Can we avoid this? Who's leaving fileLoad in a bad state?
				}
			}
		}
		
		if (ls.lock()->getFocusedLayerIndex() == 1 || ls.lock()->getFocusedLayerIndex() == 2 || ls.lock()->getFocusedLayerIndex() == 3)
		{
			ls.lock()->openScreen(ls.lock()->getPreviousScreenName());
		}
		break;
	}
}

void BaseControls::openWindow()
{
}

void BaseControls::turnWheel(int i)
{
}

void BaseControls::pad(int i, int velo, bool repeat, int tick)
{
	init();

	auto controls = mpc.getControls().lock();

	if (mpc.getHardware().lock()->getTopPanel().lock()->isFullLevelEnabled())
	{
		velo = 127;
	}

	if (controls->getPressedPads()->find(i) == controls->getPressedPads()->end())
	{
		controls->getPressedPads()->emplace(i);
		controls->getPressedPadVelos()->at(i) = velo;
	}
	else {
		if (!(controls->isTapPressed() && sequencer.lock()->isPlaying()))
		{
			return;
		}
	}
	auto note = track.lock()->getBusNumber() > 0 ? program.lock()->getPad(i + (mpc.getBank() * 16))->getNote() : i + (mpc.getBank() * 16) + 35;
	auto velocity = velo;
	auto pad = i + (mpc.getBank() * 16);

	auto assign16LevelsScreen = dynamic_pointer_cast<Assign16LevelsScreen>(Screens::getScreenComponent("assign-16-levels"));

	if (mpc.getHardware().lock()->getTopPanel().lock()->isSixteenLevelsEnabled())
	{
		if (assign16LevelsScreen->getParameter() == 0)
		{
			note = assign16LevelsScreen->getNote();
			velocity = (int)(i * (127.0 / 16.0));
			
			if (velocity == 0)
			{
				velocity = 1;
			}
		}
	}
	else
	{
		if (currentScreenName.compare("program-params") == 0)
		{
			if (note > 34)
			{
				mpc.setPadAndNote(pad, note);
			}
		}
		else
		{
			mpc.setPadAndNote(pad, note);
		}
	}

	if (currentScreenName.compare("assign-16-levels") == 0 && note != 34)
	{
		assign16LevelsScreen->setNote(note);
	}

	if (controls->isTapPressed() && sequencer.lock()->isPlaying())
	{
		if (repeat)
		{
			generateNoteOn(note, velocity, tick);
		}
	}
	else {
		generateNoteOn(note, velocity, -1);
	}
}

void BaseControls::generateNoteOn(int nn, int padVelo, int tick)
{
	init();

	auto lProgram = program.lock();
	bool slider = lProgram && nn == lProgram->getSlider()->getNote();
	bool posIsLastTick = sequencer.lock()->getTickPosition() == sequencer.lock()->getActiveSequence().lock()->getLastTick();
	bool step = currentScreenName.compare("step-editor") == 0 && !posIsLastTick;
	auto timingCorrectScreen = dynamic_pointer_cast<TimingCorrectScreen>(Screens::getScreenComponent("timing-correct"));
	auto noteValue = timingCorrectScreen->getNoteValue();
	auto swing = timingCorrectScreen->getSwing();

	bool recMainWithoutPlaying = currentScreenName.compare("sequencer") == 0 && !sequencer.lock()->isPlaying() && mpc.getControls().lock()->isRecPressed() && timingCorrectScreen->getNoteValue() != 0 && !posIsLastTick;

	shared_ptr<mpc::sequencer::NoteEvent> n;

	auto assign16LevelsScreen = dynamic_pointer_cast<Assign16LevelsScreen>(Screens::getScreenComponent("assign-16-levels"));

	if (sequencer.lock()->isRecordingOrOverdubbing() || step || recMainWithoutPlaying)
	{
		if (step)
		{
			n = track.lock()->addNoteEvent(sequencer.lock()->getTickPosition(), nn).lock();
		}
		else if (recMainWithoutPlaying)
		{
			n = track.lock()->addNoteEvent(sequencer.lock()->getTickPosition(), nn).lock();
			int noteVal = timingCorrectScreen->getNoteValue();
			int stepLength = sequencer.lock()->getTickValues()[noteVal];
			if (stepLength != 0) {
				int bar = sequencer.lock()->getCurrentBarIndex() + 1;
				track.lock()->timingCorrect(0, bar, n.get(), stepLength);
				vector<weak_ptr<mpc::sequencer::Event>> events{ n };
				track.lock()->swing(events, noteVal, timingCorrectScreen->getSwing(), vector<int>{0, 127});
				if (n->getTick() != sequencer.lock()->getTickPosition()) {
					sequencer.lock()->move(n->getTick());
				}
			}
		}
		else
		{
			n = track.lock()->recordNoteOn().lock();
			n->setNote(nn);
		}
		
		n->setVelocity(padVelo);
		n->setDuration(step ? 1 : -1);
		
		if (mpc.getHardware().lock()->getTopPanel().lock()->isSixteenLevelsEnabled() && assign16LevelsScreen->getParameter() == 1)
		{
			auto type = assign16LevelsScreen->getType();
			auto key = assign16LevelsScreen->getOriginalKeyPad();
			auto diff = lProgram->getPadIndexFromNote(nn) - (mpc.getBank() * 16) - key;
			n->setNote(assign16LevelsScreen->getNote());
			n->setVariationTypeNumber(type);
			n->setVariationValue(diff * 5);
		}
		
		if (slider)
		{
			setSliderNoteVar(n.get(), program);
		}

		if (step || recMainWithoutPlaying)
		{
			sequencer.lock()->playMetronomeTrack();
		}
	}

	auto noteEvent = make_shared<mpc::sequencer::NoteEvent>(nn);
	noteEvent->setVelocity(padVelo);
	noteEvent->setDuration(0);
	noteEvent->setVariationValue(64);
	noteEvent->setTick(tick);

	if (mpc.getHardware().lock()->getTopPanel().lock()->isSixteenLevelsEnabled() && assign16LevelsScreen->getParameter() == 1) {

		auto type = assign16LevelsScreen->getType();
		auto key = assign16LevelsScreen->getOriginalKeyPad();
		auto padnr = program.lock()->getPadIndexFromNote(nn) - (mpc.getBank() * 16);

		if (type == 0) {
			auto diff = padnr - key;
			auto candidate = 64 + (diff * 5);
			if (candidate > 124) {
				candidate = 124;
			}
			else if (candidate < 4) {
				candidate = 4;
			}
			noteEvent->setVariationValue(candidate);
		}
		else {
			noteEvent->setVariationValue((100 / 16) * padnr);
		}
		noteEvent->setNote(assign16LevelsScreen->getNote());
		noteEvent->setVariationTypeNumber(type);
	}

	if (slider)
	{
		setSliderNoteVar(noteEvent.get(), program);
	}

	mpc.getEventHandler().lock()->handle(noteEvent, track.lock().get());
}

void BaseControls::numpad(int i)
{
	init();

	auto controls = mpc.getControls().lock();
	
	if (!controls->isShiftPressed())
	{
		auto mtf = ls.lock()->getFocusedLayer().lock()->findField(param).lock();

		if (isTypable())
		{
			if (!mtf->isTypeModeEnabled())
			{
				mtf->enableTypeMode();
			}

			mtf->type(i);
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
			{
				return;
			}
			ls.lock()->openScreen("song");
			break;
		case 2:
			if (sequencer.lock()->isPlaying())
			{
				break;
			}
			ls.lock()->openScreen("punch");
			break;
		case 3:
		{
			if (sequencer.lock()->isPlaying())
			{
				break;
			}

			disk->initFiles();

			auto loadScreen = dynamic_pointer_cast<LoadScreen>(Screens::getScreenComponent("load"));

			if (loadScreen->fileLoad + 1 > (int)(disk->getFiles().size()))
			{
				loadScreen->fileLoad = (int)(disk->getFiles().size() - 1); // Same here, can we avoid this?
			}

			ls.lock()->openScreen("load");
			break;
		}
		case 4:
			if (sequencer.lock()->isPlaying())
			{
				break;
			}
			ls.lock()->openScreen("sample");
			break;
		case 5:
			if (sequencer.lock()->isPlaying())
			{
				break;
			}
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
			{
				break;
			}
			ls.lock()->openScreen("others");
			break;
		case 9:
			if (sequencer.lock()->isPlaying())
			{
				break;
			}
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
	{
		ls.lock()->openScreen("save");
	}
}

void BaseControls::rec()
{
	auto controls = mpc.getControls().lock();
	controls->setRecPressed(true);
    init();

	auto hw = mpc.getHardware().lock();
	if(!sequencer.lock()->isPlaying())
	{
		hw->getLed("rec").lock()->light(true);
    }
	else
	{
        if(sequencer.lock()->isRecordingOrOverdubbing())
		{
            sequencer.lock()->setRecording(false);
            sequencer.lock()->setOverdubbing(false);
			hw->getLed("rec").lock()->light(false);
			hw->getLed("overdub").lock()->light(false);
        }
    }
}

void BaseControls::overDub()
{
	auto controls = mpc.getControls().lock();
	controls->setOverDubPressed(true);
	init();

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
}

void BaseControls::stop()
{
	init();

	sequencer.lock()->stop();
	auto controls = mpc.getControls().lock();
	if (controls->isShiftPressed())
	{
		mpc.getAudioMidiServices().lock()->stopBouncing();
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
			if (currentScreenName.compare("sequencer") != 0)
			{
				ls.lock()->openScreen("sequencer");
			}
			sequencer.lock()->rec();
		}
		else if (controls->isOverDubPressed())
		{
			if (currentScreenName.compare("sequencer") != 0)
			{
				ls.lock()->openScreen("sequencer");
			}
			sequencer.lock()->overdub();
		}
		else {
			if (controls->isShiftPressed() && !mpc.getAudioMidiServices().lock()->isBouncing())
			{
				ls.lock()->openScreen("vmpc-direct-to-disk-recorder");
			}
			else
			{
				if (currentScreenName.compare("song") != 0 && currentScreenName.compare("sequencer") != 0 && currentScreenName.compare("track-mute") != 0)
				{
					ls.lock()->openScreen("sequencer");
				}
				if (currentScreenName.compare("song") == 0)
				{
					sequencer.lock()->setSongModeEnabled(true);
				}
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
	{
		return;
	}

	if (controls->isRecPressed())
	{
		if (currentScreenName.compare("sequencer") != 0)
		{
			ls.lock()->openScreen("sequencer");
		}
		sequencer.lock()->recFromStart();
	}
	else if (controls->isOverDubPressed())
	{
		if (currentScreenName.compare("sequencer") != 0)
		{
			ls.lock()->openScreen("sequencer");
		}
		sequencer.lock()->overdubFromStart();
	}
	else
	{
		if (controls->isShiftPressed())
		{
			ls.lock()->openScreen("vmpc-direct-to-disk-recorder");
		}
		else
		{
			if (currentScreenName.compare("song") != 0 && currentScreenName.compare("sequencer") != 0 && currentScreenName.compare("track-mute") != 0)
			{
				ls.lock()->openScreen("sequencer");
			}
			
			if (currentScreenName.compare("song") == 0)
			{
				sequencer.lock()->setSongModeEnabled(true);
			}
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
	if (ams->isRecordingSound()) {
		ams->stopSoundRecorder();
	}
	ls.lock()->openScreen("sequencer");
    sequencer.lock()->setSoloEnabled(sequencer.lock()->isSoloEnabled());
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
	ls.lock()->openScreen("next-seq");
}

void BaseControls::trackMute()
{
	init();
	if (currentScreenName.compare("track-mute") == 0) {
		ls.lock()->openScreen("sequencer");
		return;
	}
	sequencer.lock()->setSoloEnabled(false);
	ls.lock()->openScreen("track-mute");
	mpc.getHardware().lock()->getLed("track-mute").lock()->light(true);
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
	{
		mpc.getHardware().lock()->getPad(i).lock()->notifyObservers(255);
	}
}

void BaseControls::fullLevel()
{
    init();
	auto hardware = mpc.getHardware().lock();
	auto topPanel = hardware->getTopPanel().lock();
    
	topPanel->setFullLevelEnabled(!topPanel->isFullLevelEnabled());

	hardware->getLed("fulllevel").lock()->light(topPanel->isFullLevelEnabled());
}

void BaseControls::sixteenLevels()
{
	init();
	auto hardware = mpc.getHardware().lock();
	auto topPanel = hardware->getTopPanel().lock();
	if (topPanel->isSixteenLevelsEnabled()) {
		topPanel->setSixteenLevelsEnabled(false);
		hardware->getLed("sixteenlevels").lock()->light(false);
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
	{
		return;
	}

	controls->setShiftPressed(true);
}

void BaseControls::undoSeq()
{
	sequencer.lock()->undoSeq();
}

void BaseControls::setSliderNoteVar(mpc::sequencer::NoteEvent* n, weak_ptr<mpc::sampler::Program> program)
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

bool BaseControls::isTypable()
{
	if (typableParams.size() == 0)
		return false;

	for (auto str : typableParams)
		if (str.compare(param) == 0)
			return true;
	return false;
}

void BaseControls::erase()
{
	init();
	auto controls = mpc.getControls().lock();
	controls->setErasePressed(true);

	if (!sequencer.lock()->getActiveSequence().lock()->isUsed())
		return;

	if (sequencer.lock()->isOverDubbing()) {
		// TODO
	}
	else {
		ls.lock()->openScreen("erase");
	}
}

int BaseControls::getSoundIncrement(int notch_inc)
{
	auto soundInc = notch_inc;
	if (abs(notch_inc) != 1)
	{
		soundInc *= (int)(ceil(sampler.lock()->getSound().lock()->getLastFrameIndex() / 15000.0));
	}
	return soundInc;
}

void BaseControls::splitLeft()
{
	init();

	auto mtf = ls.lock()->getFocusedLayer().lock()->findField(param).lock();
	auto controls = Mpc::instance().getControls().lock();

	if (!controls->isShiftPressed())
	{
		BaseControls::left();
		return;
	}

	if (!splittable)
	{
		return;
	}

	if (!mtf->isSplit())
	{
		mtf->setSplit(true);
	}
	else
	{
		mtf->setActiveSplit(mtf->getActiveSplit() - 1);
	}
}

void BaseControls::splitRight()
{
	init();
	auto mtf = ls.lock()->getFocusedLayer().lock()->findField(param).lock();
	auto controls = Mpc::instance().getControls().lock();
	
	if (controls->isShiftPressed())
	{
		if (splittable && mtf->isSplit())
		{
			if (!mtf->setActiveSplit(mtf->getActiveSplit() + 1))
			{
				mtf->setSplit(false);
			}
		}
	}
	else {
		BaseControls::right();
	}
}
