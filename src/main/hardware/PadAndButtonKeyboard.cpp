#include "PadAndButtonKeyboard.hpp"

#include "lcdgui/screens/window/NameScreen.hpp"

using namespace mpc::hardware;
using namespace mpc::lcdgui::screens::window;

PadAndButtonKeyboard::PadAndButtonKeyboard(mpc::Mpc& mpcToUse) : mpc(mpcToUse)
{
}

std::unordered_map<std::string, std::string> PadAndButtonKeyboard::charMap()
{
    static std::unordered_map<std::string, std::string> result;

    if (result.empty())
    {
        std::string chars = "AB";

        for (int i = 1; i <= 13; i++)
        {
            result["pad-" + std::to_string(i)] = chars;
            chars[0] += 2;
            chars[1] += 2;
        }

        result["pad-14"] = "&#";
        result["pad-15"] = "-!";
        result["pad-16"] = "()";
    }

    return result;
}

void PadAndButtonKeyboard::pressHardwareComponent(const std::string& label)
{
    if (mpc.getLayeredScreen()->getCurrentScreenName() != "name")
    {
        return;
    }

    auto nameScreen = mpc.screens->get<NameScreen>("name");

    if (label == "sixteen-levels")
    {
        nameScreen->typeCharacter(' ');
        return;
    }

    if (label == "full-level")
    {
        upperCase = !upperCase;
        return;
    }

    if (charMap().find(label) == charMap().end())
    {
        return;
    }

    bool labelPressedZeroTimes = true;

    if (pressedZeroTimes.find(label) == pressedZeroTimes.end())
    {
        pressedZeroTimes[label] = false;
    }
    else
    {
        labelPressedZeroTimes = pressedZeroTimes[label];
        pressedZeroTimes[label] = !pressedZeroTimes[label];
    }

    char c = charMap()[label][labelPressedZeroTimes ? 0 : 1];

    auto charWithCasing = static_cast<char>(upperCase ? toupper(c) : tolower(c));

    const bool previousPadWasTheSameOne = previousPad == label;
    const bool previousPadWasEmpty = previousPad.empty();

    if (!previousPadWasTheSameOne && !previousPadWasEmpty)
    {
        nameScreen->right();
    }

    nameScreen->typeCharacter(charWithCasing);

    if (mpc.getLayeredScreen()->getFocus() != std::to_string(15))
    {
        // We go back one position because typeCharacter moves one forward
        nameScreen->left();
    }

    previousPad = label;

    nameScreen->SetDirty();
}

void PadAndButtonKeyboard::resetPreviousPad()
{
    previousPad.clear();
}

void PadAndButtonKeyboard::resetPressedZeroTimes()
{
    pressedZeroTimes.clear();
}