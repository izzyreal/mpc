#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens
{
    class MidiSwScreen : public mpc::lcdgui::ScreenComponent
    {

    public:
        MidiSwScreen(mpc::Mpc &, const int layerIndex);

        void open() override;
        void turnWheel(int i) override;
        void function(int i) override;

        void left() override;
        void right() override;

    private:
        int xOffset = 0;

        void setXOffset(int i);

        void displaySwitchLabels();
        void displayCtrlsAndFunctions();

        void setSwitch(const int index, const std::pair<int, int>);

        void setSwitches(const std::vector<std::pair<int, int>> &);

        void initializeDefaultMapping();
    };
} // namespace mpc::lcdgui::screens
