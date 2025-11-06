#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::window
{

    class TimeDisplayScreen : public mpc::lcdgui::ScreenComponent
    {

    public:
        void turnWheel(int i) override;

    public:
        TimeDisplayScreen(mpc::Mpc &mpc, int layerIndex);

    public:
        void open() override;

    private:
        void displayDisplayStyle();
        void displayStartTime();
        void displayFrameRate();

    public:
        void setFrameRate(int i);
        void setDisplayStyle(int i);
        void setHours(int i);
        void setMinutes(int i);
        void setSeconds(int i);
        void setFrames(int i);
        void setFrameDecimals(int i);
        int getDisplayStyle();

    private:
        std::vector<std::string> displayStyleNames{"BAR,BEAT,CLOCK",
                                                   "HOUR,MINUTE,SEC"};
        std::vector<std::string> frameRateNames = {"24", "25", "30D", "30"};
        int displayStyle = 0;
        int frameRate = 0;
    };
} // namespace mpc::lcdgui::screens::window
