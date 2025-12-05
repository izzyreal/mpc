#pragma once
#include "lcdgui/ScreenComponent.hpp"

namespace mpc::lcdgui::screens::window
{
    class NameScreen;
}

namespace mpc::lcdgui::screens
{
    class SongScreen final : public ScreenComponent
    {
    public:
        SongScreen(Mpc &mpc, int layerIndex);

        void open() override;
        void up() override;
        void down() override;
        void left() override;
        void right() override;
        void openWindow() override;
        void turnWheel(int increment) override;
        void function(int i) override;

        void setDefaultSongName(const std::string &s);
        std::string getDefaultSongName();

    private:
        std::string defaultSongName = "Song";

        void displayTempo() const;
        void displayLoop() const;
        void displaySteps() const;
        void displayTempoSource() const;
        void displayNow0() const;
        void displayNow1() const;
        void displayNow2() const;
        void displaySongName() const;

        friend class NameScreen;
    };
} // namespace mpc::lcdgui::screens
