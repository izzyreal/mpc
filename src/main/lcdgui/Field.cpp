#include "Field.hpp"

#include "Mpc.hpp"
#include "lcdgui/ScreenComponent.hpp"
#include "lcdgui/Rectangle.hpp"

#include "Util.hpp"

#include <StrUtil.hpp>

#include <stdexcept>

#include "Label.hpp"

#ifdef __linux__
#include <climits>
#endif // __linux__

using namespace mpc::lcdgui;

Field::Field(mpc::Mpc &mpc, const std::string &name, int x, int y, int width)
    : TextComp(mpc, name), mpc(mpc)
{
    if (width == 1)
    {
        // We use width 0 as an indicator of wanting to be completely invisible,
        // like dummy fields
        width = 0;
    }

    TextComp::setSize(width, 9);
    setLocation(x - 1, y - 1);
    preDrawClearRect.Clear();
}

void Field::setNextFocus(const std::string &newNextFocus)
{
    nextFocus = newNextFocus;
}

void Field::Hide(bool b)
{
    if (b && focus)
    {
        mpc.getLayeredScreen()->setFocus(nextFocus);
    }

    Component::Hide(b);
}

void Field::Draw(std::vector<std::vector<bool>> *pixels)
{
    if (shouldNotDraw(pixels))
    {
        return;
    }

    auto r = getRect();

    if (split)
    {
        Clear(pixels);
        const auto columns = static_cast<int>(floor((w - 2) / FONT_WIDTH));
        const auto nonInvertedColumns = columns - activeSplit;
        r.R = r.L + (w - (nonInvertedColumns * FONT_WIDTH));
    }

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

            if (typeModeEnabled && j - r.T <= 7)
            {
                (*pixels)[i][j] = !isInverted();
            }
            else
            {
                (*pixels)[i][j] = isInverted();
            }
        }
    }

    const auto oldInverted = inverted;

    if (typeModeEnabled)
    {
        inverted = false;
    }

    TextComp::Draw(pixels);

    if (typeModeEnabled)
    {
        inverted = oldInverted;
        (*pixels)[x][y + FONT_HEIGHT + 1] = false;
    }
}

void Field::takeFocus()
{
    const auto ls = mpc.getLayeredScreen();
    csn = ls->getCurrentScreenName();

    const auto name = getName();

    if (csn == "step-editor")
    {
        if (name == "view")
        {
            const auto screen = ls->getCurrentScreen();
            screen->findField("fromnote")->setInverted(true);
            screen->findField("tonote")->setInverted(true);
            screen->findLabel("tonote")->setInverted(true);
            screen->findChild<Rectangle>("")->setOn(true);
        }
    }
    else if (csn == "multi-recording-setup")
    {
        if (name.length() == 2 && name[0] == 'b')
        {
            setSplit(true);
            setActiveSplit(1);
        }
    }
    else if (csn == "sequencer")
    {
        if (name.find("now") != std::string::npos || name == "tempo")
        {
            Util::initSequence(mpc);
        }
    }

    focus = true;
    inverted = true;
    SetDirty();
}

void Field::loseFocus(const std::string &next)
{
    const auto ls = mpc.getLayeredScreen();
    csn = ls->getCurrentScreenName();

    focus = false;
    setInverted(false);

    if (csn == "step-editor")
    {
        if (getName() == "view")
        {
            const auto screen = ls->getCurrentScreen();
            screen->findChild<Rectangle>("")->setOn(false);

            if (next != "fromnote")
            {
                screen->findField("fromnote")->setInverted(false);
            }

            screen->findField("tonote")->setInverted(false);
            screen->findLabel("tonote")->setInverted(false);
        }
    }

    if (typeModeEnabled)
    {
        disableTypeMode();
    }

    if (split)
    {
        setSplit(false);
    }

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
        if (typeModeEnabled)
        {
            mpc.getScreen()->pressEnter();
        }

        activeSplit = text.length() - 2;
    }
    else
    {
        activeSplit = -1;
    }

    SetDirty();
}

bool Field::isSplit() const
{
    return split;
}

int Field::getActiveSplit() const
{
    return activeSplit;
}

int Field::getSplitIncrement(bool positive) const
{
    static const std::vector<int> splitInc{10000000, 1000000, 100000, 10000,
                                           1000,     100,     10,     1};
    return splitInc[activeSplit] * (positive ? 1 : -1);
}

bool Field::setActiveSplit(int i)
{
    if (i < 0 || i > text.size() - 1)
    {
        return false;
    }

    activeSplit = i;

    SetDirty();
    return true;
}

bool Field::enableTypeMode()
{
    if (typeModeEnabled)
    {
        return false;
    }

    if (split)
    {
        const auto oldActiveSplit = activeSplit;
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
    {
        return value;
    }

    typeModeEnabled = false;

    try
    {
        if (getName() == "tempo")
        {
            // UTF-8 representation of u8"\u00CB", the special dot in a tempo
            // string
            const std::string toReplace = "\xC3\x8B";
            const size_t pos = text.find(toReplace);
            value =
                stoi(std::string(text).replace(pos, toReplace.length(), ""));
        }
        else
        {
            value = stoi(getText());
        }
    }
    catch (std::invalid_argument &e)
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

    if (getName() == "tempo")
    {
        std::string newText;

        if (textCopy.empty())
        {
            newText = u8"\u00CB" + std::to_string(i);
        }
        else
        {
            // UTF-8 representation of u8"\u00CB", the special dot in a tempo
            // string
            const std::string toReplace = "\xC3\x8B";
            const size_t pos = textCopy.find(toReplace);

            if (pos != std::string::npos)
            {
                textCopy.replace(pos, toReplace.length(), "");

                if (textCopy.length() == 4)
                {
                    textCopy.clear();
                }

                if (textCopy == "0" && i == 0)
                {
                    return;
                }

                if (textCopy == "0")
                {
                    textCopy.clear();
                }

                textCopy.append(u8"\u00CB" + std::to_string(i));
            }

            newText = textCopy;
        }

        setText(StrUtil::padLeft(newText, " ", 6));

        return;
    }

    if (textCopy.length() == floor(w / FONT_WIDTH))
    {
        textCopy = "";
    }

    if (textCopy == "0" && i == 0)
    {
        return;
    }

    if (textCopy == "0")
    {
        textCopy = "";
    }

    const auto newText = textCopy.append(std::to_string(i));
    setTextPadded(newText);
}

bool Field::isTypeModeEnabled() const
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
    setText(oldText.c_str());
}

void Field::setFocusable(bool b)
{
    focusable = b;

    if (!focusable && focus)
    {
        mpc.getLayeredScreen()->setFocus(nextFocus);
    }
}

bool Field::isFocusable() const
{
    return focusable;
}

bool Field::hasFocus() const
{
    return focus;
}
