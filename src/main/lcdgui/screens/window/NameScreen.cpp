#include "NameScreen.hpp"

#include "input/PadAndButtonKeyboard.hpp"
#include "lcdgui/Underline.hpp"
#include "hardware/Hardware.hpp"

using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui;
using namespace mpc::hardware;

NameScreen::NameScreen(mpc::Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "name", layerIndex)
{
    addChildT<Underline>();
}

void NameScreen::initialize(std::string newNameToEdit, unsigned char nameLimitToUse,
                            const std::function<void(std::string &)> &enterActionToUse,
                            const std::string &cancelScreenToUse,
                            const std::function<void()> &mainScreenActionToUse)
{
    setNameToEdit(newNameToEdit);
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
    for (auto &f : findFields())
    {
        f->loseFocus("");
    }

    findField("0")->takeFocus();

    mpc.getHardware()->getLed(ComponentId::FULL_LEVEL_OR_CASE_SWITCH_LED)->setEnabled(!mpc.getPadAndButtonKeyboard()->isUpperCase());

    for (int i = 0; i < 16; i++)
    {
        findUnderline().lock()->setState(i, false);
    }

    displayName();
}

void NameScreen::close()
{
    mpc.getHardware()->getLed(ComponentId::FULL_LEVEL_OR_CASE_SWITCH_LED)->setEnabled(mpc.isFullLevelEnabled());

    ls->setLastFocus("name", "0");
    editing = false;

    enterAction = [](const std::string &) {};
    cancelScreen.clear();
    mainScreenAction = []() {};
    nameToEdit.clear();
    nameLimit = 16;
}

void NameScreen::left()
{

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (stoi(focusedFieldName) == 0)
    {
        return;
    }

    ScreenComponent::left();

    if (editing)
    {
        mpc.getPadAndButtonKeyboard()->resetPreviousPad();
        auto focus = findFocus();
        focus->setInverted(false);
        drawUnderline();
    }
}

void NameScreen::right()
{

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (stoi(focusedFieldName) == nameLimit - 1)
    {
        return;
    }

    ScreenComponent::right();

    if (editing)
    {
        mpc.getPadAndButtonKeyboard()->resetPreviousPad();
        auto focus = findFocus();
        printf("New focus: %s\n", focus->getName().c_str());
        focus->setInverted(false);
        drawUnderline();
    }
}

void NameScreen::turnWheel(int j)
{

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (editing)
    {
        for (int i = 0; i < 16; i++)
        {
            if (focusedFieldName == std::to_string(i))
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
            if (focusedFieldName == std::to_string(i))
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

    switch (i)
    {
    case 3:
    {
        mpc.getLayeredScreen()->openScreen(cancelScreen);
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
        std::string focus = ls->getFocusedFieldName();

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

    const auto focusedField = getFocusedFieldOrThrow();
    focusedField->setInverted(false);
}

void NameScreen::setNameToEdit(std::string newNameToEdit)
{
    nameToEdit = newNameToEdit;
    nameLimit = 16;
}

void NameScreen::setNameLimit(int i)
{
    nameToEdit = nameToEdit.substr(0, i);
    nameLimit = i;
}

void NameScreen::setNameToEdit(std::string str, int i)
{
    nameToEdit[i] = str[0];
}

std::string NameScreen::getNameWithoutSpaces()
{
    auto s = nameToEdit;

    while (!s.empty() && isspace(s.back()))
    {
        s.pop_back();
    }

    for (int i = 0; i < s.length(); i++)
    {
        if (s[i] == ' ')
        {
            s[i] = '_';
        }
    }

    return s;
}

void NameScreen::changeNameCharacter(int i, bool up)
{
    if (i >= nameToEdit.length())
    {
        nameToEdit = StrUtil::padRight(nameToEdit, " ", i + 1);
    }

    char schar = nameToEdit[i];
    std::string s{schar};
    auto stringCounter = 0;

    for (auto str : mpc::Mpc::akaiAscii)
    {
        if (str == s)
        {
            break;
        }

        stringCounter++;
    }

    if (stringCounter == 0 && !up)
    {
        return;
    }

    if (stringCounter == 75 && up)
    {
        return;
    }

    auto change = -1;

    if (up)
    {
        change = 1;
    }

    if (stringCounter > 75)
    {
        s = " ";
    }
    else
    {
        s = mpc::Mpc::akaiAscii[stringCounter + change];
    }

    nameToEdit = nameToEdit.substr(0, i).append(s).append(nameToEdit.substr(i + 1, nameToEdit.length()));
    displayName();
}

void NameScreen::displayName()
{
    if (nameLimit == 0)
    {
        return;
    }

    auto paddedName = StrUtil::padRight(nameToEdit, " ", nameLimit);

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

    if (std::find(mpc::Mpc::akaiAsciiChar.begin(), mpc::Mpc::akaiAsciiChar.end(),
                  c) == mpc::Mpc::akaiAsciiChar.end())
    {
        return;
    }

    const auto focusedFieldName = getFocusedFieldName();

    if (editing)
    {
        for (int i = 0; i < 16; i++)
        {
            if (focusedFieldName == std::to_string(i))
            {
                if (i >= nameToEdit.length())
                {
                    nameToEdit = StrUtil::padRight(nameToEdit, " ", i + 1);
                }

                nameToEdit[i] = c;
                displayName();
                drawUnderline();
                if (i <= 14)
                {
                    right();
                }
                break;
            }
        }
    }
    else
    {
        for (int i = 0; i < 16; i++)
        {
            if (focusedFieldName == std::to_string(i))
            {
                if (i >= nameToEdit.length())
                {
                    nameToEdit = StrUtil::padRight(nameToEdit, " ", i + 1);
                }

                nameToEdit[i] = c;

                editing = true;
                initEditColors();
                displayName();
                drawUnderline();
                if (i <= 14)
                {
                    right();
                }
                break;
            }
        }
    }
}

void NameScreen::backSpace()
{

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    for (int i = 1; i < 16; i++)
    {
        if (focusedFieldName == std::to_string(i))
        {
            if (i >= nameToEdit.length())
            {
                nameToEdit = StrUtil::padRight(nameToEdit, " ", i + 1);
            }

            nameToEdit = nameToEdit.substr(0, i - 1) + nameToEdit.substr(i);

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
