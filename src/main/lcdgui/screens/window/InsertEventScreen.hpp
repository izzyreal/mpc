#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::window
{

    class InsertEventScreen
        : public mpc::lcdgui::ScreenComponent
    {

    public:
        void function(int i) override;
        void turnWheel(int i) override;

        InsertEventScreen(mpc::Mpc &mpc, const int layerIndex);
        void open() override;
        bool isEventAddedBeforeLeavingTheScreen();

    private:
        bool eventAddedBeforeLeavingTheScreen = false;
        std::vector<std::string> eventTypeNames = {"NOTE", "PITCH BEND", "CONTROL CHANGE", "PROGRAM CHANGE", "CH PRESSURE", "POLY PRESSURE", "EXCLUSIVE", "MIXER"};
        int insertEventType = 0;
        void setInsertEventType(int i);
        void displayInsertEventType();

        void insertEvent();
    };
} // namespace mpc::lcdgui::screens::window
