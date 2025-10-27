#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::dialog
{
    class DeleteSongScreen;
    class CopySongScreen;
} // namespace mpc::lcdgui::screens::dialog
namespace mpc::lcdgui::screens::window
{
    class SongWindow;
    class NameScreen;
    class ConvertSongToSeqScreen;
} // namespace mpc::lcdgui::screens::window

namespace mpc::lcdgui::screens
{
    class SongScreen : public mpc::lcdgui::ScreenComponent
    {

    private:
        int offset = -1;
        int activeSongIndex = 0;
        std::string defaultSongName = "Song";

        void setActiveSongIndex(int i);

    public:
        void setOffset(int i);
        void setDefaultSongName(std::string s);
        std::string getDefaultSongName();
        int getOffset();
        int getActiveSongIndex();

    public:
        void up() override;
        void down() override;
        void left() override;
        void right() override;
        void openWindow() override;
        void turnWheel(int i) override;
        void function(int i) override;

        SongScreen(mpc::Mpc &mpc, const int layerIndex);

        void open() override;
        void close() override;
        void update(Observable *, Message message) override;

    private:
        void displayTempo();
        void displayLoop();
        void displaySteps();
        void displayTempoSource();
        void displayNow0();
        void displayNow1();
        void displayNow2();
        void displaySongName();

        friend class mpc::lcdgui::screens::window::SongWindow;
        friend class mpc::lcdgui::screens::dialog::DeleteSongScreen;
        friend class mpc::lcdgui::screens::dialog::CopySongScreen;
        friend class mpc::lcdgui::screens::window::NameScreen;
        friend class mpc::lcdgui::screens::window::ConvertSongToSeqScreen;
    };
} // namespace mpc::lcdgui::screens
