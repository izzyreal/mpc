#include "TrMuteScreen.hpp"

#include "controls/Controls.hpp"
#include "hardware2/Hardware2.h"
#include "sequencer/Track.hpp"

using namespace mpc::lcdgui::screens;

TrMuteScreen::TrMuteScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "track-mute", layerIndex)
{
}

void TrMuteScreen::open()
{
	if (sequencer.lock()->isSoloEnabled()) {
		findBackground()->setName("track-mute-solo-2");
	}
	else {
		findBackground()->setName("track-mute");
	}

	for (int i = 0; i < 16; i++)
    {
        auto trackField = findField(std::to_string(i + 1));
        trackField->setSize(49, 9);
        trackField->setFocusable(false);
    }

	displayBank();
	displayTrackNumbers();

	sequencer.lock()->addObserver(this);

	auto sequence = sequencer.lock()->getActiveSequence();
	
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
	sequencer.lock()->deleteObserver(this);
	auto sequence = sequencer.lock()->getActiveSequence();
	
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
	
	auto controls = mpc.getControls();

	if (mpc.getHardware2()->getButton("f6")->isPressed() || sequencer.lock()->isSoloEnabled())
	{
		if (!sequencer.lock()->isSoloEnabled())
			sequencer.lock()->setSoloEnabled(true);

		sequencer.lock()->setActiveTrackIndex(padIndexWithBank);
		ls->setCurrentBackground("track-mute-solo-2");
	}
	else
	{
		auto s = sequencer.lock()->getActiveSequence();
		auto t = s->getTrack(padIndexWithBank);
		t->setOn(!t->isOn());
	}
}

void TrMuteScreen::turnWheel(int i)
{
	init();
	
	if (param == "sq")
	{
		auto oldSequence = sequencer.lock()->getActiveSequence();

		for (int trackIndex = 0; trackIndex < 64; trackIndex++)
			oldSequence->getTrack(trackIndex)->deleteObserver(this);

		sequencer.lock()->setActiveSequenceIndex(sequencer.lock()->getActiveSequenceIndex() + i);
		auto newSequence = sequencer.lock()->getActiveSequence();
		
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
		auto controls = mpc.getControls();
		
		if (sequencer.lock()->isSoloEnabled())
		{
			ls->setCurrentBackground("track-mute");
			sequencer.lock()->setSoloEnabled(false);
		}
		else
		{
			ls->setCurrentBackground("track-mute-solo-1");
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
	findLabel("bank")->setText(letters[mpc.getBank()]);
}

void TrMuteScreen::displayTrackNumbers()
{
	std::vector<std::string> trn{ "01-16", "17-32", "33-48", "49-64" };
	findLabel("tracknumbers")->setText(trn[mpc.getBank()]);
}

void TrMuteScreen::displaySq()
{
	auto sequenceNumber = StrUtil::padLeft(std::to_string(sequencer.lock()->getActiveSequenceIndex() + 1), "0", 2);
	auto sequenceName = sequencer.lock()->getActiveSequence()->getName();
	findField("sq")->setText(sequenceNumber + "-" + sequenceName);
}

void TrMuteScreen::displayTrack(int i)
{
	findField(std::to_string(i + 1))->setText(sequencer.lock()->getActiveSequence()->getTrack(i + bankoffset())->getName().substr(0, 8));
}

void TrMuteScreen::setTrackColor(int i)
{	
	if (sequencer.lock()->isSoloEnabled())
	{
		findField(std::to_string(i + 1))->setInverted(i + bankoffset() == sequencer.lock()->getActiveTrackIndex());
	}
	else
	{
		findField(std::to_string(i + 1))->setInverted(sequencer.lock()->getActiveSequence()->getTrack(i + bankoffset())->isOn());
	}
}

void TrMuteScreen::displayNow0()
{
	findField("now0")->setTextPadded(sequencer.lock()->getCurrentBarIndex() + 1, "0");
}

void TrMuteScreen::displayNow1()
{
	findField("now1")->setTextPadded(sequencer.lock()->getCurrentBeatIndex() + 1, "0");
}

void TrMuteScreen::displayNow2()
{
	findField("now2")->setTextPadded(sequencer.lock()->getCurrentClockNumber(), "0");
}

void TrMuteScreen::refreshTracks()
{
	for (int i = 0; i < 16; i++)
	{
		displayTrack(i);
		setTrackColor(i);
	}
}

void TrMuteScreen::update(Observable* o, Message message)
{
	const auto msg = std::get<std::string>(message);

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
