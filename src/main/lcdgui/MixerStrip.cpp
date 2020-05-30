#include <lcdgui/MixerStrip.hpp>

#include <lcdgui/Knob.hpp>
#include <lcdgui/Label.hpp>
#include <lcdgui/MixerFaderBackground.hpp>
#include <lcdgui/MixerTopBackground.hpp>
#include <lcdgui/VerticalBar.hpp>
#include <lcdgui/screens/MixerScreen.hpp>
#include <lcdgui/Screens.hpp>

using namespace std;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui;

MixerStrip::MixerStrip(int columnIndex, int bank, vector<weak_ptr<Label>> labels)
	: Component("mixer-strip")
{
	this->columnIndex = columnIndex;
	this->labels = labels;

	yPos0indiv = 0;
	yPos1indiv = 4;
	yPos0fx = 2;
	yPos1fx = 2;
	selection = -1;

	auto x = 12 + (columnIndex * 15);
	addChild(move(make_shared<VerticalBar>(MRECT(x, 16, x + 5, 53))));

	x = 4 + (columnIndex * 15);
	addChild(move(make_shared<MixerFaderBackground>(MRECT(x, 15, x + 13, 49))));
	addChild(move(make_shared<MixerTopBackground>(MRECT(x, 0, x + w, 12))));

	x = 5 + (columnIndex * 15);
	addChild(move(make_shared<Knob>(MRECT(x, 1, x + 13, 14))));

	labels[1].lock()->setOpaque(false);
	labels[2].lock()->setOpaque(true);
	labels[2].lock()->setText(abcd[bank]);

	initLabels();
	setColors();
}

void MixerStrip::setValueA(int i)
{
    findKnob().lock()->setValue(i);
}

void MixerStrip::setValueB(int i)
{
    findVerticalBar().lock()->setValue(i);
}

void MixerStrip::initLabels()
{
	auto mixerScreen = dynamic_pointer_cast<MixerScreen>(Screens::getScreenComponent("mixer"));

	if (mixerScreen->getTab() == 0)
	{
		findKnob().lock()->Hide(false);
		labels[0].lock()->Hide(true);
		labels[1].lock()->Hide(true);
		labels[2].lock()->Hide(false);
		labels[3].lock()->Hide(false);
		labels[4].lock()->Hide(false);
	}
	else if (mixerScreen->getTab() == 1)
	{
		findKnob().lock()->Hide(true);
		labels[0].lock()->Hide(false);
		labels[1].lock()->Hide(false);
		labels[0].lock()->setLocation(xPos0indiv[columnIndex] - 1, yPos0indiv);
		labels[1].lock()->setLocation(xPos1indiv[columnIndex] - 1, yPos1indiv);
	}
	else if (mixerScreen->getTab() == 2)
	{
		findKnob().lock()->Hide(true);
		labels[0].lock()->Hide(false);
		labels[1].lock()->Hide(false);
		labels[0].lock()->setLocation(xPos0fx[columnIndex], yPos0fx);
		labels[1].lock()->setLocation(xPos1fx[columnIndex], yPos1fx);
	}
}

void MixerStrip::setColors()
{
	if (selection == -1)
	{
		for (auto& tf : labels)
		{
			tf.lock()->setInverted(false);
		}
		
		findMixerTopBackground().lock()->Hide(true);
		findMixerFaderBackground().lock()->Hide(true);
		findKnob().lock()->setColor(true);
		findVerticalBar().lock()->Hide(false);
		findVerticalBar().lock()->setColor(true);
	}
	else if (selection == 0)
	{
		labels[0].lock()->setInverted(true);
		labels[1].lock()->setInverted(true);
		labels[2].lock()->setInverted(false);
		labels[3].lock()->setInverted(false);
		labels[4].lock()->setInverted(false);
		findMixerTopBackground().lock()->Hide(false);
		findMixerFaderBackground().lock()->Hide(true);
		findKnob().lock()->setColor(false);
		findVerticalBar().lock()->Hide(false);
		findVerticalBar().lock()->setColor(true);
	}
    else if (selection == 1)
	{
        labels[0].lock()->setInverted(false);
        labels[1].lock()->setInverted(false);
        labels[2].lock()->setInverted(true);
        labels[3].lock()->setInverted(true);
        labels[4].lock()->setInverted(true);
		findMixerTopBackground().lock()->Hide(true); 
		findMixerFaderBackground().lock()->Hide(false);
        findKnob().lock()->setColor(true);
		findVerticalBar().lock()->Hide(false);
		findVerticalBar().lock()->setColor(false);
    }
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
			labels[0].lock()->setText(str);
			labels[0].lock()->setLocation(xPos0indiv[columnIndex] + 2, yPos0indiv + 2);
			labels[1].lock()->setText("");
		}
		else if (str.length() == 2)
		{
			labels[0].lock()->setLocation(xPos0indiv[columnIndex], yPos0indiv);
			labels[0].lock()->setText(str.substr(0, 1));
			labels[1].lock()->setText(str.substr(1, 2));
		}
	}
	else if (mixerScreen->getTab() == 2)
	{
		labels[0].lock()->setText(str.substr(0, 1));
		labels[1].lock()->setText(str.substr(1, 2));
	}
	findMixerTopBackground().lock()->SetDirty();
}

weak_ptr<VerticalBar> MixerStrip::findVerticalBar()
{
	for (auto& c : children)
	{
		if (c->getName().compare("vertical-bar") == 0)
		{
			return dynamic_pointer_cast<VerticalBar>(c);
		}
	}
	return {};
}

std::weak_ptr<Knob> MixerStrip::findKnob()
{
	for (auto& c : children)
	{
		if (c->getName().compare("knob") == 0)
		{
			return dynamic_pointer_cast<Knob>(c);
		}
	}
	return {};
}

std::weak_ptr<MixerTopBackground> MixerStrip::findMixerTopBackground()
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

std::weak_ptr<MixerFaderBackground> MixerStrip::findMixerFaderBackground()
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
