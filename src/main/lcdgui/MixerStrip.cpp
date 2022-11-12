#include <lcdgui/MixerStrip.hpp>

#include <lcdgui/Knob.hpp>
#include <lcdgui/Label.hpp>
#include <lcdgui/MixerFaderBackground.hpp>
#include <lcdgui/MixerTopBackground.hpp>
#include <lcdgui/MixerFader.hpp>
#include <lcdgui/screens/MixerScreen.hpp>

#include <lang/StrUtil.hpp>

using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui;
using namespace moduru::lang;

MixerStrip::MixerStrip(mpc::Mpc& mpc, int columnIndex)
	: Component("mixer-strip"), mpc(mpc)
{
	this->columnIndex = columnIndex;

	yPos0indiv = 0;
	yPos1indiv = 4;
	yPos0fx = 2;
	yPos1fx = 2;
	selection = -1;

	auto x1 = 4 + (columnIndex * 15);
	addChild(std::move(std::make_shared<MixerTopBackground>(MRECT(x1, 0, x1 + 14, 13))));
	addChild(std::move(std::make_shared<MixerFaderBackground>(MRECT(x1, 14, x1 + 14, 50))));

	auto x2 = 5 + (columnIndex * 15);
	findChild("mixer-top-background")->addChild(std::move(std::make_shared<Knob>(MRECT(x2, 1, x2 + 12, 12))));

	for (int i = 0; i < 5; i++)
	{
		auto xPos = 5 + (columnIndex * 15);

		if (i == 1)
		{
			xPos += 6;
		}

		auto yPos = 2 + (i * 13);

		if (i >= 1)
		{
			yPos -= 13;
		}

		auto label = std::make_shared<Label>(mpc, std::to_string(i), "", xPos, yPos, 5);

		if (i < 2)
		{
			findMixerTopBackground()->addChild(std::move(label));
		}
		else
		{
			findMixerFaderBackground()->addChild(std::move(label));
		}
	}

	auto x3 = 12 + (columnIndex * 15);
	findMixerFaderBackground()->addChild(std::move(std::make_shared<MixerFader>(MRECT(x3, 15, x3 + 4, 49))));

	auto padName = StrUtil::padLeft(std::to_string(columnIndex + 1), "0", 2);
	findLabel("3")->setText(padName.substr(0, 1));
	findLabel("4")->setText(padName.substr(1, 2));
}

void MixerStrip::setBank(int i)
{
	findLabel("2")->setText(abcd[i]);
	findMixerFaderBackground()->SetDirty();
}

void MixerStrip::setValueA(int i)
{
    findKnob()->setValue(i);
	findMixerTopBackground()->SetDirty();
}

void MixerStrip::setValueB(int i)
{
    findMixerFader()->setValue(i);
	findMixerFaderBackground()->SetDirty();
}

void MixerStrip::initLabels()
{
	auto mixerScreen = mpc.screens->get<MixerScreen>("mixer");

	if (mixerScreen->getTab() == 0)
	{
		findKnob()->Hide(false);
		findLabel("0")->Hide(true);
		findLabel("1")->Hide(true);
	}
	else if (mixerScreen->getTab() == 1)
	{
		findKnob()->Hide(true);
		findLabel("0")->Hide(false);
		findLabel("1")->Hide(false);
		findLabel("0")->setLocation(xPos0indiv[columnIndex] - 1, yPos0indiv);
		findLabel("1")->setLocation(xPos1indiv[columnIndex] - 1, yPos1indiv);
	}
	else if (mixerScreen->getTab() == 2)
	{
		findKnob()->Hide(true);
		findLabel("0")->Hide(false);
		findLabel("1")->Hide(false);
		findLabel("0")->setLocation(xPos0fx[columnIndex], yPos0fx);
		findLabel("1")->setLocation(xPos1fx[columnIndex], yPos1fx);
	}
	SetDirty();
}

void MixerStrip::setColors()
{
	if (selection == -1)
	{
		for (int i = 0; i < 5; i++)
		{
			findLabel(std::to_string(i))->setInverted(false);
		}
		
		findMixerTopBackground()->setColor(false);
		findMixerFaderBackground()->setColor(false);
		findKnob()->setColor(true);
		findMixerFader()->setColor(true);
	}
	else if (selection == 0)
	{
		findLabel("0")->setInverted(true);
		findLabel("1")->setInverted(true);
		findLabel("2")->setInverted(false);
		findLabel("3")->setInverted(false);
		findLabel("4")->setInverted(false);
		findMixerTopBackground()->setColor(true);
		findMixerFaderBackground()->setColor(false);
		findKnob()->setColor(false);
		findMixerFader()->setColor(true);
	}
    else if (selection == 1)
	{
        findLabel("0")->setInverted(false);
        findLabel("1")->setInverted(false);
        findLabel("2")->setInverted(true);
        findLabel("3")->setInverted(true);
        findLabel("4")->setInverted(true);
		findMixerTopBackground()->setColor(false);
		findMixerFaderBackground()->setColor(true);
        findKnob()->setColor(true);
		findMixerFader()->setColor(false);
    }
	SetDirty();
}

void MixerStrip::setSelection(int i)
{
    selection = i;
    setColors();
}

void MixerStrip::setValueAString(std::string str)
{
	auto mixerScreen = mpc.screens->get<MixerScreen>("mixer");

	if (mixerScreen->getTab() == 1)
	{
		if (str.length() == 1)
		{
			findLabel("0")->setText(str);
			findLabel("0")->setLocation(xPos0indiv[columnIndex] + 2, yPos0indiv + 2);
			findLabel("1")->Hide(true);
		}
		else if (str.length() == 2)
		{
			findLabel("0")->setLocation(xPos0indiv[columnIndex], yPos0indiv);
			findLabel("0")->setText(str.substr(0, 1));
			findLabel("1")->Hide(false);
			findLabel("1")->setText(str.substr(1, 2));
		}
	}
	else if (mixerScreen->getTab() == 2)
	{
		findLabel("0")->setText(str.length() > 0 ? str.substr(0, 1) : "");
		findLabel("1")->setText(str.length() > 1 ? str.substr(1, 2) : "");
	}
    
	SetDirty();
}

std::shared_ptr<MixerFader> MixerStrip::findMixerFader()
{
	return std::dynamic_pointer_cast<MixerFader>(findChild("mixer-fader-background")->findChild("mixer-fader"));
}

std::shared_ptr<Knob> MixerStrip::findKnob()
{
	return std::dynamic_pointer_cast<Knob>(findChild("mixer-top-background")->findChild("knob"));
}

std::shared_ptr<MixerTopBackground> MixerStrip::findMixerTopBackground()
{
	for (auto& c : children)
	{
		if (c->getName() == "mixer-top-background")
		{
			return std::dynamic_pointer_cast<MixerTopBackground>(c);
		}
	}
	return {};
}

std::shared_ptr<MixerFaderBackground> MixerStrip::findMixerFaderBackground()
{
	for (auto& c : children)
	{
		if (c->getName() == "mixer-fader-background")
		{
			return std::dynamic_pointer_cast<MixerFaderBackground>(c);
		}
	}
	return {};
}
