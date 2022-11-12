#include "NameScreen.hpp"

#include <lcdgui/Underline.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui;

using namespace moduru::lang;

NameScreen::NameScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "name", layerIndex)
{
	addChildT<Underline>();
}

void NameScreen::mainScreen()
{
    actionWhenGoingToMainScreen();
    ScreenComponent::mainScreen();
}

void NameScreen::setRenamerAndScreenToReturnTo(const std::function<void(std::string&)>& newRenamer, const std::string& screen)
{
    renamer = newRenamer;
    screenToReturnTo = screen;
}

std::weak_ptr<Underline> NameScreen::findUnderline()
{
	return findChild<Underline>("underline");
}

void NameScreen::open()
{
	for (int i = 0; i < 16; i++)
		findUnderline().lock()->setState(i, false);

	displayName();
}

void NameScreen::close()
{
    ls.lock()->setLastFocus("name", "0");
    editing = false;
    parameterName = "";
    actionWhenGoingToMainScreen = [](){};
}

void NameScreen::left()
{
	init();

	if (stoi(param) == 0)
		return;

	ScreenComponent::left();
	
	if (editing)
	{
        findFocus()->setInverted(false);
		drawUnderline();
	}
}

void NameScreen::right()
{
	init();

	if (stoi(param) == nameLimit - 1)
		return;
	
	ScreenComponent::right();
	
	if (editing)
	{
        findFocus()->setInverted(false);
		drawUnderline();
	}
}

void NameScreen::turnWheel(int j)
{
	init();

	if (editing)
	{
		for (int i = 0; i < 16; i++)
		{
			if (param == std::to_string(i))
			{
				changeNameCharacter(i, j > 0);
				drawUnderline();
				break;
			}
		}
	}
	else
	{
		for (int i = 0; i < 16; i++)
		{
			if (param == std::to_string(i))
			{
				changeNameCharacter(i, j > 0);
				editing = true;
				initEditColors();
				drawUnderline();
				break;
			}
		}
	}
}

void NameScreen::function(int i)
{
    init();

    switch (i)
	{
    case 3:
	{
		if (find(begin(saveScreens), end(saveScreens), parameterName) != end(saveScreens))
        {
            name = originalName;
            openScreen(parameterName);
        }
		else
			openScreen(ls.lock()->getPreviousScreenName());
		break;
	}
	case 4:
		saveName();
		break;
    }
}

void NameScreen::pressEnter()
{
	saveName();
}

void NameScreen::saveName()
{
    if (find(begin(saveScreens), end(saveScreens), parameterName) != end(saveScreens))
    {
        openScreen(parameterName);
    }
    else
    {
        auto newName = getNameWithoutSpaces();
        renamer(newName);
        
        if (screenToReturnTo.length() > 0)
            openScreen(screenToReturnTo);
    }
}

void NameScreen::drawUnderline()
{
	if (editing)
	{
        std::string focus = ls.lock()->getFocus();
	
		if (focus.length() != 1 && focus.length() != 2)
			return;
		
		auto u = findUnderline().lock();
		
		for (int i = 0; i < 16; i++)
			u->setState(i, i == stoi(focus));

		bringToFront(u.get());
	}
}

void NameScreen::initEditColors()
{
    for (int i = 0; i < 16; i++)
	{
		auto field = findField(std::to_string(i));
		field->setInverted(false);
    }

	init();
	findField(param)->setInverted(false);
}

void NameScreen::setName(std::string newName)
{
    name = newName;
	nameLimit = 16;
	originalName = newName;
}

void NameScreen::setNameLimit(int i)
{
	name = name.substr(0, i);
	nameLimit = i;
}

void NameScreen::setName(std::string str, int i)
{
	name[i] = str[0];
}

std::string NameScreen::getNameWithoutSpaces()
{
	auto s = name;

	while (!s.empty() && isspace(s.back()))
		s.pop_back();

	for (int i = 0; i < s.length(); i++)
		if (s[i] == ' ') s[i] = '_';

    return s;
}

