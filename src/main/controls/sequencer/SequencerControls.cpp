#include <controls/sequencer/SequencerControls.hpp>
#include <ui/Uis.hpp>
#include <StartUp.hpp>
#include <lcdgui/LayeredScreen.hpp>
////#include <ui/UserDefaults.hpp>
#include <lcdgui/Field.hpp>
#include <ui/sequencer/EditSequenceGui.hpp>
#include <ui/sequencer/StepEditorGui.hpp>
#include <ui/sequencer/window/SequencerWindowGui.hpp>
#include <sequencer/Sequencer.hpp>
#include <sequencer/Event.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/NoteEvent.hpp>
#include <sequencer/TimeSignature.hpp>
#include <sequencer/Track.hpp>

#include <lang/StrUtil.hpp>

#include <limits>

using namespace mpc::controls::sequencer;
using namespace std;

SequencerControls::SequencerControls()
	: AbstractSequencerControls()
{
}

void SequencerControls::init()
{
    AbstractSequencerControls::init();
    //const char* _typableParams[] = { "tempo", "now0", "now1", "now2", "velo"};
	//this->typableParams = vector<string>(_typableParams, _typableParams + sizeof(_typableParams));
}

void SequencerControls::pressEnter()
{
	AbstractSequencerControls::pressEnter();
	init();
	if (!isTypable()) return;
	auto lSequencer = sequencer.lock();
	auto mtf = ls.lock()->lookupField(param).lock();
	if (!mtf->isTypeModeEnabled()) return;

	auto candidate = mtf->enter();
	auto lTrk = track.lock();
	if (candidate != INT_MAX) {
		if (param.compare("now0") == 0) {
			lSequencer->setBar(candidate - 1);
			ls.lock()->setLastFocus("sequencer_step", "viewmodenumber");
		}
		else if (param.compare("now1") == 0) {
			lSequencer->setBeat(candidate - 1);
			ls.lock()->setLastFocus("sequencer_step", "viewmodenumber");
		}
		else if (param.compare("now2") == 0) {
			lSequencer->setClock(candidate);
			ls.lock()->setLastFocus("sequencer_step", "viewmodenumber");
		}
		else if (param.compare("tempo") == 0) {
			lSequencer->setTempo(BCMath(candidate / 10.0));
		}
		else if (param.compare("velo") == 0) {
			lTrk->setVelocityRatio(candidate);
		}
	}
}

void SequencerControls::function(int i)
{
	init();
	BaseControls::function(i);
	auto seq = sequence.lock();
	auto lSequencer = sequencer.lock();
	auto lTrk = track.lock();
	switch (i) {
	case 0:
		ls.lock()->openScreen("sequencer_step");
		break;
	case 1:
		editSequenceGui->setTime1(seq->getLastTick());
		ls.lock()->openScreen("edit");
		break;
	case 2:
		lTrk->setOn(!lTrk->isOn());
		break;
	case 3:
		lSequencer->setSoloEnabled(!lSequencer->isSoloEnabled());
		break;
	case 4:
		lSequencer->trackDown();
		break;
	case 5:
		lSequencer->trackUp();
		break;
	}
}

