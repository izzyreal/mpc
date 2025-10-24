#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens
{
    class TrMuteScreen
        : public mpc::lcdgui::ScreenComponent
    {

    public:
        void right() override;
        void turnWheel(int i) override;
        void function(int i) override;

    public:
        TrMuteScreen(mpc::Mpc &mpc, const int layerIndex);
        void update(Observable *o, Message message) override;
        void open() override;
        void close() override;

    private:
        int bankoffset();
        void displayBank();
        void displayTrackNumbers();
        void displaySq();
        void displayTrack(int i);
        void setTrackColor(int i);
        void displayNow0();
        void displayNow1();
        void displayNow2();
        void refreshTracks();
    };
} // namespace mpc::lcdgui::screens
