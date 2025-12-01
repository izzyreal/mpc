#pragma once
#include "Component.hpp"

#include <string>
#include <vector>

namespace mpc
{
    class Mpc;
}

namespace mpc::lcdgui
{
    enum class Alignment
    {
        None,
        Centered
    };

    class TextComp : public Component
    {
    public:
        TextComp(Mpc &mpc, const std::string &name);
        ~TextComp() override;

        void onTimerCallback() override;

        virtual void setText(const std::string &s);

        void setLeftMargin(int margin);
        void enableTwoDots();
        void setBlinking(bool b, int intervalMultiplier = 24);
        void setInverted(bool b);
        void setDoubleInverted(bool b);
        void setAlignment(Alignment newAlignment, int endX = -1);
        int getX() const;
        int getY() const;
        int getW() const;
        int getH() const;
        std::string getText();
        void setTextPadded(const std::string &s,
                           const std::string &padding = " ");
        void setTextPadded(int i, const std::string &padding = " ");
        unsigned int GetTextEntryLength() const;

        void Draw(std::vector<std::vector<bool>> *pixels) override;
        void setSize(int w, int h) override;

    private:
        Mpc &mpc;
        int blinkCounter = 0;
        int blinkIntervalMultiplier = 24;
        bool blinking = false;
        bool invisibleDueToBlinking = false;
        Alignment alignment = Alignment::None;
        bool textuallyAligned = false;
        int alignmentEndX = -1;
        bool twoDots = false;
        int leftMargin = 0;

    protected:
        bool inverted = false;
        bool doubleInverted = false;
        const int FONT_HEIGHT = 7;
        const int FONT_WIDTH = 6;
        bool isInverted() const;

        std::string text;
    };
} // namespace mpc::lcdgui
