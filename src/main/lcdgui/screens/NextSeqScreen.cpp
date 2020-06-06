#include "NextSeqScreen.hpp"

#include <lcdgui/screens/window/TimingCorrectScreen.hpp>
#include <lcdgui/screens/SequencerScreen.hpp>

#include <sequencer/TempoChangeEvent.hpp>

#include <Util.hpp>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace moduru::lang;
using namespace std;

NextSeqScreen::NextSeqScreen(const int layerIndex)
	: ScreenComponent("next-seq", layerIndex)
{
}

void NextSeqScreen::open()
{
	displaySq();
	displayNow0();
	displayNow1();
	displayNow2();
	displayTempo();
	displayTempoSource();
	displayTiming();
	displayNextSq();
	
	sequencer.lock()->addObserver(this);
}

void NextSeqScreen::close()
{
	sequencer.lock()->deleteObserver(this);
}

void NextSeqScreen::turnWheel(int i)
{
	init();
	
	if (param.compare("sq") == 0) {
		if (sequencer.lock()->isPlaying()) {
			sequencer.lock()->setNextSq(sequencer.lock()->getCurrentlyPlayingSequenceIndex() + i);
		}
		else {
			sequencer.lock()->setActiveSequenceIndex(sequencer.lock()->getActiveSequenceIndex() + i);
		}
	}
	if (param.compare("nextsq") == 0)
		sequencer.lock()->setNextSq(sequencer.lock()->getNextSq() + i);
}

void NextSeqScreen::function(int i)
{
	init();
	switch (i) {
	case 5:
		ls.lock()->openScreen("next-seq-pad");
		break;
	}
}

void NextSeqScreen::displaySq()
{
	string result = "";
	auto lSequencer = sequencer.lock();
	if (lSequencer->isPlaying()) {
		result.append(StrUtil::padLeft(to_string(lSequencer->getCurrentlyPlayingSequenceIndex() + 1), "0", 2));
		result.append("-");
		result.append(lSequencer->getCurrentlyPlayingSequence().lock()->getName());
		findField("sq").lock()->setText(result);
	}
	else {
		result.append(StrUtil::padLeft(to_string(lSequencer->getActiveSequenceIndex() + 1), "0", 2));
		result.append("-");
		result.append(lSequencer->getActiveSequence().lock()->getName());
		findField("sq").lock()->setText(result);
	}
}

void NextSeqScreen::displayNextSq()
{
	auto lSequencer = sequencer.lock();
	auto nextSq = lSequencer->getNextSq();
	string res = "";

	if (nextSq != -1)
	{
		auto seqName = lSequencer->getSequence(nextSq).lock()->getName();
		res = StrUtil::padLeft(to_string(lSequencer->getNextSq() + 1), "0", 2) + "-" + seqName;
	}
	
	findField("nextsq").lock()->setText(res);
}

void NextSeqScreen::displayNow0()
{
	findField("now0").lock()->setTextPadded(sequencer.lock()->getCurrentBarNumber() + 1, "0");
}

void NextSeqScreen::displayNow1()
{
	findField("now1").lock()->setTextPadded(sequencer.lock()->getCurrentBeatNumber() + 1, "0");
}

void NextSeqScreen::displayNow2()
{
	findField("now2").lock()->setTextPadded(sequencer.lock()->getCurrentClockNumber(), "0");
}

void NextSeqScreen::displayTempo()
{
	auto tempo = sequencer.lock()->getTempo().toString();
	tempo = mpc::Util::replaceDotWithSmallSpaceDot(tempo);
	findField("tempo").lock()->setText(tempo);
	displayTempoLabel();
}

void NextSeqScreen::displayTempoLabel()
{
	auto currentRatio = -1;
	auto lSequencer = sequencer.lock();
	auto seq = lSequencer->isPlaying() ? lSequencer->getCurrentlyPlayingSequence().lock() : lSequencer->getActiveSequence().lock();
	for (auto& e : seq->getTempoChangeEvents())
	{
		auto tce = e.lock();
		if (tce->getTick() > lSequencer->getTickPosition())
		{
			break;
		}
		currentRatio = tce->getRatio();
	}
	if (currentRatio != 1000) {
		findLabel("tempo").lock()->setText(u8"c\u00C0:");
	}
	else {
		findLabel("tempo").lock()->setText(u8" \u00C0:");
	}
}

void NextSeqScreen::displayTempoSource()
{
	findField("temposource").lock()->setText(sequencer.lock()->isTempoSourceSequenceEnabled() ? "(SEQ)" : "(MAS)");
}

void NextSeqScreen::displayTiming()
{
	auto timingCorrectScreen = dynamic_pointer_cast<TimingCorrectScreen>(Screens::getScreenComponent("timing-correct"));
	auto noteValue = timingCorrectScreen->getNoteValue();
	findField("timing").lock()->setText(SequencerScreen::timingCorrectNames[noteValue]);
}

void NextSeqScreen::update(moduru::observer::Observable* o, nonstd::any arg)
{
	string s = nonstd::any_cast<string>(arg);

	if (s.compare("seqnumbername") == 0)
	{
		displaySq();
	}
	else if (s.compare("bar") == 0)
	{
		displayNow0();
	}
	else if (s.compare("beat") == 0)
	{
		displayNow1();
	}
	else if (s.compare("clock") == 0)
	{
		displayNow2();
	}
	else if (s.compare("now") == 0)
	{
		displayNow0();
		displayNow1();
		displayNow2();
	}
	else if (s.compare("nextsqvalue") == 0)
	{
		displayNextSq();
	}
	else if (s.compare("nextsq") == 0)
	{
		findField("nextsq").lock()->Hide(false);
		displayNextSq();
	}
	else if (s.compare("nextsqoff") == 0) {
		findField("nextsq").lock()->Hide(true);
	}
	else if (s.compare("timing") == 0) {
		displayTiming();
	}
}
