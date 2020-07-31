#include "Field.hpp"

#include "Label.hpp"

#include <Mpc.hpp>
#include <lcdgui/LayeredScreen.hpp>
#include <lcdgui/TwoDots.hpp>

#include <lang/StrUtil.hpp>

#include <file/File.hpp>

#ifdef __linux__
#include <climits>
#endif // __linux__

#include <stdexcept>

using namespace mpc::lcdgui;
using namespace std;

Field::Field(mpc::Mpc& mpc, const string& name, int x, int y, int width)
	: TextComp(mpc, name), mpc(mpc)
{
	this->name = name;
	
	if (width == 1)
	{
		// We use width 0 as an indicator of wanting to be completely invisible, like dummy fields
		width = 0;
	}

	setSize(width, 9);
	setLocation(x - 1, y - 1);
	preDrawClearRect.Clear();
}

void Field::setNextFocus(const std::string& nextFocus)
{
	this->nextFocus = nextFocus;
}

void Field::Hide(bool b)
{
	if (b && focus)
	{
		mpc.getLayeredScreen().lock()->setFocus(nextFocus);
	}

	Component::Hide(b);
}

void Field::Draw(std::vector<std::vector<bool>>* pixels)
{
	if (shouldNotDraw(pixels))
	{
		return;
	}
	
	auto r = getRect();

	for (int i = r.L; i < r.R; i++)
	{
		if (i < 0)
		{
			continue;
		}

		for (int j = r.T; j < r.B; j++)
		{
			if (j < 0)
			{
				continue;
			}
			(*pixels)[i][j] = inverted;
		}
	}
	TextComp::Draw(pixels);
}

void Field::takeFocus(string prev)
{
	auto layeredScreen = mpc.getLayeredScreen().lock();
	csn = layeredScreen->getCurrentScreenName();
	focus = true;
	inverted = true;
	/*
	auto focusEvent = layeredScreen->getFocus();
	auto focusField = layeredScreen->lookupField(focusEvent);
	if (csn.compare("trim") == 0 || csn.compare("loop") == 0) {
		if (focusEvent.compare("st") == 0 || focusEvent.compare("to") == 0) {
			layeredScreen->getTwoDots().lock()->setSelected(0, true);
		}
		else if (focusEvent.compare("end") == 0 || focusEvent.compare("endlengthvalue") == 0) {
			layeredScreen->getTwoDots().lock()->setSelected(1, true);
		}
	}
	if (csn.compare("start-fine") == 0 || csn.compare("end-fine") == 0 || csn.compare("loop-to-fine") == 0 || csn.compare("loop-end-fine") == 0) {
		if (focusEvent.compare("start") == 0) {
			layeredScreen->getTwoDots().lock()->setSelected(2, true);
		}
		else if (focusEvent.compare("end") == 0) {
			layeredScreen->getTwoDots().lock()->setSelected(2, true);
		}
		else if (focusEvent.compare("to") == 0) {
			layeredScreen->getTwoDots().lock()->setSelected(2, true);
		}
		else if (focusEvent.compare("lngth") == 0) {
			layeredScreen->getTwoDots().lock()->setSelected(3, true);
		}
	}
	*/
	SetDirty();
}

