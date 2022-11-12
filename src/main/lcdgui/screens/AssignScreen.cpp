#include "AssignScreen.hpp"

#include <sampler/PgmSlider.hpp>

#include <lang/StrUtil.hpp>

using namespace mpc::lcdgui::screens;
using namespace moduru::lang;

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

	if (param == "assignnote")
	{
		slider->setAssignNote(slider->getNote() + i);
	}
	else if (param == "parameter")
	{
		slider->setParameter(slider->getParameter() + i);
	}
	else if (param == "highrange")
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
	else if (param == "lowrange")
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
	else if (param == "assignnv")
	{
		slider->setControlChange(slider->getControlChange() + i);
	}
}

void AssignScreen::pad(int i, int velo)
{
	ScreenComponent::pad(i, velo);
	auto nn = program.lock()->getNoteFromPad(i + (mpc.getBank() * 16));
    program.lock()->getSlider()->setAssignNote(nn);
}

void AssignScreen::displayAssignNote()
{
	init();
	auto slider = program.lock()->getSlider();
	auto note = slider->getNote();

	auto padIndex = program.lock()->getPadIndexFromNote(note);
	auto padName = sampler->getPadName(padIndex);
	
	auto soundIndex = note == 34 ? -1 : program.lock()->getNoteParameters(note)->getSoundIndex();
	auto soundName = soundIndex == -1 ? "(No sound)" : sampler->getSoundName(soundIndex);
	
	auto noteName = note == 34 ? "--" : std::to_string(note);

	findField("assignnote")->setText(noteName + "/" + padName + "-" + soundName);
}

void AssignScreen::displayParameter()
{
	init();
	auto slider = program.lock()->getSlider();
	findField("parameter")->setText(typeNames[slider->getParameter()]);
}

void AssignScreen::displayHighRange()
{
	int value = 0;
	init();
	auto slider = program.lock()->getSlider();
	auto sign = "";

	findField("highrange")->setSize(19, 9);

	switch (slider->getParameter())
	{
	case 0:
		value = slider->getTuneHighRange();
		sign = value < 0 ? "-" : " ";
		findField("highrange")->setSize(25, 9);
		break;
	case 1:
		value = slider->getDecayHighRange();
		break;
	case 2:
		value = slider->getAttackHighRange();
		break;
	case 3:
		value = slider->getFilterHighRange();
		findField("highrange")->setSize(25, 9);
		sign = value < 0 ? "-" : " ";
		break;
	}

	findField("highrange")->setText(sign + StrUtil::padLeft(std::to_string(abs(value)), " ", 3));
}

void AssignScreen::displayLowRange()
{
	auto value = 0;

	init();
	auto slider = program.lock()->getSlider();
	auto sign = "";
	findField("lowrange")->setSize(19, 9);

	switch (slider->getParameter())
	{
	case 0:
		value = slider->getTuneLowRange();
		sign = value < 0 ? "-" : " ";
		findField("lowrange")->setSize(25, 9);
		break;
	case 1:
		value = slider->getDecayLowRange();
		break;
	case 2:
		value = slider->getAttackLowRange();
		break;
	case 3:
		value = slider->getFilterLowRange();
		sign = value < 0 ? "-" : " ";
		findField("lowrange")->setSize(25, 9);
		break;
	}

	findField("lowrange")->setText(sign + StrUtil::padLeft(std::to_string(abs(value)), " ", 3));
}

void AssignScreen::displayAssignNv()
{
	init();
	auto slider = program.lock()->getSlider();
	auto assignNvString = slider->getControlChange() == 0 ? "OFF" : std::to_string(slider->getControlChange());
	findField("assignnv")->setTextPadded(assignNvString, " ");
}

void AssignScreen::update(moduru::observer::Observable* observable, nonstd::any message)
{
	auto msg = nonstd::any_cast<std::string>(message);
	
	if (msg == "assignnote")
	{
		displayAssignNote();
	}
	else if (msg == "parameter")
	{
		displayParameter();
		displayHighRange();
		displayLowRange();
	}
	else if (msg == "highrange")
	{
		displayHighRange();
	}
	else if (msg == "lowrange")
	{
		displayLowRange();
	}
	else if (msg == "controlchange")
	{
		displayAssignNv();
	}
}
