#include <lcdgui/MixerStrip.hpp>

#include <lcdgui/Knob.hpp>
#include <lcdgui/Label.hpp>
#include <lcdgui/MixerFaderBackground.hpp>
#include <lcdgui/MixerTopBackground.hpp>
#include <lcdgui/MixerFader.hpp>
#include <lcdgui/screens/MixerScreen.hpp>
#include <lcdgui/Screens.hpp>

#include <lang/StrUtil.hpp>

using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui;
using namespace moduru::lang;

MixerStrip::MixerStrip(int columnIndex)
	: Component("mixer-strip")
{
	this->columnIndex = columnIndex;

	yPos0indiv = 0;
	yPos1indiv = 4;
	yPos0fx = 2;
	yPos1fx = 2;
	selection = -1;

	auto x1 = 4 + (columnIndex * 15);
	addChild(move(make_shared<MixerTopBackground>(MRECT(x1, 0, x1 + 14, 13))));
	addChild(move(make_shared<MixerFaderBackground>(MRECT(x1, 14, x1 + 14, 50))));

	auto x2 = 5 + (columnIndex * 15);
	findChild("mixer-top-background").lock()->addChild(move(make_shared<Knob>(MRECT(x2, 1, x2 + 12, 12))));

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

		auto label = make_shared<Label>(to_string(i), "", xPos, yPos, 5);

		if (i < 2)
		{
			findMixerTopBackground().lock()->addChild(move(label));
		}
		else
		{
			findMixerFaderBackground().lock()->addChild(move(label));
		}
	}

	auto x3 = 12 + (columnIndex * 15);
	findMixerFaderBackground().lock()->addChild(move(make_shared<MixerFader>(MRECT(x3, 15, x3 + 4, 49))));

	auto padName = StrUtil::padLeft(to_string(columnIndex + 1), "0", 2);
	findLabel("3").lock()->setText(padName.substr(0, 1));
	findLabel("4").lock()->setText(padName.substr(1, 2));
}

void MixerStrip::setBank(int i)
{
	findLabel("2").lock()->setText(abcd[i]);
	findMixerFaderBackground().lock()->SetDirty();
}

void MixerStrip::setValueA(int i)
{
    findKnob().lock()->setValue(i);
	findMixerTopBackground().lock()->SetDirty();
}

void MixerStrip::setValueB(int i)
{
    findMixerFader().lock()->setValue(i);
	findMixerFaderBackground().lock()->SetDirty();
}

void MixerStrip::initLabels()
{
	auto mixerScreen = dynamic_pointer_cast<MixerScreen>(Screens::getScreenComponent("mixer"));

	if (mixerScreen->getTab() == 0)
	{
		findKnob().lock()->Hide(false);
		findLabel("0").lock()->Hide(true);
		findLabel("1").lock()->Hide(true);
	}
	else if (mixerScreen->getTab() == 1)
	{
		findKnob().lock()->Hide(true);
		findLabel("0").lock()->Hide(false);
		findLabel("1").lock()->Hide(false);
		findLabel("0").lock()->setLocation(xPos0indiv[columnIndex] - 1, yPos0indiv);
		findLabel("1").lock()->setLocation(xPos1indiv[columnIndex] - 1, yPos1indiv);
	}
	else if (mixerScreen->getTab() == 2)
	{
		findKnob().lock()->Hide(true);
		findLabel("0").lock()->Hide(false);
		findLabel("1").lock()->Hide(false);
		findLabel("0").lock()->setLocation(xPos0fx[columnIndex], yPos0fx);
		findLabel("1").lock()->setLocation(xPos1fx[columnIndex], yPos1fx);
	}
	SetDirty();
}

void MixerStrip::setColors()
{
	if (selection == -1)
	{
		for (int i = 0; i < 5; i++)
		{
			findLabel(to_string(i)).lock()->setInverted(false);
		}
		
		findMixerTopBackground().lock()->setColor(false);
		findMixerFaderBackground().lock()->setColor(false);
		findKnob().lock()->setColor(true);
		findMixerFader().lock()->setColor(true);
	}
	else if (selection == 0)
	{
		findLabel("0").lock()->setInverted(true);
		findLabel("1").lock()->setInverted(true);
		findLabel("2").lock()->setInverted(false);
		findLabel("3").lock()->setInverted(false);
		findLabel("4").lock()->setInverted(false);
		findMixerTopBackground().lock()->setColor(true);
		findMixerFaderBackground().lock()->setColor(false);
		findKnob().lock()->setColor(false);
		findMixerFader().lock()->setColor(true);
	}
    else if (selection == 1)
	{
        findLabel("0").lock()->setInverted(false);
        findLabel("1").lock()->setInverted(false);
        findLabel("2").lock()->setInverted(true);
        findLabel("3").lock()->setInverted(true);
        findLabel("4").lock()->setInverted(true);
		findMixerTopBackground().lock()->setColor(false);
		findMixerFaderBackground().lock()->setColor(true);
        findKnob().lock()->setColor(true);
		findMixerFader().lock()->setColor(false);
    }
	SetDirty();
}

void MixerStrip::setSelection(int i)
{
    selection = i;
    setColors();
}

void MixerStrip::setValueAString(string str)
{
	auto mixerScreen = dynamic_pointer_cast<MixerScreen>(Screens::getScreenComponent("mixer"));

	if (mixerScreen->getTab() == 1)
	{
		if (str.length() == 1)
		{
			findLabel("0").lock()->setText(str);
			findLabel("0").lock()->setLocation(xPos0indiv[columnIndex] + 2, yPos0indiv + 2);
			findLabel("1").lock()->Hide(true);
		}
		else if (str.length() == 2)
		{
			findLabel("0").lock()->setLocation(xPos0indiv[columnIndex], yPos0indiv);
			findLabel("0").lock()->setText(str.substr(0, 1));
			findLabel("1").lock()->Hide(false);
			findLabel("1").lock()->setText(str.substr(1, 2));
		}
	}
	else if (mixerScreen->getTab() == 2)
	{
		findLabel("0").lock()->setText(str.substr(0, 1));
		findLabel("1").lock()->setText(str.substr(1, 2));
	}
	SetDirty();
}

weak_ptr<MixerFader> MixerStrip::findMixerFader()
{
	return dynamic_pointer_cast<MixerFader>(findChild("mixer-fader-background").lock()->findChild("mixer-fader").lock());
}

weak_ptr<Knob> MixerStrip::findKnob()
{
	return dynamic_pointer_cast<Knob>(findChild("mixer-top-background").lock()->findChild("knob").lock());
}

weak_ptr<MixerTopBackground> MixerStrip::findMixerTopBackground()
{
	for (auto& c : children)
	{
		if (c->getName().compare("mixer-top-background") == 0)
		{
			return dynamic_pointer_cast<MixerTopBackground>(c);
		}
	}
	return {};
}

weak_ptr<MixerFaderBackground> MixerStrip::findMixerFaderBackground()
{
	for (auto& c : children)
	{
		if (c->getName().compare("mixer-fader-background") == 0)
		{
			return dynamic_pointer_cast<MixerFaderBackground>(c);
		}
	}
	return {};
}
