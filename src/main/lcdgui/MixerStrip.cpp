#include <lcdgui/MixerStrip.hpp>

#include <Mpc.hpp>
#include <ui/Uis.hpp>
#include <lcdgui/LayeredScreen.hpp>
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

MixerStrip::MixerStrip(int columnIndex, int bank)
{
	this->columnIndex = columnIndex;
	abcd = vector<string>{ "A", "B", "C", "D" };
	letters = vector<string>{ "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p" };
	xPos0indiv = vector<int>{ 5, 20, 35, 50, 65, 80, 95, 110, 125, 140, 155, 170, 185, 200, 215, 230 };
	xPos1indiv = vector<int>{ 12, 27, 42, 57, 72, 87, 102, 117, 132, 147, 162, 177, 192, 207, 222, 237 };
	yPos0indiv = 0;
	yPos1indiv = 4;
	xPos0fx = vector<int>{ 5, 20, 35, 50, 65, 80, 95, 110, 125, 140, 155, 170, 185, 200, 215, 230 };
	xPos1fx = vector<int>{ 11, 26, 41, 56, 71, 86, 101, 116, 131, 146, 161, 176, 191, 206, 221, 236 };
	yPos0fx = 2;
	yPos1fx = 2;
	selection = -1;

	auto lLs = Mpc::instance().getLayeredScreen().lock();
	verticalBar = lLs->getVerticalBarsMixer()[columnIndex];
	verticalBar.lock()->Hide(false);
	knob = lLs->getKnobs()[columnIndex];
	mixerFaderBackground = lLs->getMixerFaderBackgrounds()[columnIndex];
	mixerTopBackground = lLs->getMixerTopBackgrounds()[columnIndex];
	mixerStrip.push_back(verticalBar);
	mixerStrip.push_back(knob);
	mixerStrip.push_back(mixerTopBackground);
	mixerStrip.push_back(mixerFaderBackground);
	tf0 = lLs->lookupLabel(letters[columnIndex] + "0");
	tf1 = lLs->lookupLabel(letters[columnIndex] + "1");
	tf2 = lLs->lookupLabel(letters[columnIndex] + "2");
	tf3 = lLs->lookupLabel(letters[columnIndex] + "3");
	tf4 = lLs->lookupLabel(letters[columnIndex] + "4");
	tf1.lock()->setOpaque(false);
	tf2.lock()->setOpaque(true);
	labels = { tf0, tf1, tf2, tf3, tf4 };
	
	tf2.lock()->setText(abcd[bank]);
	initLabels();
	setColors();
}

vector<weak_ptr<Component>> MixerStrip::getMixerStrip()
{
    return mixerStrip;
}

void MixerStrip::setValueA(int i)
{
    knob.lock()->setValue(i);
}

void MixerStrip::setValueB(int i)
{
    verticalBar.lock()->setValue(i);
}

void MixerStrip::initLabels()
{

	auto mixerScreen = dynamic_pointer_cast<MixerScreen>(Screens::getScreenComponent("mixer"));

	if (mixerScreen->getTab() == 0)
	{
		knob.lock()->Hide(false);
		tf0.lock()->Hide(true);
		tf1.lock()->Hide(true);
		tf2.lock()->Hide(false);
		tf3.lock()->Hide(false);
		tf4.lock()->Hide(false);
	}
	else if (mixerScreen->getTab() == 1)
	{
		knob.lock()->Hide(true);
		tf0.lock()->Hide(false);
		tf1.lock()->Hide(false);
		tf0.lock()->setLocation(xPos0indiv[columnIndex] - 1, yPos0indiv);
		tf1.lock()->setLocation(xPos1indiv[columnIndex] - 1, yPos1indiv);
	}
	else if (mixerScreen->getTab() == 2)
	{
		knob.lock()->Hide(true);
		tf0.lock()->Hide(false);
		tf1.lock()->Hide(false);
		tf0.lock()->setLocation(xPos0fx[columnIndex], yPos0fx);
		tf1.lock()->setLocation(xPos1fx[columnIndex], yPos1fx);
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
		
		mixerTopBackground.lock()->Hide(true);
		mixerFaderBackground.lock()->Hide(true);
		knob.lock()->setColor(true);
		verticalBar.lock()->Hide(false);
		verticalBar.lock()->setColor(true);
	}
	else if (selection == 0)
	{
		labels[0].lock()->setInverted(true);
		labels[1].lock()->setInverted(true);
		labels[2].lock()->setInverted(false);
		labels[3].lock()->setInverted(false);
		labels[4].lock()->setInverted(false);
		mixerTopBackground.lock()->Hide(false);
		mixerFaderBackground.lock()->Hide(true);
		knob.lock()->setColor(false);
		verticalBar.lock()->Hide(false);
		verticalBar.lock()->setColor(true);
	}
    else if(selection == 1)
	{
        labels[0].lock()->setInverted(false);
        labels[1].lock()->setInverted(false);
        labels[2].lock()->setInverted(true);
        labels[3].lock()->setInverted(true);
        labels[4].lock()->setInverted(true);
		mixerTopBackground.lock()->Hide(true); 
		mixerFaderBackground.lock()->Hide(false);
        knob.lock()->setColor(true);
		verticalBar.lock()->Hide(false);
		verticalBar.lock()->setColor(false);
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
	mixerTopBackground.lock()->SetDirty();
}
