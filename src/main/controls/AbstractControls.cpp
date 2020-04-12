#include "AbstractControls.hpp"

#include <Mpc.hpp>

#include <controls/Controls.hpp>

#include <ui/Uis.hpp>

#include <hardware/Hardware.hpp>
#include <hardware/Led.hpp>
#include <hardware/HwSlider.hpp>
#include <hardware/HwPad.hpp>

#include <sequencer/Sequencer.hpp>
#include <sampler/Sampler.hpp>
#include <sampler/Program.hpp>

#include <StartUp.hpp>

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
//#include <ui/vmpc/AudioGui.hpp>
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

AbstractControls::AbstractControls(Mpc* mpc)
{
	this->mpc = mpc;
	sequencer = mpc->getSequencer();
	sampler = mpc->getSampler();
	ls = mpc->getLayeredScreen();
	sequencerGui = mpc->getUis().lock()->getSequencerGui();
	nameGui = mpc->getUis().lock()->getNameGui();
	samplerGui = mpc->getUis().lock()->getSamplerGui();
}

void AbstractControls::init()
{
	csn = ls.lock()->getCurrentScreenName();
	param = ls.lock()->getFocus();
	activeField = ls.lock()->lookupField(param);
	auto lSequencer = sequencer.lock();
    track = lSequencer->getActiveSequence().lock()->getTrack(lSequencer->getActiveTrackIndex());
	auto lTrk = track.lock();
	if (lTrk->getBusNumber() != 0 && mpc->getAudioMidiServices().lock()->getAudioServer()->isRunning()) {
		auto lSampler = sampler.lock();
        mpcSoundPlayerChannel = lSampler->getDrum(lTrk->getBusNumber() - 1);
		program = dynamic_pointer_cast<mpc::sampler::Program>(lSampler->getProgram(mpcSoundPlayerChannel->getProgram()).lock());
    }
    bank_ = samplerGui->getBank();
}

void AbstractControls::left()
{
	init();
	if (!activeField.lock()) return;
	if (param.compare("dummy") == 0) return;
	ls.lock()->transferFocus(true);
}

void AbstractControls::right()
{
	init();
	if (!activeField.lock()) return;
	if (param.compare("dummy") == 0) return;
	ls.lock()->transferFocus(false);
}

void AbstractControls::up()
{
    init();
	auto aboveCandidate = ls.lock()->findAbove(param);
	if (param.compare(aboveCandidate) != 0) {
		ls.lock()->setFocus(aboveCandidate);
	}
}

void AbstractControls::down()
{
    init();
	auto belowCandidate = ls.lock()->findBelow(param);
	if (param.compare(belowCandidate) != 0) {
		ls.lock()->setFocus(belowCandidate);
	}
}

void AbstractControls::function(int i)
{
	init();
	auto lsLocked = ls.lock();
	switch (i) {
	case 3:
		if (lsLocked->getCurrentLayer() == 1) {
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
				lsLocked->setPreviousScreenName(mpc->getUis().lock()->getEditSoundGui()->getPreviousScreenName());
			}
			else if (csn.compare("sound") == 0) {
				lsLocked->setPreviousScreenName(mpc->getUis().lock()->getSoundGui()->getPreviousScreenName());
			}
			else if (csn.compare("program") == 0) {
				lsLocked->setPreviousScreenName(mpc->getUis().lock()->getSamplerGui()->getPrevScreenName());
			}
			else if (csn.compare("name") == 0) {
				mpc->getUis().lock()->getNameGui()->setNameBeingEdited(false);
				lsLocked->setLastFocus("name", "0");
			}
			else if (csn.compare("numberofzones") == 0) {
				mpc->getUis().lock()->getSoundGui()->setNumberOfZones(mpc->getUis().lock()->getSoundGui()->getPreviousNumberOfzones());
			}
			else if (csn.compare("directory") == 0) {
				lsLocked->setPreviousScreenName(mpc->getUis().lock()->getDirectoryGui()->getPreviousScreenName());
			}
			if (lsLocked->getPreviousScreenName().compare("load") == 0) {
				if (mpc->getUis().lock()->getDiskGui()->getFileLoad() + 1 > mpc->getDisk().lock()->getFiles().size()) {
					mpc->getUis().lock()->getDiskGui()->setFileLoad(0);
				}
			}
		}
		if (lsLocked->getCurrentLayer() == 1 || lsLocked->getCurrentLayer() == 2 || lsLocked->getCurrentLayer() == 3) {
			lsLocked->openScreen(lsLocked->getPreviousScreenName());
		}
		break;
	case 5:
		mpc->getControls().lock()->setF6Pressed(true);
		break;
	}
}

