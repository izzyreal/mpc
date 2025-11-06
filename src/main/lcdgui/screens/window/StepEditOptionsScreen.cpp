#include "StepEditOptionsScreen.hpp"

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens::window;

StepEditOptionsScreen::StepEditOptionsScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "step-edit-options", layerIndex)
{
}

void StepEditOptionsScreen::open()
{
    findField("auto-step-increment")->setAlignment(Alignment::Centered);
    displayAutoStepIncrement();
    displayDurationOfRecordedNotes();
    displayTcValue();
}

void StepEditOptionsScreen::function(const int i)
{
    if (i == 4)
    {
        openScreenById(ScreenId::StepEditorScreen);
    }
}

void StepEditOptionsScreen::turnWheel(const int i)
{

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "auto-step-increment")
    {
        autoStepIncrementEnabled = i > 0;
        displayAutoStepIncrement();
    }
    else if (focusedFieldName == "duration-of-recorded-notes")
    {
        durationOfRecordedNotesTcValue = i > 0;
        displayDurationOfRecordedNotes();
        displayTcValue();
    }
    else if (focusedFieldName == "tc-value")
    {
        setTcValueRecordedNotes(tcValue + i);
    }
}

bool StepEditOptionsScreen::isAutoStepIncrementEnabled() const
{
    return autoStepIncrementEnabled;
}

bool StepEditOptionsScreen::isDurationOfRecordedNotesTcValue() const
{
    return durationOfRecordedNotesTcValue;
}

int StepEditOptionsScreen::getTcValuePercentage() const
{
    return tcValue;
}

void StepEditOptionsScreen::setAutoStepIncrementEnabled(const bool b)
{
    autoStepIncrementEnabled = b;
}

void StepEditOptionsScreen::setDurationOfRecordedNotesTcValue(const bool b)
{
    durationOfRecordedNotesTcValue = b;
}

void StepEditOptionsScreen::setTcValueRecordedNotes(const int i)
{
    auto candidate = i;

    if (candidate < 0)
    {
        candidate = 0;
    }
    else if (candidate > 100)
    {
        candidate = 100;
    }

    tcValue = candidate;

    displayTcValue();
}

void StepEditOptionsScreen::displayAutoStepIncrement() const
{
    findField("auto-step-increment")
        ->setText(autoStepIncrementEnabled ? "YES" : "NO");
}

void StepEditOptionsScreen::displayDurationOfRecordedNotes() const
{
    findField("duration-of-recorded-notes")
        ->setText(durationOfRecordedNotesTcValue ? "TC VALUE:" : "AS PLAYED");
}

void StepEditOptionsScreen::displayTcValue() const
{
    const auto f = findField("tc-value");
    f->Hide(!durationOfRecordedNotesTcValue);

    if (durationOfRecordedNotesTcValue)
    {
        f->setTextPadded(std::to_string(tcValue) + "%", " ");
    }
}
