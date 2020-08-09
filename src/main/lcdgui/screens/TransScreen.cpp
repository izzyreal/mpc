#include "TransScreen.hpp"

#include <sequencer/Track.hpp>

using namespace mpc::lcdgui::screens;
using namespace std;

TransScreen::TransScreen(mpc::Mpc& mpc, const int layerIndex) 
	: ScreenComponent(mpc, "trans", layerIndex)
{
}

void TransScreen::open()
{
	setBar0(0);
	setBar1(sequencer.lock()->getActiveSequence().lock()->getLastBarIndex());

	displayTransposeAmount();
	displayTr();
}

void TransScreen::function(int i)
{
	init();

	switch (i)
	{
	case 0:
		ls.lock()->openScreen("punch");
		break;
	case 2:
		ls.lock()->openScreen("second-seq");
		break;
	case 5:
		//if (amount == 0) break; // does 2kxl do that?
		ls.lock()->openScreen("transpose-permanent");
		break;
	}
}

void TransScreen::turnWheel(int i)
{
	init();
	
	if (param.compare("tr") == 0) {
		setTr(tr + i);
	}
	else if (param.compare("transpose-amount") == 0)
	{
		setTransposeAmount(transposeAmount + i);
	}
	else if (param.compare("bar0") == 0)
	{
		auto candidate = bar0 + i;
		
		if (candidate < 0 || candidate > sequencer.lock()->getActiveSequence().lock()->getLastBarIndex())
		{
			return;
		}

		setBar0(candidate);
	}
	else if (param.compare("bar1") == 0)
	{
		auto candidate = bar1 + i;
	
		if (candidate < 0 || candidate > sequencer.lock()->getActiveSequence().lock()->getLastBarIndex())
		{
			return;
		}

		setBar1(candidate);
	}
}

void TransScreen::setTransposeAmount(int i)
{
	if (i < -12 || i > 12)
	{
		return;
	}
	
	transposeAmount = i;
	
	displayTransposeAmount();
}

void TransScreen::setTr(int i)
{
	if (i < -1 || i > 63)
	{
		return;
	}
	
	tr = i;
	displayTr();
}

void TransScreen::setBar0(int i)
{
	if (i < 0)
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

void TransScreen::setBar1(int i)
{
	if (i < 0)
	{
		return;
	}

	bar1 = i;
	
	if (bar1 < bar0)
	{
		bar0 = bar1;
	}
	
	displayBars();
}

void TransScreen::displayTransposeAmount()
{
	findField("transpose-amount").lock()->setTextPadded(transposeAmount);
}

void TransScreen::displayTr()
{
	auto trName = string(tr == -1 ? "ALL" : sequencer.lock()->getActiveSequence().lock()->getTrack(tr).lock()->getName());
	findField("tr").lock()->setTextPadded(tr + 1, "0");
	findLabel("track-name").lock()->setText(trName);
}

void TransScreen::displayBars()
{
	findField("bar0").lock()->setTextPadded(to_string(bar0 + 1), "0");
	findField("bar1").lock()->setTextPadded(to_string(bar1 + 1), "0");
}
