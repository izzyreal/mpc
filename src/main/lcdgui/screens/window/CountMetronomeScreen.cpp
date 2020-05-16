#include "CountMetronomeScreen.hpp"

#include <ui/sequencer/window/SequencerWindowGui.hpp>

#include <lcdgui/Label.hpp>

using namespace mpc::lcdgui::screens::window;

CountMetronomeScreen::CountMetronomeScreen(const int& layer)
	: ScreenComponent("countmetronome", layer)
{
}

void CountMetronomeScreen::open()
{
	auto swGui = mpc.getUis().lock()->getSequencerWindowGui();
	swGui->addObserver(this);

	countInField = findField("countin");
	inPlayField = findField("inplay");
	rateField = findField("rate");
	inRecField = findField("inrec");
	waitForKeyField = findField("waitforkey");

	displayCountIn();
	displayInPlay();
	displayRate();
	displayInRec();
	displayWaitForKey();
}

void CountMetronomeScreen::close()
{
	auto swGui = mpc.getUis().lock()->getSequencerWindowGui();
	swGui->deleteObserver(this);
}

void CountMetronomeScreen::displayWaitForKey()
{
	auto swGui = mpc.getUis().lock()->getSequencerWindowGui();
	waitForKeyField.lock()->setText(swGui->isWaitForKeyEnabled() ? "ON" : "OFF");
}

void CountMetronomeScreen::displayInRec()
{
	auto swGui = mpc.getUis().lock()->getSequencerWindowGui();
	inRecField.lock()->setText(swGui->getInRec() ? "YES" : "NO");
}

void CountMetronomeScreen::displayRate()
{
	auto swGui = mpc.getUis().lock()->getSequencerWindowGui();
	rateField.lock()->setText(rateNames[swGui->getRate()]);
}

void CountMetronomeScreen::displayInPlay()
{
	auto swGui = mpc.getUis().lock()->getSequencerWindowGui();
	inPlayField.lock()->setText(swGui->getInPlay() ? "YES" : "NO");
}

void CountMetronomeScreen::displayCountIn()
{
	auto swGui = mpc.getUis().lock()->getSequencerWindowGui();
	countInField.lock()->setText(countInNames[swGui->getCountInMode()]);
}

void CountMetronomeScreen::update(moduru::observer::Observable* observable, nonstd::any message)
{
	auto msg = nonstd::any_cast<string>(message);

	if (msg.compare("countin") == 0)
	{
		displayCountIn();
	}
	else if (msg.compare("inplay") == 0)
	{
		displayInPlay();
	}
	else if (msg.compare("rate") == 0)
	{
		displayRate();
	}
	else if (msg.compare("inrec") == 0)
	{
		displayInRec();
	}
	else if (msg.compare("waitforkey") == 0)
	{
		displayWaitForKey();
	}
}


void CountMetronomeScreen::function(int i)
{
	auto swGui = mpc.getUis().lock()->getSequencerWindowGui();
	switch (i) {
	case 3:
		ls.lock()->openScreen("sequencer");
		break;
	case 4:
		ls.lock()->openScreen("metronomesound");
		break;
	}
}

void CountMetronomeScreen::turnWheel(int i)
{
	init();
	auto swGui = mpc.getUis().lock()->getSequencerWindowGui();
	if (param.compare("countin") == 0) {
		swGui->setCountIn(swGui->getCountInMode() + i);
	}
	else if (param.compare("inplay") == 0) {
		swGui->setInPlay(i > 0);
	}
	else if (param.compare("rate") == 0) {
		swGui->setRate(swGui->getRate() + i);
		sequencer.lock()->getActiveSequence().lock()->initMetaTracks();
	}
	else if (param.compare("inrec") == 0) {
		swGui->setInRec(i > 0);
	}
	else if (param.compare("waitforkey") == 0) {
		swGui->setWaitForKey(i > 0);
	}
}
