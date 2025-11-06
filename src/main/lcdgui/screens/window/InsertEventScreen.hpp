#pragma once
#include "lcdgui/ScreenComponent.hpp"

namespace mpc::lcdgui::screens::window
{

    class InsertEventScreen final : public ScreenComponent
    {

    public:
        void function(int i) override;
        void turnWheel(int i) override;

        InsertEventScreen(Mpc &mpc, int layerIndex);
        void open() override;
        bool isEventAddedBeforeLeavingTheScreen() const;

    private:
        bool eventAddedBeforeLeavingTheScreen = false;
        std::vector<std::string> eventTypeNames = {
            "NOTE",        "PITCH BEND",    "CONTROL CHANGE", "PROGRAM CHANGE",
            "CH PRESSURE", "POLY PRESSURE", "EXCLUSIVE",      "MIXER"};
        int insertEventType = 0;
        void setInsertEventType(int i);
        void displayInsertEventType() const;

        void insertEvent() const;
    };
} // namespace mpc::lcdgui::screens::window
