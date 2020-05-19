#include <lcdgui/screens/TrMuteScreen.hpp>

#include <sequencer/Track.hpp>
#include <sequencer/Sequencer.hpp>

using namespace mpc::lcdgui::screens;
using namespace std;

TrMuteScreen::TrMuteScreen(const int& layer)
	: ScreenComponent("trmute", layer)
{
}

void TrMuteScreen::open()
{
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

void TrMuteScreen::right()
{
}

void TrMuteScreen::pad(int i, int velo, bool repeat, int tick)
{
	init();
	
	auto lLs = ls.lock();
	auto controls = Mpc::instance().getControls().lock();
	if (controls->isF6Pressed() || sequencer.lock()->isSoloEnabled()) {
		if (!sequencer.lock()->isSoloEnabled())
			sequencer.lock()->setSoloEnabled(true);

		sequencer.lock()->setSelectedTrackIndex(i + (bank_ * 16));
		lLs->removeCurrentBackground();
		lLs->setCurrentBackground("trackmutesolo2");
	}
	else {
		auto s = sequencer.lock()->getActiveSequence().lock();
		auto t = s->getTrack(i + (bank_ * 16)).lock();
		t->setOn(!t->isOn());
	}
}

void TrMuteScreen::turnWheel(int i)
{
	init();
	
	if (param.compare("sq") == 0) sequencer.lock()->setActiveSequenceIndex(sequencer.lock()->getActiveSequenceIndex() + i);
}

void TrMuteScreen::function(int i)
{
	init();
	super::function(i);
	
	auto lLs = ls.lock();
	switch (i) {
	case 5:
		if (sequencer.lock()->isSoloEnabled()) {
			lLs->setCurrentBackground("trackmute");
			sequencer.lock()->setSoloEnabled(false);
		}
		else {
			lLs->setCurrentBackground("trackmutesolo1");
			sequencer.lock()->setSoloEnabled(sequencer.lock()->isSoloEnabled());
		}
		break;
	}
}

int TrMuteScreen::bankoffset()
{
	return samplerGui->getBank() * 16;
}

void TrMuteScreen::displayBank()
{
	auto letters = vector<string>{ "A", "B", "C", "D" };
	bank.lock()->setText(letters[samplerGui->getBank()]);
}

void TrMuteScreen::displayTrackNumbers()
{
	auto trn = vector<string>{ "01-16", "17-32", "33-48", "49-64" };
	trackNumbers.lock()->setText(trn[samplerGui->getBank()]);
}

void TrMuteScreen::displaySq()
{
	auto lSequencer = sequencer.lock();
	sqField.lock()->setText(moduru::lang::StrUtil::padLeft(to_string(lSequencer->getActiveSequenceIndex() + 1), "0", 2) + "-" + lSequencer->getActiveSequence().lock()->getName());
}

void TrMuteScreen::setOpaque(int i)
{
	tracks[i].lock()->setOpaque(true);
}

void TrMuteScreen::displayTrack(int i)
{
	tracks[i].lock()->setText(sequencer.lock()->getActiveSequence().lock()->getTrack(i + bankoffset()).lock()->getName().substr(0, 8));
}

void TrMuteScreen::setTrackColor(int i)
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

void TrMuteScreen::displayNow0()
{
	findField("now0").lock()->setTextPadded(sequencer.lock()->getCurrentBarNumber() + 1, "0");
}

void TrMuteScreen::displayNow1()
{
	findField("now1").lock()->setTextPadded(sequencer.lock()->getCurrentBeatNumber() + 1, "0");
}

void TrMuteScreen::displayNow2()
{
	findField("now2").lock()->setTextPadded(sequencer.lock()->getCurrentClockNumber(), "0");
}

void TrMuteScreen::refreshTracks()
{
	auto lSequencer = sequencer.lock();
	for (int i = 0; i < 16; i++) {
		displayTrack(i);
		setTrackColor(i);
		auto t = lSequencer->getActiveSequence().lock()->getTrack(i + bankoffset()).lock();
	}
}
