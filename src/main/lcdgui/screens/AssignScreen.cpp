#include "AssignScreen.hpp"

#include <sampler/PgmSlider.hpp>

using namespace mpc::lcdgui::screens;
using namespace std;

AssignScreen::AssignScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "assign", layerIndex)
{
}

void AssignScreen::open()
{
	displayAssignNote();
	displayParameter();
	displayHighRange();
	displayLowRange();
	displayAssignNv();
	
	init();
	auto slider = program.lock()->getSlider();
	slider->addObserver(this);
}

void AssignScreen::close()
{
	init();
	auto slider = program.lock()->getSlider();
	slider->deleteObserver(this);
}

void AssignScreen::turnWheel(int i)
{
	init();
	auto slider = program.lock()->getSlider();
	auto parameter = slider->getParameter();

	if (param.compare("assignnote") == 0)
	{
		slider->setAssignNote(slider->getNote() + i);
	}
	else if (param.compare("parameter") == 0)
	{
		slider->setParameter(slider->getParameter() + i);
	}
	else if (param.compare("highrange") == 0)
	{
		switch (parameter)
		{
		case 0:
			slider->setTuneHighRange(slider->getTuneHighRange() + i);
			break;
		case 1:
			slider->setDecayHighRange(slider->getDecayHighRange() + i);
			break;
		case 2:
			slider->setAttackHighRange(slider->getAttackHighRange() + i);
			break;
		case 3:
			slider->setFilterHighRange(slider->getFilterHighRange() + i);
			break;
		}
	}
	else if (param.compare("lowrange") == 0)
	{
		switch (parameter)
		{
		case 0:
			slider->setTuneLowRange(slider->getTuneLowRange() + i);
			break;
		case 1:
			slider->setDecayLowRange(slider->getDecayLowRange() + i);
			break;
		case 2:
			slider->setAttackLowRange(slider->getAttackLowRange() + i);
			break;
		case 3:
			slider->setFilterLowRange(slider->getFilterLowRange() + i);
			break;
		}

	}
	else if (param.compare("assignnv") == 0)
	{
		slider->setControlChange(slider->getControlChange() + i);
	}
}

void AssignScreen::pad(int i, int velo, bool repeat, int tick)
{
	baseControls->pad(i, velo, repeat, 0);
	auto nn = program.lock()->getNoteFromPad(i + (mpc.getBank() * 16));
    program.lock()->getSlider()->setAssignNote(nn);
}

void AssignScreen::displayAssignNote()
{
	init();
	auto slider = program.lock()->getSlider();
	auto nn = slider->getNote();

	if (nn == 34)
	{
		findField("assignnote").lock()->setText("OFF");
		return;
	}

	auto lProgram = program.lock();
	auto lSampler = sampler.lock();
	string padName = lSampler->getPadName(lProgram->getPadIndexFromNote(nn));
	auto sn = lProgram->getNoteParameters(nn)->getSndNumber();
	auto soundName = sn == -1 ? "OFF" : lSampler->getSoundName(sn);
	findField("assignnote").lock()->setText(to_string(nn) + "/" + padName + "-" + soundName);
}

void AssignScreen::displayParameter()
{
	init();
	auto slider = program.lock()->getSlider();
	findField("parameter").lock()->setText(typeNames[slider->getParameter()]);
}

void AssignScreen::displayHighRange()
{
	int hr = 0;
	init();
	auto slider = program.lock()->getSlider();

	switch (slider->getParameter())
	{
	case 0:
		hr = slider->getTuneHighRange();
		break;
	case 1:
		hr = slider->getDecayHighRange();
		break;
	case 2:
		hr = slider->getAttackHighRange();
		break;
	case 3:
		hr = slider->getFilterHighRange();
		break;
	}
	findField("highrange").lock()->setTextPadded(hr, " ");
}

void AssignScreen::displayLowRange()
{
	auto lr = 0;

	init();
	auto slider = program.lock()->getSlider();

	switch (slider->getParameter())
	{
	case 0:
		lr = slider->getTuneLowRange();
		break;
	case 1:
		lr = slider->getDecayLowRange();
		break;
	case 2:
		lr = slider->getAttackLowRange();
		break;
	case 3:
		lr = slider->getFilterLowRange();
		break;
	}

	findField("lowrange").lock()->setTextPadded(lr, " ");
}

void AssignScreen::displayAssignNv()
{
	init();
	auto slider = program.lock()->getSlider();
	auto assignNvString = slider->getControlChange() == 0 ? "OFF" : to_string(slider->getControlChange());
	findField("assignnv").lock()->setTextPadded(assignNvString, " ");
}

void AssignScreen::update(moduru::observer::Observable* observable, nonstd::any message)
{
	string msg = nonstd::any_cast<string>(message);
	
	if (msg.compare("assignnote") == 0)
	{
		displayAssignNote();
	}
	else if (msg.compare("parameter") == 0)
	{
		displayParameter();
		displayHighRange();
		displayLowRange();
	}
	else if (msg.compare("highrange") == 0)
	{
		displayHighRange();
	}
	else if (msg.compare("lowrange") == 0)
	{
		displayLowRange();
	}
	else if (msg.compare("controlchange") == 0)
	{
		displayAssignNv();
	}
}
