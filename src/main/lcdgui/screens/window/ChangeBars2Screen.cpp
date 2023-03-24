#include "ChangeBars2Screen.hpp"

#include <sequencer/Sequence.hpp>

#include <lang/StrUtil.hpp>

using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace moduru::lang;

ChangeBars2Screen::ChangeBars2Screen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "change-bars-2", layerIndex)
{
}

void ChangeBars2Screen::open()
{
	setNewBars(sequencer->getActiveSequence()->getLastBarIndex());
	displayCurrent();
	displayNewBars();
}

void ChangeBars2Screen::displayCurrent()
{
	auto seq = sequencer->getActiveSequence();
	findLabel("current")->setText(std::to_string(seq->getLastBarIndex() + 1));
}

void ChangeBars2Screen::function(int i)
{
	ScreenComponent::function(i);
	auto seq = sequencer->getActiveSequence();

	switch (i)
	{
	case 2:
		openScreen("sequencer"); // Required for desired screen transitions
		openScreen("change-bars");
		break;
	case 4:
	{
		auto lastBarIndex = seq->getLastBarIndex();
		auto changed = false;

		if (newBars < lastBarIndex)
		{
			seq->deleteBars(newBars + 1, lastBarIndex);
			lastBarIndex = seq->getLastBarIndex();
			changed = true;
		}

		if (newBars > lastBarIndex)
		{
			seq->insertBars(newBars - lastBarIndex, lastBarIndex + 1);
			changed = true;
		}

		if (changed)
			sequencer->move(0);

		openScreen("sequencer");
		break;
	}
	}
}

void ChangeBars2Screen::displayNewBars()
{
	auto seq = sequencer->getActiveSequence();

	auto message0 = findLabel("message0");
	auto message1 = findLabel("message1");

	findField("newbars")->setText(StrUtil::padLeft(std::to_string(newBars + 1), " ", 3));

	if (newBars == seq->getLastBarIndex())
	{
		message0->setText("");
		message1->setText("");
	}
	else if (newBars > seq->getLastBarIndex())
	{
		message0->setText("Pressing DO IT will add");
		message1->setText("blank bars after last bar.");
	}
	else if (newBars < seq->getLastBarIndex())
	{
		message0->setText("Pressing DO IT will truncate");
		message1->setText("bars after last bar.");
	}
}

void ChangeBars2Screen::turnWheel(int i)
{
	init();

	if (param == "newbars")
		setNewBars(newBars + i);
}

void ChangeBars2Screen::setNewBars(int i)
{
	if (i < 0 || i > 998)
		return;

	newBars = i;
	displayNewBars();
}
