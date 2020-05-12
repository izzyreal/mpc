#include "BaseControls.hpp"

#include <Mpc.hpp>

#include <controls/Controls.hpp>

#include <ui/Uis.hpp>

#include <hardware/Hardware.hpp>
#include <hardware/Led.hpp>
#include <hardware/HwSlider.hpp>
#include <hardware/HwPad.hpp>
#include <hardware/TopPanel.hpp>

#include <sequencer/Sequencer.hpp>
#include <sampler/Sampler.hpp>
#include <sampler/Program.hpp>

#include <Paths.hpp>

#include <audiomidi/AudioMidiServices.hpp>
#include <audiomidi/EventHandler.hpp>
#include <disk/AbstractDisk.hpp>

#include <ui/NameGui.hpp>
#include <lcdgui/Field.hpp>
#include <ui/disk/DiskGui.hpp>
#include <ui/disk/window/DirectoryGui.hpp>
#include <ui/misc/PunchGui.hpp>
#include <ui/sampler/SamplerGui.hpp>
#include <ui/sampler/SoundGui.hpp>
#include <ui/sampler/window/EditSoundGui.hpp>
#include <ui/sequencer/SequencerGui.hpp>
#include <ui/sequencer/window/SequencerWindowGui.hpp>
#include <ui/vmpc/DirectToDiskRecorderGui.hpp>
#include <sampler/Pad.hpp>
#include <sampler/Program.hpp>
#include <sampler/Sampler.hpp>
#include <sampler/PgmSlider.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/NoteEvent.hpp>
#include <sequencer/Sequencer.hpp>

#include <mpc/MpcSoundPlayerChannel.hpp>
#include <audio/server/NonRealTimeAudioServer.hpp>

using namespace mpc;
using namespace mpc::controls;
using namespace std;

BaseControls::BaseControls()
{
	
	sequencer = Mpc::instance().getSequencer();
	sampler = Mpc::instance().getSampler();
	ls = Mpc::instance().getLayeredScreen();
	sequencerGui = Mpc::instance().getUis().lock()->getSequencerGui();
	nameGui = Mpc::instance().getUis().lock()->getNameGui();
	samplerGui = Mpc::instance().getUis().lock()->getSamplerGui();
}

void BaseControls::init()
{
	csn = ls.lock()->getCurrentScreenName();
	param = ls.lock()->getFocus();
	activeField = ls.lock()->lookupField(param);
	auto lSequencer = sequencer.lock();
    track = lSequencer->getActiveSequence().lock()->getTrack(lSequencer->getActiveTrackIndex());
	auto lTrk = track.lock();
	if (lTrk->getBusNumber() != 0 && Mpc::instance().getAudioMidiServices().lock()->getAudioServer()->isRunning()) {
		auto lSampler = sampler.lock();
        mpcSoundPlayerChannel = lSampler->getDrum(lTrk->getBusNumber() - 1);
		program = dynamic_pointer_cast<mpc::sampler::Program>(lSampler->getProgram(mpcSoundPlayerChannel->getProgram()).lock());
    }
    bank_ = samplerGui->getBank();
}

void BaseControls::left()
{
	init();
	
	if (!activeField.lock()) {
		return;
	}
	
	if (param.compare("dummy") == 0)
	{
		return;
	}
	
	ls.lock()->transferFocus(true);
}

void BaseControls::right()
{
	init();
	
	if (!activeField.lock()) {
		return;
	}
	
	if (param.compare("dummy") == 0) {
		return;
	}
	
	ls.lock()->transferFocus(false);
}

void BaseControls::up()
{
    init();
	auto aboveCandidate = ls.lock()->findAbove(param);
	if (param.compare(aboveCandidate) != 0) {
		ls.lock()->setFocus(aboveCandidate);
	}
}

void BaseControls::down()
{
    init();
	auto belowCandidate = ls.lock()->findBelow(param);
	if (param.compare(belowCandidate) != 0) {
		ls.lock()->setFocus(belowCandidate);
	}
}

