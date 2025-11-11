#pragma once

#include "TextComp.hpp"

#include <string>
#include <vector>

namespace mpc
{
    class Mpc;
}

namespace mpc::lcdgui
{
    class Label;
}

namespace mpc::lcdgui
{
    class Field : public TextComp
    {

        Mpc &mpc;
        // When hiding a field that is focused, we pass focus to nextFocus
        std::string nextFocus = "_";

        bool focusable = true;
        bool focus = false;
        std::string csn;
        bool split = false;
        int activeSplit = 0;
        bool typeModeEnabled = false;
        std::string oldText;
        bool scrolling = false;

    public:
        bool hasFocus() const;
        void setFocusable(bool b);
        bool isFocusable() const;
        void loseFocus(const std::string &next);
        void takeFocus();
        void setSplit(bool b);
        bool isSplit() const;
        int getSplitIncrement(bool positive) const;
        int getActiveSplit() const;
        bool setActiveSplit(int i);
        bool enableTypeMode();
        int enter();
        void type(int i);
        bool isTypeModeEnabled() const;
        void disableTypeMode();
        void setNextFocus(const std::string &newNextFocus);

        void Draw(std::vector<std::vector<bool>> *pixels) override;
        void Hide(bool b) override;

        Field(Mpc &mpc, const std::string &name, int x, int y, int width);
    };
} // namespace mpc::lcdgui
