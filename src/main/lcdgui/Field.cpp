#include "Field.hpp"
#include "Label.hpp"
//#include <maingui/Constants.hpp>
//#include <StartUp.hpp>
#include <lcdgui/LayeredScreen.hpp>
#include <ui/NameGui.hpp>
//#include <lcdgui/Field_Blinker.hpp>
//#include <lcdgui/Field_Scroller.hpp>
#include <lcdgui/TwoDots.hpp>
//#include <ui/sequencer/TrMoveGui.hpp>

#include <lang/StrUtil.hpp>

//#include <Util.hpp>

#include <file/File.hpp>

#ifdef __linux__
#include <climits>
#endif // __linux__

using namespace mpc::lcdgui;
using namespace std;

Field::Field(LayeredScreen* layeredScreen, std::vector<std::vector<bool>>* atlas, moduru::gui::bmfont* font)
	: TextComp(atlas, font) {
	this->layeredScreen = layeredScreen;
}

void Field::initialize(std::string name, int x, int y, int columns) {
	noLeftMargin = false;
	split = false;
	focusable = true;
	opaque = true;
	inverted = false;
	focus = false;
	Hide(false);
	this->name = name;
	this->columns = columns;

	setText("");
	setLocation(x, y, false);
	setSize(columns * TEXT_WIDTH + 1, TEXT_HEIGHT + 1, false);
	clearRects.clear();
	//loseFocus(name);
}

void Field::Draw(vector<vector<bool>>* pixels) {
	const int margin = noLeftMargin ? 0 : 1;
	if (opaque) {
		for (int j = 0; j < w; j++) {
			for (int k = 0; k < TEXT_HEIGHT + 2; k++) {
				int x1 = x + j - margin;
				int y1 = y + k;
				if (x1 < 0 || x1 > 247 || y1 < 0 || y1 > 59) continue;
				(*pixels)[x1][y1] = inverted ? true : false;
			}
		}
	}
	TextComp::Draw(pixels);
}

void Field::setSize(int width, int height, bool clear) {
	TextComp::setSize(width, height, clear);
	setText(text);
}

const int Field::BLINKING_RATE;

