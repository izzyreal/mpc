#include <ui/sequencer/NextSeqPadObserver.hpp>
#include <Mpc.hpp>
#include <ui/Uis.hpp>
#include <lcdgui/LayeredScreen.hpp>
#include <lcdgui/Field.hpp>
#include <ui/sampler/SamplerGui.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/Sequencer.hpp>
#include <lcdgui/Field.hpp>
#include <lcdgui/Label.hpp>

#include <lang/StrUtil.hpp>

using namespace mpc::ui::sequencer;
using namespace std;

NextSeqPadObserver::NextSeqPadObserver(mpc::Mpc* mpc)
{
	seqs = vector<weak_ptr<mpc::lcdgui::Label>>(16);
	this->sequencer = sequencer;
	auto lSequencer = sequencer.lock();
	lSequencer->deleteObservers();
	lSequencer->addObserver(this);
	samplerGui = mpc->getUis().lock()->getSamplerGui();
	samplerGui->deleteObservers();
	samplerGui->addObserver(this);
	auto ls = mpc->getLayeredScreen().lock();
	sqField = ls->lookupField("sq");
	nextSqLabel = ls->lookupLabel("nextsq");
	now0Field = ls->lookupField("now0");
	now1Field = ls->lookupField("now1");
	now2Field = ls->lookupField("now2");
	seq1 = ls->lookupLabel("1");
	seq2 = ls->lookupLabel("2");
	seq3 = ls->lookupLabel("3");
	seq4 = ls->lookupLabel("4");
	seq5 = ls->lookupLabel("5");
	seq6 = ls->lookupLabel("6");
	seq7 = ls->lookupLabel("7");
	seq8 = ls->lookupLabel("8");
	seq9 = ls->lookupLabel("9");
	seq10 = ls->lookupLabel("10");
	seq11 = ls->lookupLabel("11");
	seq12 = ls->lookupLabel("12");
	seq13 = ls->lookupLabel("13");
	seq14 = ls->lookupLabel("14");
	seq15 = ls->lookupLabel("15");
	seq16 = ls->lookupLabel("16");
	seqNumbers = ls->lookupLabel("seqnumbers");
	bank = ls->lookupLabel("bank");
	seqs[0] = seq1;
	seqs[1] = seq2;
	seqs[2] = seq3;
	seqs[3] = seq4;
	seqs[4] = seq5;
	seqs[5] = seq6;
	seqs[6] = seq7;
	seqs[7] = seq8;
	seqs[8] = seq9;
	seqs[9] = seq10;
	seqs[10] = seq11;
	seqs[11] = seq12;
	seqs[12] = seq13;
	seqs[13] = seq14;
	seqs[14] = seq15;
	seqs[15] = seq16;
	for (int i = 0; i < 16; i++) {
		displaySeq(i);
		setOpaque(i);
		setSeqColor(i);
	}
	displaySq();
	displayNow0();
	displayNow1();
	displayNow2();
	displayBank();
	displaySeqNumbers();
	displayNextSq();
}

void NextSeqPadObserver::displayNextSq()
{
	auto lSequencer = sequencer.lock();
    nextSqLabel.lock()->setText(lSequencer->getNextSq() == -1 ? "" : moduru::lang::StrUtil::padLeft(to_string(lSequencer->getNextSq() + 1), "0", 2) + "-" + lSequencer->getSequence(lSequencer->getNextSq()).lock()->getName());
}

int NextSeqPadObserver::bankoffset()
{
    return samplerGui->getBank() * 16;
}

void NextSeqPadObserver::displayBank()
{
    auto letters = vector<string>{ "A", "B", "C", "D" };
    bank.lock()->setText(letters[samplerGui->getBank()]);
}

void NextSeqPadObserver::displaySeqNumbers()
{
    auto seqn = vector<string>{ "01-16", "17-32", "33-48", "49-64" };
    seqNumbers.lock()->setText(seqn[samplerGui->getBank()]);
}

void NextSeqPadObserver::displaySq()
{
	auto lSequencer = sequencer.lock();
    sqField.lock()->setText(moduru::lang::StrUtil::padLeft(to_string(lSequencer->getActiveSequenceIndex() + 1), "0", 2) + "-" + lSequencer->getActiveSequence().lock()->getName());
}

void NextSeqPadObserver::setOpaque(int i)
{
    seqs[i].lock()->setOpaque(true);
}

void NextSeqPadObserver::displaySeq(int i)
{
	auto lSequencer = sequencer.lock();
    seqs[i].lock()->setText(lSequencer->getSequence(i + bankoffset()).lock()->getName().substr(0, 8));
}

void NextSeqPadObserver::setSeqColor(int i)
{
	auto lSequencer = sequencer.lock();

	auto label = seqs[i].lock();
	if (i + bankoffset() == lSequencer->getNextSq()) {
		//label->setForeground(false);
		label->setInverted(true);
	}
	else {
		label->setInverted(false);
	}
}

void NextSeqPadObserver::update(moduru::observer::Observable* o, std::any arg)
{
	string s = std::any_cast<string>(arg);
	if (s.compare("soloenabled") == 0 || s.compare("bank") == 0) {
		displayBank();
		displaySeqNumbers();
		refreshSeqs();
	}
	else if (s.compare("seqnumbername") == 0) {
		displaySq();
		refreshSeqs();
	}
	else if (s.compare("nextsqoff") == 0) {
		nextSqLabel.lock()->setText("");
		refreshSeqs();
		displayNextSq();
	}
	else if (s.compare("nextsqvalue") == 0 || s.compare("nextsq") == 0) {
		refreshSeqs();
		displayNextSq();
	}
	else if (s.compare("now") == 0 || s.compare("clock") == 0) {
		displayNow0();
		displayNow1();
		displayNow2();
	}
}

void NextSeqPadObserver::displayNow0()
{
	auto lSequencer = sequencer.lock();
    now0Field.lock()->setTextPadded(lSequencer->getCurrentBarNumber() + 1, "0");
}

void NextSeqPadObserver::displayNow1()
{
	auto lSequencer = sequencer.lock();
	now1Field.lock()->setTextPadded(lSequencer->getCurrentBeatNumber() + 1, "0");
}

void NextSeqPadObserver::displayNow2()
{
	auto lSequencer = sequencer.lock();
	now2Field.lock()->setTextPadded(lSequencer->getCurrentClockNumber(), "0");
}

void NextSeqPadObserver::refreshSeqs()
{
	for (int i = 0; i < 16; i++) {
		displaySeq(i);
		setSeqColor(i);
	}
}

NextSeqPadObserver::~NextSeqPadObserver() {
	samplerGui->deleteObserver(this);
	sequencer.lock()->deleteObserver(this);
}
