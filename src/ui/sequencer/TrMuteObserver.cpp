#include <ui/sequencer/TrMuteObserver.hpp>
#include <Mpc.hpp>
#include <hardware/Hardware.hpp>
#include <hardware/Led.hpp>
#include <ui/Uis.hpp>
#include <lcdgui/Field.hpp>
#include <lcdgui/Label.hpp>
#include <ui/sampler/SamplerGui.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/Sequencer.hpp>

#include <lang/StrUtil.hpp>

using namespace mpc::ui::sequencer;
using namespace std;

TrMuteObserver::TrMuteObserver(mpc::Mpc* mpc)
{
	this->mpc = mpc;
	tracks = vector<weak_ptr<mpc::lcdgui::Label>>(16);
	sequencer = mpc->getSequencer();
	auto lSequencer = sequencer.lock();
	lSequencer->addObserver(this);
	samplerGui = mpc->getUis().lock()->getSamplerGui();
	samplerGui->addObserver(this);
	auto ls = mpc->getLayeredScreen().lock();
	sqField = ls->lookupField("sq");
	now0Field = ls->lookupField("now0");
	now1Field = ls->lookupField("now1");
	now2Field = ls->lookupField("now2");
	tr1 = ls->lookupLabel("1");
	tr2 = ls->lookupLabel("2");
	tr3 = ls->lookupLabel("3");
	tr4 = ls->lookupLabel("4");
	tr5 = ls->lookupLabel("5");
	tr6 = ls->lookupLabel("6");
	tr7 = ls->lookupLabel("7");
	tr8 = ls->lookupLabel("8");
	tr9 = ls->lookupLabel("9");
	tr10 = ls->lookupLabel("10");
	tr11 = ls->lookupLabel("11");
	tr12 = ls->lookupLabel("12");
	tr13 = ls->lookupLabel("13");
	tr14 = ls->lookupLabel("14");
	tr15 = ls->lookupLabel("15");
	tr16 = ls->lookupLabel("16");
	trackNumbers = ls->lookupLabel("tracknumbers");
	bank = ls->lookupLabel("bank");
	trackNumbers.lock()->setOpaque(false);
	bank.lock()->setOpaque(false);
	tracks[0] = tr1;
	tracks[1] = tr2;
	tracks[2] = tr3;
	tracks[3] = tr4;
	tracks[4] = tr5;
	tracks[5] = tr6;
	tracks[6] = tr7;
	tracks[7] = tr8;
	tracks[8] = tr9;
	tracks[9] = tr10;
	tracks[10] = tr11;
	tracks[11] = tr12;
	tracks[12] = tr13;
	tracks[13] = tr14;
	tracks[14] = tr15;
	tracks[15] = tr16;
	displayBank();
	displayTrackNumbers();

	auto s = lSequencer->getActiveSequence().lock();
	for (int i = 0; i < 64; i++) {
		s->getTrack(i).lock()->addObserver(this);
	}

	for (int i = 0; i < 16; i++) {
		displayTrack(i);
		setOpaque(i);
		setTrackColor(i);
	}
	displaySq();
	displayNow0();
	displayNow1();
	displayNow2();
}

int TrMuteObserver::bankoffset()
{
    return samplerGui->getBank() * 16;
}

void TrMuteObserver::displayBank()
{
    auto letters = vector<string>{ "A", "B", "C", "D" };
    bank.lock()->setText(letters[samplerGui->getBank()]);
}

void TrMuteObserver::displayTrackNumbers()
{
	auto trn = vector<string>{ "01-16", "17-32", "33-48", "49-64" };
	trackNumbers.lock()->setText(trn[samplerGui->getBank()]);
}

void TrMuteObserver::displaySq()
{
	auto lSequencer = sequencer.lock();
	sqField.lock()->setText(moduru::lang::StrUtil::padLeft(to_string(lSequencer->getActiveSequenceIndex() + 1), "0", 2) + "-" + lSequencer->getActiveSequence().lock()->getName());
}

void TrMuteObserver::setOpaque(int i)
{
    tracks[i].lock()->setOpaque(true);
}

void TrMuteObserver::displayTrack(int i)
{
    tracks[i].lock()->setText(sequencer.lock()->getActiveSequence().lock()->getTrack(i + bankoffset()).lock()->getName().substr(0, 8));
}

void TrMuteObserver::setTrackColor(int i)
{

	auto lSequencer = sequencer.lock();
	if (lSequencer->isSoloEnabled()) {
		if (i + bankoffset() == lSequencer->getActiveTrackIndex()) {
			tracks[i].lock()->setInverted(true);
		}
		else {
			tracks[i].lock()->setInverted(false);
		}
	}
	else {
		if (lSequencer->getActiveSequence().lock()->getTrack(i + bankoffset()).lock()->isOn()) {
			tracks[i].lock()->setInverted(true);
		}
		else {
			tracks[i].lock()->setInverted(false);
		}
	}
}

void TrMuteObserver::update(moduru::observer::Observable* o, std::any arg)
{
	string s = std::any_cast<string>(arg);
	if (s.compare("soloenabled") == 0 ) {
		for (auto& l : mpc->getLayeredScreen().lock()->getLayer(0)->getAllLabelsAndFields()) {
			l.lock()->SetDirty();
		}
		refreshTracks();
	}
	else if (s.compare("selectedtrackindex") == 0) {
		for (auto& l : mpc->getLayeredScreen().lock()->getLayer(0)->getAllLabelsAndFields()) {
			l.lock()->SetDirty();
		}
		refreshTracks();
	}
	else if (s.compare("bank") == 0) {
		displayBank();
		displayTrackNumbers();
		for (int i = 0; i < 16; i++) {
			setTrackColor(i);
		}
		refreshTracks();
	}
	else if (s.compare("seqnumbername") == 0) {
		displaySq();
		refreshTracks();
	}
	else if (s.compare("trackon") == 0) {
		for (int i = 0; i < 16; i++) {
			setTrackColor(i);
		}
	}
	else if (s.compare("now") == 0 || s.compare("clock") == 0) {
		displayNow0();
		displayNow1();
		displayNow2();
	}
}

void TrMuteObserver::displayNow0()
{
    now0Field.lock()->setTextPadded(sequencer.lock()->getCurrentBarNumber() + 1, "0");
}

void TrMuteObserver::displayNow1()
{
	now1Field.lock()->setTextPadded(sequencer.lock()->getCurrentBeatNumber() + 1, "0");
}

void TrMuteObserver::displayNow2()
{
	now2Field.lock()->setTextPadded(sequencer.lock()->getCurrentClockNumber(), "0");
}

void TrMuteObserver::refreshTracks()
{
	auto lSequencer = sequencer.lock();
	for (int i = 0; i < 16; i++) {
		displayTrack(i);
		setTrackColor(i);
		auto t = lSequencer->getActiveSequence().lock()->getTrack(i + bankoffset()).lock();
	}
}

TrMuteObserver::~TrMuteObserver() {
	auto lSeq = sequencer.lock();
	auto s = lSeq->getActiveSequence().lock();
	for (int i = 0; i < 64; i++) {
		s->getTrack(i).lock()->deleteObserver(this);
	}
	lSeq->deleteObserver(this);
	samplerGui->deleteObserver(this);
	mpc->getHardware().lock()->getLed("trackmute").lock()->light(false);
}