void NameScreen::changeNameCharacter(int i, bool up)
{
    if (i >= name.length())
        name = StrUtil::padRight(name, " ", i + 1);
        
	char schar = name[i];
    std::string s{ schar };
	auto stringCounter = 0;
	
	for (auto str : mpc::Mpc::akaiAscii)
	{
		if (str == s)
			break;

		stringCounter++;
	}

	if (stringCounter == 0 && !up)
		return;

	if (stringCounter == 75 && up)
		return;

	auto change = -1;
	
	if (up)
		change = 1;

	if (stringCounter > 75)
		s = " ";
	else
		s = mpc::Mpc::akaiAscii[stringCounter + change];
	
	name = name.substr(0, i).append(s).append(name.substr(i + 1, name.length()));
    displayName();
}

void NameScreen::displayName()
{
	if (nameLimit == 0)
		return;

    auto paddedName = StrUtil::padRight(name, " ", nameLimit);
    
	findField("0")->setText(paddedName.substr(0, 1));
	findField("1")->setText(paddedName.substr(1, 1));
	findField("2")->setText(paddedName.substr(2, 1));
	findField("3")->setText(paddedName.substr(3, 1));
	findField("4")->setText(paddedName.substr(4, 1));
	findField("5")->setText(paddedName.substr(5, 1));
	findField("6")->setText(paddedName.substr(6, 1));
	findField("7")->setText(paddedName.substr(7, 1));

	if (nameLimit > 8)
	{
		findField("8")->Hide(false);
		findField("9")->Hide(false);
		findField("10")->Hide(false);
		findField("11")->Hide(false);
		findField("12")->Hide(false);
		findField("13")->Hide(false);
		findField("14")->Hide(false);
		findField("15")->Hide(false);
		findField("8")->setText(paddedName.substr(8, 1));
		findField("9")->setText(paddedName.substr(9, 1));
		findField("10")->setText(paddedName.substr(10, 1));
		findField("11")->setText(paddedName.substr(11, 1));
		findField("12")->setText(paddedName.substr(12, 1));
		findField("13")->setText(paddedName.substr(13, 1));
		findField("14")->setText(paddedName.substr(14, 1));
		findField("15")->setText(paddedName.substr(15, 1));
	}
	else
	{
		findField("8")->Hide(true);
		findField("9")->Hide(true);
		findField("10")->Hide(true);
		findField("11")->Hide(true);
		findField("12")->Hide(true);
		findField("13")->Hide(true);
		findField("14")->Hide(true);
		findField("15")->Hide(true);
	}
}

void NameScreen::typeCharacter(char c)
{
    init();
    auto charWithCasing = static_cast<char>(mpc.getControls().lock()->isShiftPressed() ? toupper(c) : tolower(c));

    if (std::find(mpc::Mpc::akaiAsciiChar.begin(), mpc::Mpc::akaiAsciiChar.end(),
            charWithCasing) == mpc::Mpc::akaiAsciiChar.end())
    {
        return;
    }

    if (editing)
    {
        for (int i = 0; i < 16; i++)
        {
            if (param == std::to_string(i))
            {
                if (i >= name.length())
                {
                    name = StrUtil::padRight(name, " ", i + 1);
                }

                name[i] = charWithCasing;
                displayName();
                drawUnderline();
                if (i <= 14) right();
                break;
            }
        }
    }
    else
    {
        for (int i = 0; i < 16; i++)
        {
            if (param == std::to_string(i))
            {
                if (i >= name.length())
                {
                    name = StrUtil::padRight(name, " ", i + 1);
                }

                name[i] = charWithCasing;

                editing = true;
                initEditColors();
                displayName();
                drawUnderline();
                if (i <= 14) right();
                break;
            }
        }
    }
}

void NameScreen::backSpace()
{
    init();

    for (int i = 1; i < 16; i++)
    {
        if (param == std::to_string(i))
        {
            if (i >= name.length())
            {
                name = StrUtil::padRight(name, " ", i + 1);
            }

            name = name.substr(0, i - 1) + name.substr(i);

            if (!editing)
            {
                editing = true;
                initEditColors();
            }

            displayName();
            drawUnderline();
            left();
        }
    }
}