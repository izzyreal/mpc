#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::window
{

    class StepEditOptionsScreen
        : public mpc::lcdgui::ScreenComponent
    {
    public:
        StepEditOptionsScreen(mpc::Mpc &mpc, const int layerIndex);
        void turnWheel(int i) override;
        void open() override;
        void function(int i) override;

        bool isAutoStepIncrementEnabled();
        bool isDurationOfRecordedNotesTcValue();
        int getTcValuePercentage();

        void setAutoStepIncrementEnabled(bool);
        void setDurationOfRecordedNotesTcValue(bool);
        void setTcValueRecordedNotes(int newTcValue);

    private:
        bool autoStepIncrementEnabled = false;
        bool durationOfRecordedNotesTcValue = false;
        int tcValue = 100;

        void displayAutoStepIncrement();
        void displayDurationOfRecordedNotes();
        void displayTcValue();
    };
} // namespace mpc::lcdgui::screens::window