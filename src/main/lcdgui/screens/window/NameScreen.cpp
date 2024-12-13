#include "NameScreen.hpp"

#include "hardware/Hardware.hpp"
#include "hardware/PadAndButtonKeyboard.hpp"
#include "hardware/Led.hpp"
#include "hardware/TopPanel.hpp"
#include "lcdgui/Underline.hpp"


using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui;


NameScreen::NameScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "name", layerIndex)
{
	addChildT<Underline>();
}

void NameScreen::mainScreen()
{
    mainScreenAction();
    ScreenComponent::mainScreen();
}

void NameScreen::initialize(std::string name, unsigned char nameLimitToUse,
                             const std::function<void(std::string&)>& enterActionToUse,
                             const std::string& cancelScreenToUse,
                             const std::function<void()>& mainScreenActionToUse)
{
    setName(name);
    setNameLimit(nameLimitToUse);
    enterAction = enterActionToUse;
    cancelScreen = cancelScreenToUse;
    mainScreenAction = mainScreenActionToUse;
}

std::weak_ptr<Underline> NameScreen::findUnderline()
{
	return findChild<Underline>("underline");
}

void NameScreen::open()
{
	for (int i = 0; i < 16; i++)
    {
        findUnderline().lock()->setState(i, false);
    }

	displayName();
}

void NameScreen::close()
{
    ls->setLastFocus("name", "0");
    editing = false;

    enterAction = [](const std::string&) {};
    cancelScreen.clear();
    mainScreenAction = [](){};
	name.clear();
	nameLimit = 16;

    const auto fullLevelEnabled = mpc.getHardware()->getTopPanel()->isFullLevelEnabled();

    mpc.getHardware()->getLed("full-level")->notifyObservers("full-level-" + std::string(fullLevelEnabled ? "on" : "off"));
}

void NameScreen::left()
{
	init();

	if (stoi(param) == 0)
		return;

	ScreenComponent::left();
	
	if (editing)
	{
        mpc.getHardware()->getPadAndButtonKeyboard()->resetPreviousPad();
        findFocus()->setInverted(false);
		drawUnderline();
	}
}

void NameScreen::right()
{
	init();

	if (stoi(param) == nameLimit - 1)
    {
        return;
    }
	
	ScreenComponent::right();
	
	if (editing)
	{
        mpc.getHardware()->getPadAndButtonKeyboard()->resetPreviousPad();
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
        openScreen(cancelScreen);
		break;
	}
	case 4:
        auto newName = getNameWithoutSpaces();
        if (newName.empty())
        {
            return;
        }
        enterAction(newName);
		break;
    }
}

void NameScreen::pressEnter()
{
    auto newName = getNameWithoutSpaces();

    if (newName.empty())
    {
        return;
    }

    enterAction(newName);
}

void NameScreen::drawUnderline()
{
	if (editing)
	{
        std::string focus = ls->getFocus();
	
		if (focus.length() != 1 && focus.length() != 2)
        {
            return;
        }
		
		auto u = findUnderline().lock();
		
		for (int i = 0; i < 16; i++)
        {
            u->setState(i, i == stoi(focus));
        }

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
    nameScreenName = newName;
	nameLimit = 16;
}

void NameScreen::setNameLimit(int i)
{
	nameScreenName = nameScreenName.substr(0, i);
	nameLimit = i;
}

void NameScreen::setName(std::string str, int i)
{
	nameScreenName[i] = str[0];
}

std::string NameScreen::getNameWithoutSpaces()
{
	auto s = nameScreenName;

	while (!s.empty() && isspace(s.back()))
    {
        s.pop_back();
    }

	for (int i = 0; i < s.length(); i++)
    {
        if (s[i] == ' ') s[i] = '_';
    }

    return s;
}

void NameScreen::changeNameCharacter(int i, bool up)
{
    if (i >= nameScreenName.length())
    {
        nameScreenName = StrUtil::padRight(nameScreenName, " ", i + 1);
    }
        
	char schar = nameScreenName[i];
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

    nameScreenName = nameScreenName.substr(0, i).append(s).append(nameScreenName.substr(i + 1, nameScreenName.length()));
    displayName();
}

void NameScreen::displayName()
{
	if (nameLimit == 0)
    {
        return;
    }

    auto paddedName = StrUtil::padRight(nameScreenName, " ", nameLimit);
    
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

    if (std::find(mpc::Mpc::akaiAsciiChar.begin(), mpc::Mpc::akaiAsciiChar.end(),
            c) == mpc::Mpc::akaiAsciiChar.end())
    {
        return;
    }

    if (editing)
    {
        for (int i = 0; i < 16; i++)
        {
            if (param == std::to_string(i))
            {
                if (i >= nameScreenName.length())
                {
                    nameScreenName = StrUtil::padRight(nameScreenName, " ", i + 1);
                }

                nameScreenName[i] = c;
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
                if (i >= nameScreenName.length())
                {
                    nameScreenName = StrUtil::padRight(nameScreenName, " ", i + 1);
                }

                nameScreenName[i] = c;

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
            if (i >= nameScreenName.length())
            {
                nameScreenName = StrUtil::padRight(nameScreenName, " ", i + 1);
            }

            nameScreenName = nameScreenName.substr(0, i - 1) + nameScreenName.substr(i);

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

void NameScreen::setEditing(bool b)
{
    editing = b;
}
