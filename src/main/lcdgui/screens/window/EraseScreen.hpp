#pragma once
#include <lcdgui/ScreenComponent.hpp>
#include <lcdgui/screens/WithTimesAndNotes.hpp>

namespace mpc::lcdgui::screens::window
{
    class EraseScreen
        : public mpc::lcdgui::ScreenComponent,
          public mpc::lcdgui::screens::WithTimesAndNotes
    {

    public:
        void turnWheel(int i) override;
        void function(int i) override;

    public:
        EraseScreen(mpc::Mpc &mpc, const int layerIndex);

    public:
        void open() override;

    private:
        void displayTrack();
        void displayErase();
        void displayType();

    protected:
        void displayNotes() override;
        void displayTime() override;

    private:
        const std::vector<std::string> eventTypes{"note-on", "pitch-bend", "control-change", "program-change", "channel-pressure", "poly-pressure", "system-exclusive"};
        const std::vector<std::string> typeNames{"NOTES", "PITCH BEND", "CONTROL", "PROG CHANGE", "CH PRESSURE", "POLY PRESS", "EXCLUSIVE"};
        const std::vector<std::string> eraseNames{"ALL EVENTS", "ALL EXCEPT", "ONLY ERASE"};

        int track = 0;
        int erase = 0;
        int type = 0;

        void setTrack(int i);
        void setErase(int i);
        void setType(int i);

        void doErase();
    };
} // namespace mpc::lcdgui::screens::window
