#pragma once
#include "lcdgui/ScreenComponent.hpp"

namespace mpc::lcdgui::screens
{
    class TrMuteScreen final : public ScreenComponent
    {

    public:
        TrMuteScreen(Mpc &mpc, int layerIndex);

        void open() override;

        void right() override;
        void turnWheel(int i) override;
        void function(int i) override;

    private:
        int bankoffset() const;
        void displayBank() const;
        void displayTrackNumbers() const;
        void displaySq() const;
        void displayTrack(int i) const;
        void setTrackColor(int i) const;
        void displayNow0() const;
        void displayNow1() const;
        void displayNow2() const;
        void refreshTracks() const;
    };
} // namespace mpc::lcdgui::screens
