#pragma once
#include "lcdgui/ScreenComponent.hpp"

namespace mpc::lcdgui::screens::window
{

    class TimeDisplayScreen final : public ScreenComponent
    {

    public:
        void turnWheel(int i) override;

    public:
        TimeDisplayScreen(Mpc &mpc, int layerIndex);

    public:
        void open() override;

    private:
        void displayDisplayStyle() const;
        void displayStartTime() const;
        void displayFrameRate() const;

    public:
        void setFrameRate(int i);
        void setDisplayStyle(int i);
        void setHours(int i) const;
        void setMinutes(int i) const;
        void setSeconds(int i) const;
        void setFrames(int i) const;
        void setFrameDecimals(int i) const;
        int getDisplayStyle() const;

    private:
        std::vector<std::string> displayStyleNames{"BAR,BEAT,CLOCK",
                                                   "HOUR,MINUTE,SEC"};
        std::vector<std::string> frameRateNames = {"24", "25", "30D", "30"};
        int displayStyle = 0;
        int frameRate = 0;
    };
} // namespace mpc::lcdgui::screens::window
