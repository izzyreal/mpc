#include "TrMuteScreen.hpp"

#include "sequencer/Track.hpp"

using namespace moduru::lang;
using namespace mpc::lcdgui::screens;

TrMuteScreen::TrMuteScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "track-mute", layerIndex)
{
}

void TrMuteScreen::open()
{
	if (sequencer->isSoloEnabled()) {
		findBackground().lock()->setName("track-mute-solo-2");
	}
	else {
		findBackground().lock()->setName("track-mute");
	}

	for (int i = 0; i < 16; i++)
    {
        auto trackField = findField(std::to_string(i + 1)).lock();
        trackField->setSize(49, 9);
        trackField->setFocusable(false);
    }

	displayBank();
	displayTrackNumbers();

	sequencer->addObserver(this);

	auto sequence = sequencer->getActiveSequence();
	
	for (int i = 0; i < 64; i++)
		sequence->getTrack(i)->addObserver(this);

	for (int i = 0; i < 16; i++)
	{
		displayTrack(i);
		setTrackColor(i);
	}

	displaySq();
	displayNow0();
	displayNow1();
	displayNow2();

    mpc.addObserver(this);
}

void TrMuteScreen::close()
{
    mpc.deleteObserver(this);
	sequencer->deleteObserver(this);
	auto sequence = sequencer->getActiveSequence();
	
	for (int i = 0; i < 64; i++)
		sequence->getTrack(i)->deleteObserver(this);
}

void TrMuteScreen::right()
{
	// Stop right from propgating to BaseController
}

void TrMuteScreen::pad(int padIndexWithBank, int velo)
{
	init();
	
	auto controls = mpc.getControls().lock();

	if (controls->isF6Pressed() || sequencer->isSoloEnabled())
	{
		if (!sequencer->isSoloEnabled())
			sequencer->setSoloEnabled(true);

		sequencer->setActiveTrackIndex(padIndexWithBank);
		ls.lock()->setCurrentBackground("track-mute-solo-2");
	}
	else
	{
		auto s = sequencer->getActiveSequence();
		auto t = s->getTrack(padIndexWithBank);
		t->setOn(!t->isOn());
	}
}

void TrMuteScreen::turnWheel(int i)
{
	init();
	
	if (param == "sq")
	{
		auto oldSequence = sequencer->getActiveSequence();

		for (int trackIndex = 0; trackIndex < 64; trackIndex++)
			oldSequence->getTrack(trackIndex)->deleteObserver(this);

		sequencer->setActiveSequenceIndex(sequencer->getActiveSequenceIndex() + i);
		auto newSequence = sequencer->getActiveSequence();
		
		for (int trackIndex = 0; trackIndex < 64; trackIndex++)
			newSequence->getTrack(trackIndex)->addObserver(this);

		displaySq();
		refreshTracks();
	}
}

void TrMuteScreen::function(int i)
{
	init();
	ScreenComponent::function(i);
	
	switch (i)
	{
	case 5:
		auto controls = mpc.getControls().lock();
		
		if (controls->isF6Pressed())
			return;

		controls->setF6Pressed(true);

		if (sequencer->isSoloEnabled())
		{
			ls.lock()->setCurrentBackground("track-mute");
			sequencer->setSoloEnabled(false);
		}
		else
		{
			ls.lock()->setCurrentBackground("track-mute-solo-1");
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
	std::vector<std::string> letters{ "A", "B", "C", "D" };
	findLabel("bank").lock()->setText(letters[mpc.getBank()]);
}

void TrMuteScreen::displayTrackNumbers()
{
	std::vector<std::string> trn{ "01-16", "17-32", "33-48", "49-64" };
	findLabel("tracknumbers").lock()->setText(trn[mpc.getBank()]);
}

void TrMuteScreen::displaySq()
{
	auto sequenceNumber = StrUtil::padLeft(std::to_string(sequencer->getActiveSequenceIndex() + 1), "0", 2);
	auto sequenceName = sequencer->getActiveSequence()->getName();
	findField("sq").lock()->setText(sequenceNumber + "-" + sequenceName);
}

void TrMuteScreen::displayTrack(int i)
{
	findField(std::to_string(i + 1)).lock()->setText(sequencer->getActiveSequence()->getTrack(i + bankoffset())->getName().substr(0, 8));
}

void TrMuteScreen::setTrackColor(int i)
{	
	if (sequencer->isSoloEnabled())
	{
		findField(std::to_string(i + 1)).lock()->setInverted(i + bankoffset() == sequencer->getActiveTrackIndex());
	}
	else
	{
		findField(std::to_string(i + 1)).lock()->setInverted(sequencer->getActiveSequence()->getTrack(i + bankoffset())->isOn());
	}
}

void TrMuteScreen::displayNow0()
{
	findField("now0").lock()->setTextPadded(sequencer->getCurrentBarIndex() + 1, "0");
}

void TrMuteScreen::displayNow1()
{
	findField("now1").lock()->setTextPadded(sequencer->getCurrentBeatIndex() + 1, "0");
}

void TrMuteScreen::displayNow2()
{
	findField("now2").lock()->setTextPadded(sequencer->getCurrentClockNumber(), "0");
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
	auto msg = nonstd::any_cast<std::string>(message);

	if (msg == "soloenabled")
	{
		refreshTracks();
	}
	else if (msg == "active-track-index")
	{
		refreshTracks();
	}
	else if (msg == "bank")
	{
		displayBank();
		displayTrackNumbers();

		for (int i = 0; i < 16; i++)
			setTrackColor(i);

		refreshTracks();
	}
	else if (msg == "seqnumbername")
	{
		displaySq();
		refreshTracks();
	}
	else if (msg == "trackon")
	{
		for (int i = 0; i < 16; i++)
			setTrackColor(i);
	}
	else if (msg == "now" || msg == "clock")
	{
		displayNow0();
		displayNow1();
		displayNow2();
	}
}