void Field::takeFocus(string prev)
{
	csn = layeredScreen->getCurrentScreenName();
	focus = true;
	inverted = true;
	//if (csn.compare("name") == 0) setOpaque(true);
	//	//auto lMainFrame = lGui->getMainFrame().lock();
	//auto lMainFrame = mainFrame;
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
	if (csn.compare("startfine") == 0 || csn.compare("endfine") == 0 || csn.compare("looptofine") == 0 || csn.compare("loopendfine") == 0) {
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
	//if (csn.compare("directory") == 0) {
//		focusField.lock()->setOpaque(true);
	//}
	//if (!csn.compare("name") == 0) {
//		focusField.lock()->setOpaque(true);
	//}
	//if (csn.compare("name") == 0) {
		//		//if (!lGui->getNameGui()->isNameBeingEdited()) {
		//focusField.lock()->setOpaque(true);
	//}
	//if (!(csn.compare("name") == 0 && nameGui->isNameBeingEdited())) {
	//		focusField.lock()->setForeground(false);
	//}
	//	}
	//	if (csn.compare("tempochange") == 0) {
//		focusField.lock()->setOpaque(true);
//	}
//	if (csn.compare("trmove") == 0) {
//		//if (focusEvent.compare("tr1") == 0 && !lGui->getTrMoveGui()->isSelected())
//		//	lLs->drawFunctionKeys("trmove_notselected");
//	}
	SetDirty();
}

void Field::loseFocus(string next)
{
	focus = false;
	inverted = false;
	//if (csn.compare("name") == 0) {
//		setOpaque(false);
//	}
	auto focusEvent = getName();
	//setSplit(false);
	csn = layeredScreen->getCurrentScreenName();
	if (csn.compare("trim") == 0 || csn.compare("loop") == 0) {
		if (focusEvent.compare("st") == 0 || focusEvent.compare("to") == 0) {
			layeredScreen->getTwoDots().lock()->setSelected(0, false);
		}
		else if (focusEvent.compare("end") == 0 || focusEvent.compare("endlengthvalue") == 0) {
			layeredScreen->getTwoDots().lock()->setSelected(1, false);
		}
	}
	else if (csn.compare("startfine") == 0 || csn.compare("endfine") == 0 || csn.compare("looptofine") == 0 || csn.compare("loopendfine") == 0) {
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


	//if (csn.compare("tempochange") == 0) {
	//	setOpaque(false);
	//	setForeground(false);
	//}

	//if (csn.compare("name") == 0) {
//		setOpaque(false);
//	}

	////if ((!lGui->getNameGui()->isNameBeingEdited() && csn.compare("name") == 0))
	//	setOpaque(false);
	////if (!(csn.compare("name") == 0 && lGui->getNameGui()->isNameBeingEdited())) {
	//	setForeground(!getForeground());
	////}
	//if (csn.compare("trmove") == 0) {
	////	if (focusEvent.compare("tr1") == 0 && !lGui->getTrMoveGui()->isSelected())
	////		lLs->drawFunctionKeys("trmove");
	//}
	//else if (csn.compare("save") == 0) {
	//	setOpaque(false);
	//}
	//else if (csn.compare("assignmentview") == 0) {
	//	setOpaque(false);
	//}

	//if (csn.compare("directory") == 0) {
		//setOpaque(false);
	//}

	SetDirty();
}


void Field::setSplit(bool b)
{
	if (split == b) return;
	split = b;
	if (split) {
		this->setOpaque(false);
		if (letters.size() != 0) {
			for (auto& l : letters)
				delete l;
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

		setOpaque(true);
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
		//letters[i]->setOpaque(i < activeSplit);
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
	if (i < 1 || i + 1 > letters.size()) return false;
	activeSplit = i;
	redrawSplit();
	return true;
}

bool Field::enableTypeMode()
{
    if(typeModeEnabled)
        return false;

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
    if(!typeModeEnabled)
        return value;

    //setFont(mpc2000font);
    //setFontColor(Constants::LCD_OFF);
	//setTransparency(false);
    typeModeEnabled = false;
    try {
	//	string valueString = getText();
	//	value = stoi(valueString);
	}
	catch (std::invalid_argument& e) {
		printf("Field.enter ERROR: %s", e.what());
        return value;
    }
    //setText(oldText.c_str());
    return value;
}

void Field::type(int i)
{
	/*
	string str = getText();
	remove_if(str.begin(), str.end(), isspace);
    if(str.length() == getColumns())
        str = "";
	string newStr = str.append(to_string(i));
	pad_left(newStr, getColumns());
    setText(newStr.c_str());
	*/
}

bool Field::isTypeModeEnabled()
{
    return typeModeEnabled;
}

void Field::disableTypeMode()
{
    if(!typeModeEnabled)
        return;

    typeModeEnabled = false;
    //setFont(Constants::mpc2000xlFont);
    //setFontColor(Constants::LCD_OFF);
	//setTransparency(false);
    //setText(oldText.c_str());
}

/*
void Field::enableScrolling(vector<CLabel> enablers)
{
    if(scroller != nullptr)
        scroller->stopped = true;

	// check if sizeof gets the right amount
    if(getColumns() > sizeof(getText()))
        return;

    //scroller = new Field_Scroller(this, this, enablers);
    //(new ::java::lang::Thread(static_cast< ::java::lang::Runnable* >(scroller)))->start();
    scrolling = true;
}
*/

void Field::startBlinking()
{
    blinking = true;
    //(new ::java::lang::Thread(static_cast< ::java::lang::Runnable* >(new Field_Blinker())))->start();
}

void Field::stopBlinking()
{
    blinking = false;
}

bool Field::getBlinking()
{
    return this->blinking;
}

void Field::setFocusable(bool b) {
	focusable = b;
}

bool Field::isFocusable() {
	return focusable;
}

bool Field::hasFocus() {
	return focus;
}

Field::~Field()
{
	for (auto& l : letters) {
		delete l;
	}
}
