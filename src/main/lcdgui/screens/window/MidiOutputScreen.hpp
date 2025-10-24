#pragma once
#include <lcdgui/ScreenComponent.hpp>
#include "lcdgui/screens/OpensNameScreen.hpp"

namespace mpc::lcdgui::screens::window
{

    class MidiOutputScreen
        : public mpc::lcdgui::ScreenComponent,
          public mpc::lcdgui::screens::OpensNameScreen
    {

    public:
        void turnWheel(int i) override;
        void function(int i) override;
        void right() override;
        void openNameScreen() override;

    public:
        MidiOutputScreen(mpc::Mpc &mpc, const int layerIndex);

    public:
        void open() override;

    private:
        int softThru = 0;
        int deviceIndex = 1;
        std::vector<std::string> softThruNames = {"OFF", "AS TRACK", "OMNI-A", "OMNI-B", "OMNI-AB"};
        void displaySoftThru();
        void displayDeviceName();
        void setDeviceIndex(int i);

    public:
        int getSoftThru();
        void setSoftThru(int i);
    };
} // namespace mpc::lcdgui::screens::window
