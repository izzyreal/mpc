#include "TrMuteScreen.hpp"

#include <sequencer/Track.hpp>

using namespace moduru::lang;
using namespace mpc::lcdgui::screens;
using namespace std;

TrMuteScreen::TrMuteScreen(const int layerIndex)
	: ScreenComponent("trmute", layerIndex)
{
}

void TrMuteScreen::open()
{
	displayBank();
	displayTrackNumbers();

	sequencer.lock()->addObserver(this);

	auto sequence = sequencer.lock()->getActiveSequence().lock();
	for (int i = 0; i < 64; i++) {
		sequence->getTrack(i).lock()->addObserver(this);
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

void TrMuteScreen::close()
{
	sequencer.lock()->deleteObserver(this);
	auto sequence = sequencer.lock()->getActiveSequence().lock();
	for (int i = 0; i < 64; i++) {
		sequence->getTrack(i).lock()->deleteObserver(this);
	}
}

void TrMuteScreen::right()
{
}

void TrMuteScreen::pad(int i, int velo, bool repeat, int tick)
{
	init();
	
	auto lLs = ls.lock();
	auto controls = Mpc::instance().getControls().lock();

	if (controls->isF6Pressed() || sequencer.lock()->isSoloEnabled())
	{
		if (!sequencer.lock()->isSoloEnabled())
		{
			sequencer.lock()->setSoloEnabled(true);
		}

		sequencer.lock()->setSelectedTrackIndex(i + (mpc.getBank() * 16));
		ls.lock()->removeCurrentBackground();
		ls.lock()->setCurrentBackground("track-mute-solo-2");
	}
	else
	{
		auto s = sequencer.lock()->getActiveSequence().lock();
		auto t = s->getTrack(i + (mpc.getBank() * 16)).lock();
		t->setOn(!t->isOn());
	}
}

void TrMuteScreen::turnWheel(int i)
{
	init();
	
	if (param.compare("sq") == 0)
	{
		auto oldSequence = sequencer.lock()->getActiveSequence().lock();
		for (int i = 0; i < 64; i++)
		{
			oldSequence->getTrack(i).lock()->deleteObserver(this);
		}
		sequencer.lock()->setActiveSequenceIndex(sequencer.lock()->getActiveSequenceIndex() + i);
		auto newSequence = sequencer.lock()->getActiveSequence().lock();
		
		for (int i = 0; i < 64; i++)
		{
			newSequence->getTrack(i).lock()->addObserver(this);
		}
		displaySq();
		refreshTracks();
	}
}

void TrMuteScreen::function(int i)
{
	init();
	BaseControls::function(i);
	
	switch (i)
	{
	case 5:
		if (sequencer.lock()->isSoloEnabled())
		{
			ls.lock()->setCurrentBackground("track-mute");
			sequencer.lock()->setSoloEnabled(false);
		}
		else
		{
			ls.lock()->setCurrentBackground("track-mute-solo-1");
			sequencer.lock()->setSoloEnabled(sequencer.lock()->isSoloEnabled());
		}
		break;
	}
}

int TrMuteScreen::bankoffset()
{
	int bank = mpc.getBank();
	return bank * 16;
}

void TrMuteScreen::displayBank()
{
	auto letters = vector<string>{ "A", "B", "C", "D" };
	findLabel("bank").lock()->setText(letters[mpc.getBank()]);
}

void TrMuteScreen::displayTrackNumbers()
{
	auto trn = vector<string>{ "01-16", "17-32", "33-48", "49-64" };
	findLabel("tracknumbers").lock()->setText(trn[mpc.getBank()]);
}

void TrMuteScreen::displaySq()
{
	auto sequenceNumber = StrUtil::padLeft(to_string(sequencer.lock()->getActiveSequenceIndex() + 1), "0", 2);
	auto sequenceName = sequencer.lock()->getActiveSequence().lock()->getName();
	findField("sq").lock()->setText(sequenceNumber + "-" + sequenceName);
}

void TrMuteScreen::setOpaque(int i)
{
	findLabel(to_string(i + 1)).lock()->setOpaque(true);
}

void TrMuteScreen::displayTrack(int i)
{
	findLabel(to_string(i + 1)).lock()->setText(sequencer.lock()->getActiveSequence().lock()->getTrack(i + bankoffset()).lock()->getName().substr(0, 8));
}

void TrMuteScreen::setTrackColor(int i)
{	
	if (sequencer.lock()->isSoloEnabled())
	{
		findLabel(to_string(i + 1)).lock()->setInverted(i + bankoffset() == sequencer.lock()->getActiveTrackIndex());
	}
	else
	{
		findLabel(to_string(i + 1)).lock()->setInverted(sequencer.lock()->getActiveSequence().lock()->getTrack(i + bankoffset()).lock()->isOn());
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
	for (int i = 0; i < 16; i++)
	{
		displayTrack(i);
		setTrackColor(i);
	}
}

void TrMuteScreen::update(moduru::observer::Observable* o, nonstd::any message)
{
	string msg = nonstd::any_cast<string>(message);

	if (msg.compare("soloenabled") == 0)
	{
		refreshTracks();
	}
	else if (msg.compare("selectedtrackindex") == 0)
	{
		refreshTracks();
	}
	else if (msg.compare("bank") == 0)
	{
		displayBank();
		displayTrackNumbers();
		for (int i = 0; i < 16; i++)
		{
			setTrackColor(i);
		}
		refreshTracks();
	}
	else if (msg.compare("seqnumbername") == 0)
	{
		displaySq();
		refreshTracks();
	}
	else if (msg.compare("trackon") == 0)
	{
		for (int i = 0; i < 16; i++)
		{
			setTrackColor(i);
		}
	}
	else if (msg.compare("now") == 0 || msg.compare("clock") == 0)
	{
		displayNow0();
		displayNow1();
		displayNow2();
	}
}