void Field::loseFocus(string next)
{
	focus = false;
	inverted = false;
	/*
	auto focusEvent = getName();
	
	auto layeredScreen = mpc.getLayeredScreen().lock();

	csn = layeredScreen->getCurrentScreenName();
	if (csn.compare("trim") == 0 || csn.compare("loop") == 0) {
		if (focusEvent.compare("st") == 0 || focusEvent.compare("to") == 0) {
			layeredScreen->getTwoDots().lock()->setSelected(0, false);
		}
		else if (focusEvent.compare("end") == 0 || focusEvent.compare("endlengthvalue") == 0) {
			layeredScreen->getTwoDots().lock()->setSelected(1, false);
		}
	}
	else if (csn.compare("start-fine") == 0 || csn.compare("end-fine") == 0 || csn.compare("loop-to-fine") == 0 || csn.compare("loop-end-fine") == 0) {
		if (focusEvent.compare("start") == 0) {
			layeredScreen->getTwoDots().lock()->setSelected(2, false);
		}
		else if (focusEvent.compare("end") == 0) {
			layeredScreen->getTwoDots().lock()->setSelected(2, false);
		}
		else if (focusEvent.compare("to") == 0) {
			layeredScreen->getTwoDots().lock()->setSelected(2, false);
		}
		else if (focusEvent.compare("lngth") == 0) {
			layeredScreen->getTwoDots().lock()->setSelected(3, false);
		}
	}
	*/
	SetDirty();
}


void Field::setSplit(bool b)
{
	if (split == b)
	{
		return;
	}
	
	split = b;
	
	if (split)
	{
		if (letters.size() != 0)
		{
			for (auto& l : letters)
			{
				delete l;
			}
		}

		letters.clear();
		letters = vector<Label*>(getText().length());
		activeSplit = letters.size() - 1;
		//auto x = getLocation())->x;
		//auto y = npc(this->getLocation())->y;

		for (int i = 0; i < letters.size(); i++) {
			//letters[i] = new Label(GetGUI()->GetPlug());
			//letters[i]->setFocusable(false);
			//GetGUI()->AttachControl(letters[i]);
		}
		setText(getText());
		redrawSplit();
		//SetDirty(true);
	}
	else {
		if (letters.size() == 0)
			return;

		//for (int i = 0; i < letters.size(); i++)
			//GetGUI()->DetachControl(letters[i]);
		activeSplit = 0;
		letters.clear();
		//SetDirty(true);
	}
}

void Field::redrawSplit()
{
	for (int i = 0; i < letters.size(); i++) {
		//letters[i]->setForeground(i < activeSplit);
		//letters[i]->setBackground(i < activeSplit ? mpc::maingui::Constants::LCD_ON() : mpc::maingui::Constants::LCD_OFF());
	}
	//SetDirty(true);
}

bool Field::isSplit()
{
    return split;
}

int Field::getActiveSplit()
{
    return activeSplit;
}

bool Field::setActiveSplit(int i)
{
	if (i < 1 || i + 1 > letters.size())
	{
		return false;
	}
	activeSplit = i;
	redrawSplit();
	return true;
}

bool Field::enableTypeMode()
{
	if (typeModeEnabled)
	{
		return false;
	}

    typeModeEnabled = true;
	//oldText = this->getText();
    //setFont(mpc2000fontunderline);
    //setFontColor(gui::Constants::LCD_ON);
	//setTransparency(true);
    setText("");
    return true;
}

int Field::enter()
{
	auto value = INT_MAX;

	if (!typeModeEnabled)
	{
		return value;
	}

    //setFont(mpc2000font);
    //setFontColor(Constants::LCD_OFF);
	//setTransparency(false);
    typeModeEnabled = false;
    
	try
	{
	//	string valueString = getText();
	//	value = stoi(valueString);
	}
	catch (std::invalid_argument& e)
	{
		printf("Field.enter ERROR: %s", e.what());
        return value;
    }
    //setText(oldText.c_str());
    return value;
}

void Field::type(int i)
{
}

bool Field::isTypeModeEnabled()
{
    return typeModeEnabled;
}

void Field::disableTypeMode()
{
	if (!typeModeEnabled)
	{
		return;
	}

    typeModeEnabled = false;
    //setFontColor(Constants::LCD_OFF);
	//setTransparency(false);
    //setText(oldText.c_str());
}

void Field::setFocusable(bool b)
{
	focusable = b;
}

bool Field::isFocusable()
{
	return focusable;
}

bool Field::hasFocus()
{
	return focus;
}

Field::~Field()
{
	for (auto& l : letters)
	{
		delete l;
	}
}