void AbstractControls::openWindow()
{
}

void AbstractControls::turnWheel(int i)
{
}

void AbstractControls::pad(int i, int velo, bool repeat, int tick)
{
	init();
	auto lTrk = track.lock();
	auto controls = mpc->getControls().lock();
	auto lSequencer = sequencer.lock();
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
	if (mpc->getUis().lock()->getSequencerGui()->isSixteenLevelsEnabled()) {
		if (mpc->getUis().lock()->getSequencerGui()->getParameter() == 0) {
			note = mpc->getUis().lock()->getSequencerGui()->getNote();
			velocity = (int)(i * (127.0 / 16.0));
			if (velocity == 0) {
				velocity = 1;
			}
		}
	}
	else {
		if (csn.compare("programparams") == 0) {
			if (note > 34) {
				samplerGui->setPadAndNote(pad, note);
			}
		}
		else {
			samplerGui->setPadAndNote(pad, note);
		}
	}
	if (csn.compare("assign16levels") == 0 && note != 34) {
		mpc->getUis().lock()->getSequencerGui()->setNote(note);
	}

	if (controls->isTapPressed() && lSequencer->isPlaying()) {
		if (repeat) {
			generateNoteOn(note, velocity, tick);
		}
	}
	else {
		generateNoteOn(note, velocity, -1);
	}
}

