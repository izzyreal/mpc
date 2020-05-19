#include <lcdgui/screens/NextSeqScreen.hpp>

#include <lcdgui/LayeredScreen.hpp>
#include <sequencer/Sequencer.hpp>

using namespace mpc::lcdgui::screens;
using namespace std;

NextSeqScreen::NextSeqScreen(const int& layer)
	: ScreenComponent("nextseq", layer)
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
		ls.lock()->openScreen("nextseqpad");
		break;
	}
}

void NextSeqScreen::displaySq()
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

void NextSeqScreen::displayNextSq()
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
	tempo = Util::replaceDotWithSmallSpaceDot(tempo);
	findField("tempo").lock()->setText(tempo);
	displayTempoLabel();
}

void NextSeqScreen::displayTempoLabel()
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

void NextSeqScreen::displayTempoSource()
{
	tempoSourceField.lock()->setText(sequencer.lock()->isTempoSourceSequenceEnabled() ? "(SEQ)" : "(MAS)");
}

void NextSeqScreen::displayTiming()
{
	auto timingCorrectScreen = dynamic_pointer_cast<TimingCorrectScreen>(Screens::getScreenComponent("timingcorrect"));
	auto noteValue = timingCorrectScreen->getNoteValue();
	timingField.lock()->setText(mpc::lcdgui::screens::SequencerScreen::timingCorrectNames[noteValue]);
}

