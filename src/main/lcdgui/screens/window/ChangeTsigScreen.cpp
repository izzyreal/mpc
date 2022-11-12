#include "ChangeTsigScreen.hpp"

#include <sequencer/Sequence.hpp>

#include <lang/StrUtil.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace moduru::lang;

ChangeTsigScreen::ChangeTsigScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "change-tsig", layerIndex)
{
}

void ChangeTsigScreen::open()
{
	timesignature = sequencer->getActiveSequence()->getTimeSignature();

	bar0 = 0;
	bar1 = sequencer->getActiveSequence()->getLastBarIndex();

	displayBars();
	displayNewTsig();
}

void ChangeTsigScreen::function(int i)
{
    ScreenComponent::function(i);

	switch (i)
	{
	case 4:
		auto sequence = sequencer->getActiveSequence();

		auto barLengths = sequence->getBarLengthsInTicks();

        sequence->setTimeSignature(bar0, bar1, timesignature.getNumerator(), timesignature.getDenominator());
		
		auto& newBarLengths = sequence->getBarLengthsInTicks();

		for (int j = 0; j < barLengths.size(); j++)
		{
			if (barLengths[j] != newBarLengths[j])
			{
				sequencer->move(0); // Only reset sequencer position when a bar length has changed
				break;
			}
		}

        openScreen("sequencer");
        break;
    }
}

void ChangeTsigScreen::turnWheel(int i)
{
    init();

	auto seq = sequencer->getActiveSequence();

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
	findField("bar0")->setTextPadded(bar0 + 1);
	findField("bar1")->setTextPadded(bar1 + 1);
}

void ChangeTsigScreen::displayNewTsig()
{
	if (ls.lock()->getCurrentScreenName().compare("delete-sequence") == 0)
		return;

	auto result = StrUtil::padLeft(std::to_string(timesignature.getNumerator()), " ", 2) +
		"/" + StrUtil::padLeft(std::to_string(timesignature.getDenominator()), " ", 2);
	findField("newtsig")->setText(result);
}

void ChangeTsigScreen::setBar0(int i, int max)
{
	if (i < 0 || i > max)
		return;

	bar0 = i;
	
	if (bar0 > bar1)
		bar1 = bar0;

	displayBars();
}

void ChangeTsigScreen::setBar1(int i, int max)
{
	if (i < 0 || i > max)
		return;

	bar1 = i;
	
	if (bar1 < bar0)
		bar0 = bar1;

	displayBars();
}