void SequencerControls::turnWheel(int i)
{
	init();
	auto lSequencer = sequencer.lock();
	auto lTrk = track.lock();
	auto seq = sequence.lock();
	if (param.size() >= 3 && param.substr(0, 3).compare("now") == 0) {
		ls.lock()->setLastFocus("sequencer_step", "viewmodenumber");
	}

	if (param.compare("now0") == 0) {
		lSequencer->setBar(lSequencer->getCurrentBarNumber() + i);
	}
	else if (param.compare("now1") == 0) {
		lSequencer->setBeat(lSequencer->getCurrentBeatNumber() + i);
	}
	else if (param.compare("now2") == 0) {
		lSequencer->setClock(lSequencer->getCurrentClockNumber() + i);
	}
	else if (param.compare("devicenumber") == 0) {
		lTrk->setDeviceNumber(lTrk->getDevice() + i);
	}
	else if (param.compare("tr") == 0) {
		if (i > 0) {
			lSequencer->trackUp();
		}
		else if (i < 0) {
			lSequencer->trackDown();
		}
	}
	else if (param.compare("tracktype") == 0) {
		auto lTrk = track.lock();
		lTrk->setBusNumber(lTrk->getBusNumber() + i);
		auto lastFocus = ls.lock()->getLastFocus("sequencer_step");
		if (lastFocus.length() == 2) {
			auto eventNumber = stoi(lastFocus.substr(1, 2));
			if (dynamic_pointer_cast<mpc::sequencer::NoteEvent>(seGui->getVisibleEvents()[eventNumber].lock())) {
				if (lTrk->getBusNumber() == 0) {
					if (lastFocus[0] == 'd' || lastFocus[0] == 'e') {
						ls.lock()->setLastFocus("sequencer_step", "a" + to_string(eventNumber));
					}
				}
			}
		}
		lastFocus = ls.lock()->getLastFocus("edit");

		string midinote = "midinote";
		string drumnote = "drumnote";

		if (lastFocus.compare("") != 0) {
			if (std::mismatch(midinote.begin(), midinote.end(), lastFocus.begin()).first == midinote.end()) {
				if (lTrk->getBusNumber() != 0) {
					ls.lock()->setLastFocus("edit", "drumnote");
				}
			}
			else if (std::mismatch(drumnote.begin(), drumnote.end(), lastFocus.begin()).first == drumnote.end()) {
				if (lTrk->getBusNumber() == 0) {
					ls.lock()->setLastFocus("edit", "midinote0");
				}
			}
		}
	}
	else if (param.compare("pgm") == 0) {
		lTrk->setProgramChange(lTrk->getProgramChange() + i);
	}
	else if (param.compare("velo") == 0) {
		lTrk->setVelocityRatio(lTrk->getVelocityRatio() + i);
	}
	else if (param.compare("timing") == 0) {
		swGui->setNoteValue(swGui->getNoteValue() + i);
	}
	else if (param.compare("sq") == 0) {
		if (lSequencer->isPlaying()) {
			lSequencer->setNextSq(lSequencer->getCurrentlyPlayingSequenceIndex() + i);
		}
		else {
			lSequencer->setActiveSequenceIndex(lSequencer->getActiveSequenceIndex() + i);
		}
	}
	else if (param.compare("nextsq") == 0) {
		lSequencer->setNextSq(lSequencer->getNextSq() + i);
	}
	else if (param.compare("bars") == 0) {
		Mpc::instance().getUis().lock()->getSequencerWindowGui()->setNewBars(lSequencer->getActiveSequence().lock()->getLastBar());
		ls.lock()->openScreen("changebars2");
	}
	else if (param.compare("tempo") == 0) {
		double oldTempo = lSequencer->getTempo().toDouble();
		double newTempo = oldTempo + (i / 10.0);
		lSequencer->setTempo(BCMath(newTempo));
	}
	else if (param.compare("tsig") == 0) {
		mpc::sequencer::TimeSignature ts;
		auto oldTs = seq->getTimeSignature();
		ts.setNumerator(oldTs.getNumerator());
		ts.setDenominator(oldTs.getDenominator());
		Mpc::instance().getUis().lock()->getSequencerWindowGui()->setNewTimeSignature(ts);
		ls.lock()->openScreen("changetsig");
	}
	else if (param.compare("temposource") == 0) {
		lSequencer->setTempoSourceSequence(i > 0);
	}
	else if (param.compare("count") == 0) {
		lSequencer->setCountEnabled(i > 0);
	}
	else if (param.compare("loop") == 0) {
		seq->setLoopEnabled(i > 0);
	}
	else if (param.compare("recordingmode") == 0) {
		lSequencer->setRecordingModeMulti(i > 0);
	}
	else if (param.compare("on") == 0) {
		lTrk->setOn(i > 0);
	}
}

