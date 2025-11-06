#pragma once
#include "Component.hpp"

#include <string>
#include <vector>
#include <thread>

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

        virtual void setText(const std::string &s);

        void setLeftMargin(int margin);
        void enableTwoDots();
        void setBlinking(bool b);
        void setInverted(bool b);
        void setDoubleInverted(bool b);
        void setAlignment(Alignment newAlignment, int endX = -1);
        int getX();
        int getY();
        int getW();
        int getH();
        std::string getText();
        void setTextPadded(const std::string &s,
                           const std::string &padding = " ");
        void setTextPadded(int i, const std::string &padding = " ");
        unsigned int GetTextEntryLength();

        void Draw(std::vector<std::vector<bool>> *pixels) override;
        void setSize(int w, int h) override;

    private:
        Mpc &mpc;
        static constexpr int BLINK_INTERVAL = 300;
        bool blinking = false;
        bool invisibleDueToBlinking = false;
        std::thread blinkThread;
        static void static_blink(void *args);
        void runBlinkThread();
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
        bool isInverted();

    protected:
        std::string text;
    };
} // namespace mpc::lcdgui
