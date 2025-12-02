#pragma once
#include "lcdgui/ScreenComponent.hpp"
#include "lcdgui/screens/WithTimesAndNotes.hpp"

namespace mpc::lcdgui::screens::window
{
    class EraseScreen final : public ScreenComponent, public WithTimesAndNotes
    {
    public:
        void turnWheel(int i) override;
        void function(int i) override;

        EraseScreen(Mpc &mpc, int layerIndex);

        void open() override;

    private:
        void displayTrack() const;
        void displayErase() const;
        void displayType() const;

    protected:
        void displayNotes() override;
        void displayTime() override;

    private:
        const std::vector<std::string> eventTypes{
            "note-on",         "pitch-bend",       "control-change",
            "program-change",  "channel-pressure", "poly-pressure",
            "system-exclusive"};
        const std::vector<std::string> typeNames{
            "NOTES",       "PITCH BEND", "CONTROL",  "PROG CHANGE",
            "CH PRESSURE", "POLY PRESS", "EXCLUSIVE"};
        const std::vector<std::string> eraseNames{"ALL EVENTS", "ALL EXCEPT",
                                                  "ONLY ERASE"};

        int track = 0;
        int erase = 0;
        int type = 0;

        void setTrack(int i);
        void setErase(int i);
        void setType(int i);

        void doErase() const;
    };
} // namespace mpc::lcdgui::screens::window
