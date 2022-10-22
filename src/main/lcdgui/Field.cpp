#include "Field.hpp"

#include <Mpc.hpp>
#include <lcdgui/ScreenComponent.hpp>
#include <lcdgui/Rectangle.hpp>

#include <Util.hpp>

#include <lang/StrUtil.hpp>

#include <stdexcept>

#ifdef __linux__
#include <climits>
#endif // __linux__

#include <stdexcept>

using namespace mpc::lcdgui;
using namespace moduru::lang;
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

void Field::setRectangleOnly(bool b)
{
	rectangleOnly = b;
	SetDirty();
}

void Field::setNextFocus(const std::string& nextFocus)
{
	this->nextFocus = nextFocus;
}

void Field::Hide(bool b)
{
	if (b && focus)
		mpc.getLayeredScreen().lock()->setFocus(nextFocus);

	Component::Hide(b);
}

void Field::Draw(std::vector<std::vector<bool>>* pixels)
{
	if (shouldNotDraw(pixels))
		return;

	auto r = getRect();

	if (split)
	{
		Clear(pixels);
		auto columns = (int) floor((w - 2) / FONT_WIDTH);
		auto nonInvertedColumns = columns - activeSplit;
		r.R = r.L + (w - (nonInvertedColumns * FONT_WIDTH));
	}

	for (int i = r.L; i < r.R; i++)
	{
		if (i < 0)
			continue;

		for (int j = r.T; j < r.B; j++)
		{
			if (j < 0)
				continue;

			if (typeModeEnabled && j - r.T <= 7)
				(*pixels)[i][j] = !inverted;
			else
				(*pixels)[i][j] = inverted;
		}
	}

	auto oldInverted = inverted;

	if (typeModeEnabled)
		inverted = false;

	if (!rectangleOnly)
		TextComp::Draw(pixels);

	if (typeModeEnabled)
	{
		inverted = oldInverted;
		(*pixels)[x][y + FONT_HEIGHT + 1] = false;
		(*pixels)[x + 12][y + FONT_HEIGHT + 1] = false;
		(*pixels)[x + 30][y + FONT_HEIGHT + 1] = false;
	}
}

void Field::takeFocus(string prev)
{
	auto ls = mpc.getLayeredScreen().lock();
	csn = ls->getCurrentScreenName();

	if (csn.compare("step-editor") == 0)
	{
		if (name.compare("view") == 0)
		{
			auto screen = ls->findScreenComponent().lock();
			screen->findField("fromnote").lock()->setInverted(true);
			screen->findField("tonote").lock()->setInverted(true);
			screen->findLabel("tonote").lock()->setInverted(true);
			screen->findChild<Rectangle>("").lock()->setOn(true);
		}
	}
	else if (csn.compare("multi-recording-setup") == 0)
	{
		if (name.length() == 2 && name[0] == 'b')
		{
			setSplit(true);
			setActiveSplit(1);
		}	
	}
	else if (csn.compare("sequencer") == 0)
	{
		if (name.find("now") != string::npos || name.compare("tempo") == 0)
			Util::initSequence(mpc);
	}

	focus = true;
	inverted = true;
	SetDirty();
}

void Field::loseFocus(string next)
{
	auto ls = mpc.getLayeredScreen().lock();
	csn = ls->getCurrentScreenName();

	focus = false;
	inverted = false;

	if (csn.compare("step-editor") == 0)
	{
		if (name.compare("view") == 0)
		{
			auto screen = ls->findScreenComponent().lock();
			screen->findChild<Rectangle>("").lock()->setOn(false);

			if (next.compare("fromnote") != 0)
				screen->findField("fromnote").lock()->setInverted(false);

			screen->findField("tonote").lock()->setInverted(false);
			screen->findLabel("tonote").lock()->setInverted(false);
		}
	}
	
	if (typeModeEnabled)
		disableTypeMode();

	if (split)
		setSplit(false);

	SetDirty();
}


void Field::setSplit(bool b)
{
	if (split == b)
		return;
	
	split = b;
	
	if (split)
	{
		if (typeModeEnabled)
			mpc.getActiveControls().lock()->pressEnter();

		activeSplit = text.length() - 2;
	}
	else {
		activeSplit = -1;
	}

	SetDirty();
}

bool Field::isSplit()
{
    return split;
}

int Field::getActiveSplit()
{
    return activeSplit;
}

int Field::getSplitIncrement(bool positive)
{
	static const vector<int> splitInc { 10000000, 1000000, 100000, 10000, 1000, 100, 10, 1 };
	return splitInc[activeSplit] * (positive ? 1 : -1);
}

bool Field::setActiveSplit(int i)
{
	if (i < 0 || i > text.size() - 1)
		return false;

	activeSplit = i;

	SetDirty();
	return true;
}

bool Field::enableTypeMode()
{
	if (typeModeEnabled)
		return false;

	if (split)
	{
		auto oldActiveSplit = activeSplit;
		setSplit(false);
		activeSplit = oldActiveSplit;
	}

    typeModeEnabled = true;
	oldText = text;
	setText("");
    return true;
}

int Field::enter()
{
	auto value = INT_MAX;

	if (!typeModeEnabled)
		return value;

    typeModeEnabled = false;
    
	try
	{
		value = stoi(getText());
	}
	catch (std::invalid_argument& e)
	{
		printf("Field.enter ERROR: %s", e.what());
        return value;
    }

    setText(oldText);
    return value;
}

void Field::type(int i)
{
	auto textCopy = StrUtil::replaceAll(getText(), ' ', "");
	
	if (textCopy.length() == floor(w / FONT_WIDTH))
		textCopy = "";

	if (textCopy.compare("0") == 0 && i == 0)
		return;

	if (textCopy.compare("0") == 0)
		textCopy = "";

	auto newText = textCopy.append(to_string(i));
	setTextPadded(newText.c_str());
}

bool Field::isTypeModeEnabled()
{
    return typeModeEnabled;
}

void Field::disableTypeMode()
{
	if (!typeModeEnabled)
		return;

    typeModeEnabled = false;
    setText(oldText.c_str());
}

void Field::setFocusable(bool b)
{
	focusable = b;

	if (!focusable && focus)
		mpc.getLayeredScreen().lock()->setFocus(nextFocus);
}

bool Field::isFocusable()
{
	return focusable;
}

bool Field::hasFocus()
{
	return focus;
}