void AbstractControls::generateNoteOn(int nn, int padVelo, int tick)
{
	init();
	auto lTrk = track.lock();
	auto lProgram = program.lock();
	bool slider = lProgram && nn == lProgram->getSlider()->getNote();
	auto lSequencer = sequencer.lock();
	bool posIsLastTick = lSequencer->getTickPosition() == lSequencer->getActiveSequence().lock()->getLastTick();
	bool step = csn.compare("sequencer_step") == 0 && !posIsLastTick;
	auto swGui = mpc->getUis().lock()->getSequencerWindowGui();
	bool recMainWithoutPlaying = csn.compare("sequencer") == 0 && !lSequencer->isPlaying() && mpc->getControls().lock()->isRecPressed() && swGui->getNoteValue() != 0 && !posIsLastTick;
	shared_ptr<mpc::sequencer::NoteEvent> n;
	if (lSequencer->isRecordingOrOverdubbing() || step || recMainWithoutPlaying) {
		if (step) {
			n = lTrk->addNoteEvent(lSequencer->getTickPosition(), nn).lock();
		}
		else if (recMainWithoutPlaying) {
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
		
		if (sequencerGui->isSixteenLevelsEnabled() && sequencerGui->getParameter() == 1) {
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

	if (sequencerGui->isSixteenLevelsEnabled() && sequencerGui->getParameter() == 1) {

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

	mpc->getEventHandler().lock()->handle(noteEvent, lTrk.get());
}

void AbstractControls::numpad(int i)
{
	init();
	auto controls = mpc->getControls().lock();
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
	auto lDisk = mpc->getDisk().lock();

	if (controls->isShiftPressed()) {
		switch (i) {
		case 1:
			if (lSequencer->isPlaying()) return;
			ls.lock()->openScreen("song");
			return;
		case 2:
			if (lSequencer->isPlaying()) return;
			ls.lock()->openScreen("punch");
			mpc->getUis().lock()->getPunchGui()->setTime0(0);
			mpc->getUis().lock()->getPunchGui()->setTime1(lSequencer->getActiveSequence().lock()->getLastTick());
			return;
		case 3:
			if (lSequencer->isPlaying()) return;
			if (mpc != nullptr && lDisk != nullptr) lDisk->initFiles();

			if (mpc->getUis().lock()->getDiskGui()->getFileLoad() + 1 > lDisk->getFiles().size()) {
				mpc->getUis().lock()->getDiskGui()->setFileLoad((int)(lDisk->getFiles().size()) - 1);
			}
			ls.lock()->openScreen("load");
			return;
		case 4:
			if (lSequencer->isPlaying()) return;
			mpc->getAudioMidiServices().lock()->startSampling();
			ls.lock()->openScreen("sample");
			return;
		case 5:
			if (lSequencer->isPlaying()) return;
			ls.lock()->openScreen("trim");
			return;
		case 6:
		case 130:
			ls.lock()->openScreen("selectdrum");
			return;
		case 7:
			ls.lock()->openScreen("selectdrum_mixer");
			return;
		case 8:
			if (lSequencer->isPlaying()) return;
			ls.lock()->openScreen("others");
			return;
		case 9:
			if (lSequencer->isPlaying()) return;
			ls.lock()->openScreen("sync");
			return;
		}

	}
}

void AbstractControls::pressEnter()
{
	init();
	auto controls = mpc->getControls().lock();
	if (controls->isShiftPressed()) {
		ls.lock()->openScreen("save");
	}
}

void AbstractControls::rec()
{
	auto controls = mpc->getControls().lock();
	controls->setRecPressed(true);
    init();
	auto lSequencer = sequencer.lock();
	auto hw = mpc->getHardware().lock();
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

void AbstractControls::overDub()
{
	auto controls = mpc->getControls().lock();
	controls->setOverDubPressed(true);
	init();
	auto hw = mpc->getHardware().lock();
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

void AbstractControls::stop()
{
	init();
	auto lSequencer = sequencer.lock();
	lSequencer->stop();
	auto controls = mpc->getControls().lock();
	if (controls->isShiftPressed())
		mpc->getAudioMidiServices().lock()->stopBouncing();
}

void AbstractControls::play()
{
	init();
	auto controls = mpc->getControls().lock();
	auto hw = mpc->getHardware().lock();
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
			if (controls->isShiftPressed() && !mpc->getAudioMidiServices().lock()->isBouncing()) {
				mpc->getUis().lock()->getD2DRecorderGui()->setSq(lSequencer->getActiveSequenceIndex());
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

void AbstractControls::playStart()
{
	init();
	auto hw = mpc->getHardware().lock();
	auto controls = mpc->getControls().lock();
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
			mpc->getUis().lock()->getD2DRecorderGui()->setSq(lSequencer->getActiveSequenceIndex());
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

void AbstractControls::mainScreen()
{
    init();
	auto ams = mpc->getAudioMidiServices().lock();
	if (ams->isSampling()) {
		ams->stopSampling();
	}
	ls.lock()->openScreen("sequencer");
	auto lSequencer = sequencer.lock();
    lSequencer->setSoloEnabled(lSequencer->isSoloEnabled());
}

void AbstractControls::tap()
{
	init();
	auto controls = mpc->getControls().lock();
	controls->setTapPressed(true);
	sequencer.lock()->tap();
}

void AbstractControls::prevStepEvent()
{
}

void AbstractControls::nextStepEvent()
{
}

void AbstractControls::goTo()
{
	init();
	auto controls = mpc->getControls().lock();
	controls->setGoToPressed(true);
}

void AbstractControls::prevBarStart()
{
}

void AbstractControls::nextBarEnd()
{
}

void AbstractControls::nextSeq()
{
	init();
	ls.lock()->openScreen("nextseq");
}

void AbstractControls::trackMute()
{
	init();
	if (csn.compare("trackmute") == 0) {
		ls.lock()->openScreen("sequencer");
		return;
	}
	sequencer.lock()->setSoloEnabled(false);
	ls.lock()->openScreen("trackmute");
	mpc->getHardware().lock()->getLed("trackmute").lock()->light(true);
}

void AbstractControls::bank(int i)
{
	init();
	auto oldBank = samplerGui->getBank();
	samplerGui->setBank(i);
	auto diff = 16 * (i - oldBank);
	auto newPadNr = samplerGui->getPad() + diff;
	auto newNN = program.lock()->getPad(newPadNr)->getNote();
	samplerGui->setPadAndNote(newPadNr, newNN);
	for (int i=0;i<16;i++)
		mpc->getHardware().lock()->getPad(i).lock()->notifyObservers(255);
}

void AbstractControls::fullLevel()
{
    init();
    sequencerGui->setFullLevelEnabled(!sequencerGui->isFullLevelEnabled());
	auto hw = mpc->getHardware().lock();
	hw->getLed("fulllevel").lock()->light(sequencerGui->isFullLevelEnabled());
}

void AbstractControls::sixteenLevels()
{
	init();
	auto hw = mpc->getHardware().lock();
	if (sequencerGui->isSixteenLevelsEnabled()) {
		sequencerGui->setSixteenLevelsEnabled(false);
		hw->getLed("sixteenlevels").lock()->light(false);
	}
	else {
		ls.lock()->openScreen("assign16levels");
	}
}

void AbstractControls::after()
{
	init();
	auto hw = mpc->getHardware().lock();
	auto controls = mpc->getControls().lock();
	if (controls->isShiftPressed()) {
		ls.lock()->openScreen("assign");
	}
	else {
		sequencerGui->setAfterEnabled(!sequencerGui->isAfterEnabled());
		hw->getLed("after").lock()->light(sequencerGui->isAfterEnabled());
	}
}

void AbstractControls::shift()
{
	auto controls = mpc->getControls().lock();
	if (controls->isShiftPressed())
		return;

	controls->setShiftPressed(true);
}

void AbstractControls::undoSeq()
{
	sequencer.lock()->undoSeq();
}

void AbstractControls::setSliderNoteVar(mpc::sequencer::NoteEvent* n, weak_ptr<mpc::sampler::Program> program)
{
	auto lProgram = program.lock();
	if (n->getNote() != lProgram->getSlider()->getNote())
		return;

	auto sliderParam = lProgram->getSlider()->getParameter();
	int rangeLow = 0, rangeHigh = 0, sliderRange = 0;
	n->setVariationTypeNumber(sliderParam);
	int sliderValue = mpc->getHardware().lock()->getSlider().lock()->getValue();
	double sliderRangeRatio = 0;
	int tuneValue;
	int decayValue;
	int attackValue;
	int filterValue;
	switch (sliderParam) {
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

bool AbstractControls::isTypable()
{
	if (typableParams.size() == 0)
		return false;

	for (auto str : typableParams)
		if (str.compare(param) == 0)
			return true;
	return false;
}

void AbstractControls::erase()
{
	init();
	auto controls = mpc->getControls().lock();
	controls->setErasePressed(true);
	auto lSequencer = sequencer.lock();

	if (!lSequencer->getActiveSequence().lock()->isUsed())
		return;

	if (lSequencer->isOverDubbing()) {
	}
	else {
		mpc->getUis().lock()->getSequencerWindowGui()->setTime0(0);
		mpc->getUis().lock()->getSequencerWindowGui()->setTime1(lSequencer->getActiveSequence().lock()->getLastTick());
		ls.lock()->openScreen("erase");
	}
}

AbstractControls::~AbstractControls() {
}
