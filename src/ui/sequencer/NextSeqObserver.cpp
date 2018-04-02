#include <ui/sequencer/NextSeqObserver.hpp>

#include <Mpc.hpp>
#include <Util.hpp>
#include <ui/Uis.hpp>
#include <ui/sequencer/window/SequencerWindowGui.hpp>
#include <lcdgui/LayeredScreen.hpp>
#include <lcdgui/Field.hpp>
#include <ui/sequencer/SequencerObserver.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/Sequencer.hpp>
#include <sequencer/TempoChangeEvent.hpp>
#include <thirdp/bcmath/bcmath_stl.h>
#include <lcdgui/Label.hpp>
#include <lcdgui/Field.hpp>

#include <lang/StrUtil.hpp>

using namespace mpc::ui::sequencer;
using namespace std;

NextSeqObserver::NextSeqObserver(mpc::Mpc* mpc) 
{
	this->mpc = mpc;
	sequencer = mpc->getSequencer();
	auto lSequencer = sequencer.lock();
	lSequencer->deleteObservers();
	lSequencer->addObserver(this);
	auto ls = mpc->getLayeredScreen().lock();
	sqField = ls->lookupField("sq");
	now0Field = ls->lookupField("now0");
	now1Field = ls->lookupField("now1");
	now2Field = ls->lookupField("now2");
	now0Field.lock()->setFocusable(false);
	now1Field.lock()->setFocusable(false);
	now2Field.lock()->setFocusable(false);
	tempoField = ls->lookupField("tempo");
	tempoLabel = ls->lookupLabel("tempo");
	tempoSourceField = ls->lookupField("temposource");
	timingField = ls->lookupField("timing");
	nextSqField = ls->lookupField("nextsq");
	displaySq();
	displayNow0();
	displayNow1();
	displayNow2();
	displayTempo();
	displayTempoSource();
	displayTiming();
	displayNextSq();
}

void NextSeqObserver::displaySq()
{
	string result{ "" };
	auto lSequencer = sequencer.lock();
	if (lSequencer->isPlaying()) {
		result.append(moduru::lang::StrUtil::padLeft(to_string(lSequencer->getCurrentlyPlayingSequenceIndex() + 1), "0", 2));
		result.append("-");
		result.append(lSequencer->getCurrentlyPlayingSequence().lock()->getName());
		sqField.lock()->setText(result);
	}
	else {
		result.append(moduru::lang::StrUtil::padLeft(to_string(lSequencer->getActiveSequenceIndex() + 1), "0", 2));
		result.append("-");
		result.append(lSequencer->getActiveSequence().lock()->getName());
		sqField.lock()->setText(result);
	}
}

void NextSeqObserver::displayNextSq()
{
	auto lSequencer = sequencer.lock();
	auto nextSq = lSequencer->getNextSq();
	string res = "";
	if (nextSq != -1) {
		auto seqName = lSequencer->getSequence(nextSq).lock()->getName();
		res = moduru::lang::StrUtil::padLeft(to_string(lSequencer->getNextSq() + 1), "0", 2) + "-" + seqName;
	}
    nextSqField.lock()->setText(res);
}

void NextSeqObserver::displayNow0()
{
	now0Field.lock()->setTextPadded(sequencer.lock()->getCurrentBarNumber() + 1, "0");
}

void NextSeqObserver::displayNow1()
{
	now1Field.lock()->setTextPadded(sequencer.lock()->getCurrentBeatNumber() + 1, "0");
}

void NextSeqObserver::displayNow2()
{
	now2Field.lock()->setTextPadded(sequencer.lock()->getCurrentClockNumber(), "0");
}

void NextSeqObserver::displayTempo()
{
	auto tempo = sequencer.lock()->getTempo().toString();
	tempo = Util::replaceDotWithSmallSpaceDot(tempo);
	tempoField.lock()->setText(tempo);
	displayTempoLabel();
}

void NextSeqObserver::displayTempoLabel()
{
	auto currentRatio = -1;
	auto lSequencer = sequencer.lock();
	auto seq = lSequencer->isPlaying() ? lSequencer->getCurrentlyPlayingSequence().lock() : lSequencer->getActiveSequence().lock();
	for (auto& e : seq->getTempoChangeEvents()) {
		auto tce = e.lock();
		if (tce->getTick() > lSequencer->getTickPosition()) {
			break;
		}
		currentRatio = tce->getRatio();
	}
	if (currentRatio != 1000) {
		tempoLabel.lock()->setText(u8"c\u00C0:");
	}
	else {
		tempoLabel.lock()->setText(u8" \u00C0:");
	}
}

void NextSeqObserver::displayTempoSource()
{
	tempoSourceField.lock()->setText(sequencer.lock()->isTempoSourceSequence() ? "(SEQ)" : "(MAS)");
}

void NextSeqObserver::displayTiming()
{
    timingField.lock()->setText(SequencerObserver::timingCorrectNames()->at(mpc->getUis().lock()->getSequencerWindowGui()->getNoteValue()));
}

void NextSeqObserver::update(moduru::observer::Observable* o, boost::any arg)
{
	string s = boost::any_cast<string>(arg);
	if (s.compare("seqnumbername") == 0) {
		displaySq();
	}
	else if (s.compare("bar") == 0) {
		displayNow0();
	}
	else if (s.compare("beat") == 0) {
		displayNow1();
	}
	else if (s.compare("clock") == 0) {
		displayNow2();
	}
	else if (s.compare("now") == 0) {
		displayNow0();
		displayNow1();
		displayNow2();
	}
	else if (s.compare("nextsqvalue") == 0) {
		displayNextSq();
	}
	else if (s.compare("nextsq") == 0) {
		nextSqField.lock()->Hide(false);
		//mainFrame->setFocus(nextSqField.lock()->getName(), 0);
		displayNextSq();
	}
	else if (s.compare("nextsqoff") == 0) {
		nextSqField.lock()->Hide(true);
		//mainFrame->setFocus("sq", 0);
	}
	else if (s.compare("timing") == 0) {
		displayTiming();
	}
}

NextSeqObserver::~NextSeqObserver() {
	sequencer.lock()->deleteObserver(this);
}
