#pragma once
#include "lcdgui/ScreenComponent.hpp"
#include "lcdgui/screens/OpensNameScreen.hpp"

namespace mpc::lcdgui::screens::window
{

    class MidiOutputScreen final : public ScreenComponent,
                                   public OpensNameScreen
    {

    public:
        void turnWheel(int i) override;
        void function(int i) override;
        void right() override;
        void openNameScreen() override;

    public:
        MidiOutputScreen(Mpc &mpc, int layerIndex);

    public:
        void open() override;

    private:
        int softThru = 0;
        int deviceIndex = 1;
        std::vector<std::string> softThruNames = {"OFF", "AS TRACK", "OMNI-A",
                                                  "OMNI-B", "OMNI-AB"};
        void displaySoftThru() const;
        void displayDeviceName() const;
        void setDeviceIndex(int i);

    public:
        int getSoftThru() const;
        void setSoftThru(int i);
    };
} // namespace mpc::lcdgui::screens::window
