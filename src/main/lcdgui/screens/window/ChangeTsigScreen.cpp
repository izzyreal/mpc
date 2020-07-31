#include "ChangeTsigScreen.hpp"

#include <lcdgui/LayeredScreen.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/TimeSignature.hpp>

#include <Util.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace std;

ChangeTsigScreen::ChangeTsigScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "change-tsig", layerIndex)
{
}

void ChangeTsigScreen::open()
{
	timesignature = sequencer.lock()->getActiveSequence().lock()->getTimeSignature();
	displayBars();
	displayNewTsig();
}

void ChangeTsigScreen::function(int i)
{
    baseControls->function(i);

	switch (i)
	{
	case 4:
		auto sequence = sequencer.lock()->getActiveSequence().lock();
        sequence->setTimeSignature(bar0, bar1, timesignature.getNumerator(), timesignature.getDenominator());
        ls.lock()->openScreen("sequencer");
        break;
    }
}

void ChangeTsigScreen::turnWheel(int i)
{
    init();

	auto seq = sequencer.lock()->getActiveSequence().lock();

	if (param.compare("bar0") == 0)
	{
		setBar0(bar0 + i, seq->getLastBarIndex());
		displayBars();
	}
	else if (param.compare("bar1") == 0)
	{
		setBar1(bar1 + i, seq->getLastBarIndex());
		displayBars();
	}
	else if (param.compare("newtsig") == 0 && i > 0)
	{
		timesignature.increase();
		displayNewTsig();
	}
	else if (param.compare("newtsig") == 0 && i < 0)
	{
		timesignature.decrease();
		displayNewTsig();
	}
}

void ChangeTsigScreen::displayBars()
{
	findField("bar0").lock()->setText(to_string(bar0 + 1));
	findField("bar1").lock()->setText(to_string(bar1 + 1));
}

void ChangeTsigScreen::displayNewTsig()
{
	if (ls.lock()->getCurrentScreenName().compare("delete-sequence") == 0) {
		return;
	}
	auto result = to_string(timesignature.getNumerator()) + "/" + to_string(timesignature.getDenominator());
	findField("newtsig").lock()->setText(mpc::Util::distributeTimeSig(result));
}

/*
mpc::sequencer::TimeSignature& ChangeTsigScreen::getNewTimeSignature()
{
	return newTimeSignature;
}
*/

void ChangeTsigScreen::setBar0(int i, int max)
{
	if (i < 0 || i > max)
	{
		return;
	}

	bar0 = i;
	
	if (bar0 > bar1)
	{
		bar1 = bar0;
	}
	displayBars();
}

void ChangeTsigScreen::setBar1(int i, int max)
{
	if (i < 0 || i > max)
	{
		return;
	}

	bar1 = i;
	displayBars();
}