void BaseControls::function(int i)
{
	init();
	auto lsLocked = ls.lock();
	switch (i) {
	case 3:
		if (lsLocked->getFocusedLayerIndex() == 1) {
			if (csn.compare("sequence") == 0) {
				lsLocked->setPreviousScreenName("sequencer");
			}
			else if (csn.compare("midiinput") == 0) {
				lsLocked->setPreviousScreenName("sequencer");
			}
			else if (csn.compare("midioutput") == 0) {
				lsLocked->setPreviousScreenName("sequencer");
			}
			else if (csn.compare("editsound") == 0) {
				lsLocked->setPreviousScreenName(Mpc::instance().getUis().lock()->getEditSoundGui()->getPreviousScreenName());
			}
			else if (csn.compare("sound") == 0) {
				lsLocked->setPreviousScreenName(Mpc::instance().getUis().lock()->getSoundGui()->getPreviousScreenName());
			}
			else if (csn.compare("program") == 0) {
				lsLocked->setPreviousScreenName(Mpc::instance().getUis().lock()->getSamplerGui()->getPrevScreenName());
			}
			else if (csn.compare("name") == 0) {
				Mpc::instance().getUis().lock()->getNameGui()->setNameBeingEdited(false);
				lsLocked->setLastFocus("name", "0");
			}
			else if (csn.compare("numberofzones") == 0) {
				Mpc::instance().getUis().lock()->getSoundGui()->setNumberOfZones(Mpc::instance().getUis().lock()->getSoundGui()->getPreviousNumberOfzones());
			}
			else if (csn.compare("directory") == 0) {
				lsLocked->setPreviousScreenName(Mpc::instance().getUis().lock()->getDirectoryGui()->getPreviousScreenName());
			}
			if (lsLocked->getPreviousScreenName().compare("load") == 0) {
				if (Mpc::instance().getUis().lock()->getDiskGui()->getFileLoad() + 1 > Mpc::instance().getDisk().lock()->getFiles().size()) {
					Mpc::instance().getUis().lock()->getDiskGui()->setFileLoad(0);
				}
			}
		}
		if (lsLocked->getFocusedLayerIndex() == 1 || lsLocked->getFocusedLayerIndex() == 2 || lsLocked->getFocusedLayerIndex() == 3) {
			lsLocked->openScreen(lsLocked->getPreviousScreenName());
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
	auto lTrk = track.lock();
	auto& mpc = Mpc::instance();
	auto controls = mpc.getControls().lock();
	auto lSequencer = sequencer.lock();

	if (mpc.getHardware().lock()->getTopPanel().lock()->isFullLevelEnabled())
	{
		velo = 127;
	}

	if (controls->getPressedPads()->find(i) == controls->getPressedPads()->end()) {
		controls->getPressedPads()->emplace(i);
		controls->getPressedPadVelos()->at(i) = velo;
	}
	else {
		if (!(controls->isTapPressed() && lSequencer->isPlaying())) {
			return;
		}
	}
	auto note = lTrk->getBusNumber() > 0 ? program.lock()->getPad(i + (bank_ * 16))->getNote() : i + (bank_ * 16) + 35;
	auto velocity = velo;
	auto pad = i + (bank_ * 16);

	if (mpc.getHardware().lock()->getTopPanel().lock()->isSixteenLevelsEnabled())
	{
		if (mpc.getUis().lock()->getSequencerGui()->getParameter() == 0)
		{
			note = mpc.getUis().lock()->getSequencerGui()->getNote();
			velocity = (int)(i * (127.0 / 16.0));
			
			if (velocity == 0)
			{
				velocity = 1;
			}
		}
	}
	else
	{
		if (csn.compare("programparams") == 0)
		{
			if (note > 34)
			{
				samplerGui->setPadAndNote(pad, note);
			}
		}
		else {
			samplerGui->setPadAndNote(pad, note);
		}
	}

	if (csn.compare("assign16levels") == 0 && note != 34)
	{
		mpc.getUis().lock()->getSequencerGui()->setNote(note);
	}

	if (controls->isTapPressed() && lSequencer->isPlaying())
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
	auto& mpc = mpc::Mpc::instance();
	auto lTrk = track.lock();
	auto lProgram = program.lock();
	bool slider = lProgram && nn == lProgram->getSlider()->getNote();
	auto lSequencer = sequencer.lock();
	bool posIsLastTick = lSequencer->getTickPosition() == lSequencer->getActiveSequence().lock()->getLastTick();
	bool step = csn.compare("sequencer_step") == 0 && !posIsLastTick;
	auto swGui = Mpc::instance().getUis().lock()->getSequencerWindowGui();
	bool recMainWithoutPlaying = csn.compare("sequencer") == 0 && !lSequencer->isPlaying() && Mpc::instance().getControls().lock()->isRecPressed() && swGui->getNoteValue() != 0 && !posIsLastTick;

	shared_ptr<mpc::sequencer::NoteEvent> n;
	
	if (lSequencer->isRecordingOrOverdubbing() || step || recMainWithoutPlaying)
	{
		if (step)
		{
			n = lTrk->addNoteEvent(lSequencer->getTickPosition(), nn).lock();
		}
		else if (recMainWithoutPlaying)
		{
			n = lTrk->addNoteEvent(lSequencer->getTickPosition(), nn).lock();
			int noteVal = swGui->getNoteValue();
			int stepLength = lSequencer->getTickValues()[noteVal];
			if (stepLength != 0) {
				int bar = lSequencer->getCurrentBarNumber() + 1;
				lTrk->timingCorrect(0, bar, n.get(), stepLength);
				vector<weak_ptr<mpc::sequencer::Event>> events{ n };
				lTrk->swing(events, noteVal, swGui->getSwing(), vector<int>{0, 127});
				if (n->getTick() != lSequencer->getTickPosition()) {
					lSequencer->move(n->getTick());
				}
			}
		}
		else {
			n = lTrk->recordNoteOn().lock();
			n->setNote(nn);
		}
		n->setVelocity(padVelo);
		n->setDuration(step ? 1 : -1);
		
		if (mpc.getHardware().lock()->getTopPanel().lock()->isSixteenLevelsEnabled() && sequencerGui->getParameter() == 1) {
			auto type = sequencerGui->getType();
			auto key = sequencerGui->getOriginalKeyPad();
			auto diff = lProgram->getPadNumberFromNote(nn) - (bank_ * 16) - key;
			n->setNote(sequencerGui->getNote());
			n->setVariationTypeNumber(type);
			n->setVariationValue(diff * 5);
		}
		if (slider) {
			setSliderNoteVar(n.get(), program);
		}

		if (step || recMainWithoutPlaying) {
			lSequencer->playMetronomeTrack();
		}
	}

	auto noteEvent = make_shared<mpc::sequencer::NoteEvent>(nn);
	noteEvent->setVelocity(padVelo);
	noteEvent->setDuration(0);
	noteEvent->setVariationValue(64);
	noteEvent->setTick(tick);

	if (mpc.getHardware().lock()->getTopPanel().lock()->isSixteenLevelsEnabled() && sequencerGui->getParameter() == 1) {

		auto type = sequencerGui->getType();
		auto key = sequencerGui->getOriginalKeyPad();
		auto padnr = program.lock()->getPadNumberFromNote(nn) - (bank_ * 16);

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
		noteEvent->setNote(sequencerGui->getNote());
		noteEvent->setVariationTypeNumber(type);
	}

	if (slider) {
		setSliderNoteVar(noteEvent.get(), program);
	}

	Mpc::instance().getEventHandler().lock()->handle(noteEvent, lTrk.get());
}

void BaseControls::numpad(int i)
{
	init();
	auto controls = Mpc::instance().getControls().lock();
	if (!controls->isShiftPressed()) {
		auto mtf = ls.lock()->lookupField(param);
		if (isTypable()) {
			auto lMtf = mtf.lock();
			if (!lMtf->isTypeModeEnabled())
				lMtf->enableTypeMode();

			lMtf->type(i);
		}
	}

	auto lSequencer = sequencer.lock();
	auto lDisk = Mpc::instance().getDisk().lock();

	if (controls->isShiftPressed()) {
		switch (i) {
		case 0:
			ls.lock()->openScreen("settings");
			break;
		case 1:
			if (lSequencer->isPlaying())
			{
				return;
			}
			ls.lock()->openScreen("song");
			break;
		case 2:
			if (lSequencer->isPlaying()) break;
			ls.lock()->openScreen("punch");
			Mpc::instance().getUis().lock()->getPunchGui()->setTime0(0);
			Mpc::instance().getUis().lock()->getPunchGui()->setTime1(lSequencer->getActiveSequence().lock()->getLastTick());
			break;
		case 3:
			if (lSequencer->isPlaying()) {
				break;
			}
			
			if (lDisk != nullptr) {
				lDisk->initFiles();
			}

			if (Mpc::instance().getUis().lock()->getDiskGui()->getFileLoad() + 1 > (int) (lDisk->getFiles().size())) {
				Mpc::instance().getUis().lock()->getDiskGui()->setFileLoad((int)(lDisk->getFiles().size() - 1));
			}
			ls.lock()->openScreen("load");
			break;
		case 4:
			if (lSequencer->isPlaying()) {
				break;
			}
			ls.lock()->openScreen("sample");
			break;
		case 5:
			if (lSequencer->isPlaying()) {
				break;
			}
			ls.lock()->openScreen("trim");
			break;
		case 6:
		case 130:
			ls.lock()->openScreen("selectdrum");
			break;
		case 7:
			ls.lock()->openScreen("selectdrum_mixer");
			break;
		case 8:
			if (lSequencer->isPlaying()) {
				break;
			}
			ls.lock()->openScreen("others");
			break;
		case 9:
			if (lSequencer->isPlaying()) {
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
	auto controls = Mpc::instance().getControls().lock();
	if (controls->isShiftPressed()) {
		ls.lock()->openScreen("save");
	}
}

void BaseControls::rec()
{
	auto controls = Mpc::instance().getControls().lock();
	controls->setRecPressed(true);
    init();
	auto lSequencer = sequencer.lock();
	auto hw = Mpc::instance().getHardware().lock();
	if(!lSequencer->isPlaying()) {
		hw->getLed("rec").lock()->light(true);
    } else {
        if(lSequencer->isRecordingOrOverdubbing()) {
            lSequencer->setRecording(false);
            lSequencer->setOverdubbing(false);
			hw->getLed("rec").lock()->light(false);
			hw->getLed("overdub").lock()->light(false);
        }
    }
}

void BaseControls::overDub()
{
	auto controls = Mpc::instance().getControls().lock();
	controls->setOverDubPressed(true);
	init();
	auto hw = Mpc::instance().getHardware().lock();
	auto lSequencer = sequencer.lock();
	if (!lSequencer->isPlaying()) {
		hw->getLed("overdub").lock()->light(true);
	}
	else {
		if (lSequencer->isRecordingOrOverdubbing()) {
			lSequencer->setRecording(false);
			lSequencer->setOverdubbing(false);
			hw->getLed("rec").lock()->light(false);
			hw->getLed("overdub").lock()->light(false);
		}
	}
}

void BaseControls::stop()
{
	init();
	auto lSequencer = sequencer.lock();
	lSequencer->stop();
	auto controls = Mpc::instance().getControls().lock();
	if (controls->isShiftPressed())
		Mpc::instance().getAudioMidiServices().lock()->stopBouncing();
}

void BaseControls::play()
{
	init();
	auto controls = Mpc::instance().getControls().lock();
	auto hw = Mpc::instance().getHardware().lock();
	auto lSequencer = sequencer.lock();

	if (lSequencer->isPlaying()) {
		if (controls->isRecPressed() && !lSequencer->isOverDubbing()) {
			lSequencer->setOverdubbing(false);
			lSequencer->setRecording(true);
			hw->getLed("overdub").lock()->light(false);
			hw->getLed("rec").lock()->light(true);
		}
		else if (controls->isOverDubPressed() && !lSequencer->isRecording()) {
			lSequencer->setOverdubbing(true);
			lSequencer->setRecording(false);
			hw->getLed("overdub").lock()->light(true);
			hw->getLed("rec").lock()->light(false);
		}
	}
	else {
		if (controls->isRecPressed()) {
			if (csn.compare("sequencer") != 0) {
				ls.lock()->openScreen("sequencer");
			}
			lSequencer->rec();
		}
		else if (controls->isOverDubPressed()) {
			if (csn.compare("sequencer") != 0) {
				ls.lock()->openScreen("sequencer");
			}
			lSequencer->overdub();
		}
		else {
			if (controls->isShiftPressed() && !Mpc::instance().getAudioMidiServices().lock()->isBouncing()) {
				Mpc::instance().getUis().lock()->getD2DRecorderGui()->setSq(lSequencer->getActiveSequenceIndex());
				ls.lock()->openScreen("directtodiskrecorder");
			}
			else {
				if (csn.compare("song") != 0 && csn.compare("sequencer") != 0 && csn.compare("trackmute") != 0) {
					ls.lock()->openScreen("sequencer");
				}
				if (csn.compare("song") == 0) {
					lSequencer->setSongModeEnabled(true);
				}
				lSequencer->play();
			}
		}
	}
}

void BaseControls::playStart()
{
	init();
	auto hw = Mpc::instance().getHardware().lock();
	auto controls = Mpc::instance().getControls().lock();
	auto lSequencer = sequencer.lock();
	if (lSequencer->isPlaying()) return;

	if (controls->isRecPressed()) {
		if (csn.compare("sequencer") != 0) {
			ls.lock()->openScreen("sequencer");
		}
		lSequencer->recFromStart();
	}
	else if (controls->isOverDubPressed()) {
		if (csn.compare("sequencer") != 0) {
			ls.lock()->openScreen("sequencer");
		}
		lSequencer->overdubFromStart();
	}
	else {
		if (controls->isShiftPressed()) {
			Mpc::instance().getUis().lock()->getD2DRecorderGui()->setSq(lSequencer->getActiveSequenceIndex());
			ls.lock()->openScreen("directtodiskrecorder");
		}
		else {
			if (csn.compare("song") != 0 && csn.compare("sequencer") != 0 && csn.compare("trackmute") != 0) {
				ls.lock()->openScreen("sequencer");
			}
			if (csn.compare("song") == 0) {
				lSequencer->setSongModeEnabled(true);
			}
			lSequencer->playFromStart();
		}
	}
	hw->getLed("play").lock()->light(lSequencer->isPlaying());
	hw->getLed("rec").lock()->light(lSequencer->isRecording());
	hw->getLed("overdub").lock()->light(lSequencer->isOverDubbing());
}

void BaseControls::mainScreen()
{
    init();

	auto ams = Mpc::instance().getAudioMidiServices().lock();
	if (ams->isRecordingSound()) {
		ams->stopSoundRecorder();
	}
	ls.lock()->openScreen("sequencer");
	auto lSequencer = sequencer.lock();
    lSequencer->setSoloEnabled(lSequencer->isSoloEnabled());
}

void BaseControls::tap()
{
	init();
	auto controls = Mpc::instance().getControls().lock();
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
	auto controls = Mpc::instance().getControls().lock();
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
	ls.lock()->openScreen("nextseq");
}

void BaseControls::trackMute()
{
	init();
	if (csn.compare("trackmute") == 0) {
		ls.lock()->openScreen("sequencer");
		return;
	}
	sequencer.lock()->setSoloEnabled(false);
	ls.lock()->openScreen("trackmute");
	Mpc::instance().getHardware().lock()->getLed("trackmute").lock()->light(true);
}

void BaseControls::bank(int i)
{
	init();
	auto oldBank = samplerGui->getBank();
	samplerGui->setBank(i);
	auto diff = 16 * (i - oldBank);
	auto newPadNr = samplerGui->getPad() + diff;
	auto newNN = program.lock()->getPad(newPadNr)->getNote();
	samplerGui->setPadAndNote(newPadNr, newNN);
	for (int i=0;i<16;i++)
		Mpc::instance().getHardware().lock()->getPad(i).lock()->notifyObservers(255);
}

void BaseControls::fullLevel()
{
    init();
	auto hardware = mpc::Mpc::instance().getHardware().lock();
	auto topPanel = hardware->getTopPanel().lock();
    
	topPanel->setFullLevelEnabled(!topPanel->isFullLevelEnabled());

	hardware->getLed("fulllevel").lock()->light(topPanel->isFullLevelEnabled());
}

void BaseControls::sixteenLevels()
{
	init();
	auto hardware = Mpc::instance().getHardware().lock();
	auto topPanel = hardware->getTopPanel().lock();
	if (topPanel->isSixteenLevelsEnabled()) {
		topPanel->setSixteenLevelsEnabled(false);
		hardware->getLed("sixteenlevels").lock()->light(false);
	}
	else {
		ls.lock()->openScreen("assign16levels");
	}
}

void BaseControls::after()
{
	init();
	auto hw = Mpc::instance().getHardware().lock();
	auto topPanel = hw->getTopPanel().lock();
	auto controls = Mpc::instance().getControls().lock();

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
	auto controls = Mpc::instance().getControls().lock();
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
	int sliderValue = Mpc::instance().getHardware().lock()->getSlider().lock()->getValue();
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
	auto controls = Mpc::instance().getControls().lock();
	controls->setErasePressed(true);
	auto lSequencer = sequencer.lock();

	if (!lSequencer->getActiveSequence().lock()->isUsed())
		return;

	if (lSequencer->isOverDubbing()) {
	}
	else {
		Mpc::instance().getUis().lock()->getSequencerWindowGui()->setTime0(0);
		Mpc::instance().getUis().lock()->getSequencerWindowGui()->setTime1(lSequencer->getActiveSequence().lock()->getLastTick());
		ls.lock()->openScreen("erase");
	}
}

BaseControls::~BaseControls() {
}
