#include "ChangeBars2Screen.hpp"

#include <lcdgui/Label.hpp>
#include <lcdgui/LayeredScreen.hpp>
#include <sequencer/Sequence.hpp>
#include <sequencer/Sequencer.hpp>

#include <lang/StrUtil.hpp>

using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace std;

ChangeBars2Screen::ChangeBars2Screen(const int layerIndex)
	: ScreenComponent("change-bars-2", layerIndex)
{
}

void ChangeBars2Screen::open()
{
	setNewBars(sequencer.lock()->getActiveSequence().lock()->getLastBarIndex());
	displayCurrent();
	displayNewBars();
}

void ChangeBars2Screen::displayCurrent()
{
	auto seq = sequencer.lock()->getActiveSequence().lock();
	findLabel("current").lock()->setText(to_string(seq->getLastBarIndex() + 1));
}

void ChangeBars2Screen::function(int i)
{
	BaseControls::function(i);
	auto seq = sequencer.lock()->getActiveSequence().lock();

	switch (i)
	{
	case 2:
		ls.lock()->openScreen("sequencer");
		ls.lock()->openScreen("change-bars");
		break;
	case 4:
		if (newBars < seq->getLastBarIndex())
		{
			seq->deleteBars(newBars + 1, seq->getLastBarIndex());
		}

		if (newBars > seq->getLastBarIndex())
		{
			seq->insertBars(newBars - seq->getLastBarIndex(), seq->getLastBarIndex() + 1);
		}
		ls.lock()->openScreen("sequencer");
		sequencer.lock()->setBar(0);
		break;
	}
}

void ChangeBars2Screen::displayNewBars()
{
	auto seq = sequencer.lock()->getActiveSequence().lock();

	auto message0 = findLabel("message0").lock();
	auto message1 = findLabel("message1").lock();

	findField("newbars").lock()->setText(moduru::lang::StrUtil::padLeft(to_string(newBars + 1), " ", 3));


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

	if (param.compare("newbars") == 0)
	{
		setNewBars(newBars + i);
	}
}

void ChangeBars2Screen::setNewBars(int i)
{
	if (i < 0 || i > 998)
	{
		return;
	}
	newBars = i;
	displayNewBars();
}
