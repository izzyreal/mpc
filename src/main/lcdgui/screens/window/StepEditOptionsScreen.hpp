#pragma once
#include "lcdgui/ScreenComponent.hpp"

namespace mpc::lcdgui::screens::window
{

    class StepEditOptionsScreen final : public ScreenComponent
    {
    public:
        StepEditOptionsScreen(Mpc &mpc, int layerIndex);
        void turnWheel(int i) override;
        void open() override;
        void function(int i) override;

        bool isAutoStepIncrementEnabled() const;
        bool isDurationOfRecordedNotesTcValue() const;
        int getTcValuePercentage() const;

        void setAutoStepIncrementEnabled(bool);
        void setDurationOfRecordedNotesTcValue(bool);
        void setTcValueRecordedNotes(int newTcValue);

    private:
        bool autoStepIncrementEnabled = false;
        bool durationOfRecordedNotesTcValue = false;
        int tcValue = 100;

        void displayAutoStepIncrement() const;
        void displayDurationOfRecordedNotes() const;
        void displayTcValue() const;
    };
} // namespace mpc::lcdgui::screens::window