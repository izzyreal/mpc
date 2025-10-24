#include "PadAndButtonKeyboard.hpp"

#include "lcdgui/screens/window/NameScreen.hpp"

using namespace mpc::input;
using namespace mpc::hardware;
using namespace mpc::lcdgui::screens::window;

PadAndButtonKeyboard::PadAndButtonKeyboard(mpc::Mpc &mpcToUse) : mpc(mpcToUse)
{
}

std::unordered_map<ComponentId, std::string> PadAndButtonKeyboard::charMap()
{
    static std::unordered_map<ComponentId, std::string> result;

    if (result.empty())
    {
        result = {
            {PAD_1_OR_AB, "AB"},
            {PAD_2_OR_CD, "CD"},
            {PAD_3_OR_EF, "EF"},
            {PAD_4_OR_GH, "GH"},
            {PAD_5_OR_IJ, "IJ"},
            {PAD_6_OR_KL, "KL"},
            {PAD_7_OR_MN, "MN"},
            {PAD_8_OR_OP, "OP"},
            {PAD_9_OR_QR, "QR"},
            {PAD_10_OR_ST, "ST"},
            {PAD_11_OR_UV, "UV"},
            {PAD_12_OR_WX, "WX"},
            {PAD_13_OR_YZ, "YZ"},
            {PAD_14_OR_AMPERSAND_OCTOTHORPE, "&#"},
            {PAD_15_OR_HYPHEN_EXCLAMATION_MARK, "-!"},
            {PAD_16_OR_PARENTHESES, "()"}};
    }

    return result;
}

void PadAndButtonKeyboard::pressHardwareComponent(const ComponentId id)
{
    if (mpc.getLayeredScreen()->getCurrentScreenName() != "name")
    {
        return;
    }

    auto nameScreen = mpc.screens->get<NameScreen>();

    if (id == ComponentId::SIXTEEN_LEVELS_OR_SPACE)
    {
        nameScreen->typeCharacter(' ');
    }
    else if (id == ComponentId::FULL_LEVEL_OR_CASE_SWITCH)
    {
        upperCase = !upperCase;
        return;
    }

    if (charMap().find(id) == charMap().end())
    {
        return;
    }

    bool labelPressedZeroTimes = true;

    if (pressedZeroTimes.find(id) == pressedZeroTimes.end())
    {
        pressedZeroTimes[id] = false;
    }
    else
    {
        labelPressedZeroTimes = pressedZeroTimes[id];
        pressedZeroTimes[id] = !pressedZeroTimes[id];
    }

    char c = charMap()[id][labelPressedZeroTimes ? 0 : 1];

    auto charWithCasing = static_cast<char>(upperCase ? toupper(c) : tolower(c));

    const bool previousPadWasTheSameOne = previousPad == id;
    const bool previousPadWasEmpty = previousPad == ComponentId::NONE;

    if (!previousPadWasTheSameOne && !previousPadWasEmpty)
    {
        nameScreen->right();
    }

    nameScreen->typeCharacter(charWithCasing);

    if (mpc.getLayeredScreen()->getFocusedFieldName() != std::to_string(15))
    {
        // We go back one position because typeCharacter moves one forward
        nameScreen->left();
    }

    previousPad = id;

    nameScreen->SetDirty();
}

void PadAndButtonKeyboard::resetPreviousPad()
{
    previousPad = ComponentId::NONE;
}

void PadAndButtonKeyboard::resetPressedZeroTimes()
{
    pressedZeroTimes.clear();
}

void PadAndButtonKeyboard::resetUpperCase()
{
    upperCase = true;
}