void SequencerControls::openWindow()
{
	init();
	auto lSequencer = sequencer.lock();
	if (lSequencer->isPlaying()) return;
	auto seq = sequence.lock();
	if (param.compare("sq") == 0) {
		ls.lock()->openScreen("sequence");
	}
	else if (param.find("now") != string::npos) {
		ls.lock()->openScreen("timedisplay");
	}
	else if (param.find("tempo") != string::npos) {
		ls.lock()->openScreen("tempochange");
	}
	else if (param.compare("timing") == 0) {
		ls.lock()->openScreen("timingcorrect");
	}
	else if (param.compare("tsig") == 0) {
		mpc::sequencer::TimeSignature ts;
		auto oldTs = seq->getTimeSignature();
		ts.setNumerator(oldTs.getNumerator());
		ts.setDenominator(oldTs.getDenominator());
		Mpc::instance().getUis().lock()->getSequencerWindowGui()->setNewTimeSignature(ts);
		ls.lock()->openScreen("changetsig");
	}
	else if (param.compare("count") == 0) {
		ls.lock()->openScreen("countmetronome");
	}
	else if (param.compare("loop") == 0) {
		ls.lock()->openScreen("loopbarswindow");
	}
	else if (param.compare("tr") == 0) {
		ls.lock()->openScreen("track");
	}
	else if (param.compare("on") == 0) {
		ls.lock()->openScreen("eraseallofftracks");
	}
	else if (param.compare("pgm") == 0) {
		ls.lock()->openScreen("transmitprogramchanges");
	}
	else if (param.compare("recordingmode") == 0) {
		ls.lock()->openScreen("multirecordingsetup");
	}
	else if (param.compare("tracktype") == 0) {
		ls.lock()->openScreen("midiinput");
	}
	else if (param.compare("devicenumber") == 0) {
		ls.lock()->openScreen("midioutput");
	}
	else if (param.compare("bars") == 0) {
		if (swGui->getChangeBarsAfterBar() > seq->getLastBar() + 1) swGui->setChangeBarsAfterBar(seq->getLastBar() + 1, seq->getLastBar());
		if (swGui->getChangeBarsFirstBar() > seq->getLastBar()) swGui->setChangeBarsFirstBar(seq->getLastBar(), seq->getLastBar());
		if (swGui->getChangeBarsLastBar() > seq->getLastBar()) swGui->setChangeBarsLastBar(seq->getLastBar(), seq->getLastBar());
		ls.lock()->openScreen("changebars");
	}
	else if (param.compare("velo") == 0) {
		Mpc::instance().getUis().lock()->getSequencerWindowGui()->setTime0(0);
		Mpc::instance().getUis().lock()->getSequencerWindowGui()->setTime1(seq->getLastTick());
		ls.lock()->openScreen("editvelocity");
	}
}

void SequencerControls::left()
{
    init();
	
	auto lSequencer = sequencer.lock();
	
	if (lSequencer->getNextSq() != -1) {
		return;
	}

	BaseControls::left();
}

void SequencerControls::right()
{
	init();
	
	auto lSequencer = sequencer.lock();
	
	if (lSequencer->getNextSq() != -1) {
		return;
	}

	auto seq = sequence.lock();
	
	if (!seq->isUsed()) {
        seq->init(mpc::StartUp::getUserDefaults().lock()->getLastBarIndex());
		int index = lSequencer->getActiveSequenceIndex();
		string name = moduru::lang::StrUtil::trim(lSequencer->getDefaultSequenceName()) + moduru::lang::StrUtil::padLeft(to_string(index + 1), "0", 2);
		seq->setName(name);
		lSequencer->setActiveSequenceIndex(lSequencer->getActiveSequenceIndex());
    }
	BaseControls::right();
}

void SequencerControls::up()
{
    init();
	
	if (sequencer.lock()->getNextSq() != -1) {
		return;
	}
	
	BaseControls::up();
}

void SequencerControls::down()
{
	init();
	
	auto lSequencer = sequencer.lock();
	
	if (lSequencer->getNextSq() != -1) {
		return;
	}

	auto seq = sequence.lock();
	if (!seq->isUsed()) {
        seq->init(mpc::StartUp::getUserDefaults().lock()->getLastBarIndex());
		int index = lSequencer->getActiveSequenceIndex();
		string name = moduru::lang::StrUtil::trim(lSequencer->getDefaultSequenceName()) + moduru::lang::StrUtil::padLeft(to_string(index + 1), "0", 2);
		seq->setName(name);
		lSequencer->setActiveSequenceIndex(lSequencer->getActiveSequenceIndex());
    }
	BaseControls::down();
}
