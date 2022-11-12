#include "StepEditOptionsScreen.hpp"

#include "lang/StrUtil.hpp"

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens::window;

StepEditOptionsScreen::StepEditOptionsScreen(mpc::Mpc &mpc, const int layerIndex)
: mpc::lcdgui::ScreenComponent(mpc, "step-edit-options", layerIndex)
{
}

void StepEditOptionsScreen::open()
{
    findField("auto-step-increment")->setAlignment(Alignment::Centered);
    displayAutoStepIncrement();
    displayDurationOfRecordedNotes();
    displayTcValue();
}

void StepEditOptionsScreen::function(int i)
{
    if (i == 4)
    {
        openScreen("step-editor");
    }
}

void StepEditOptionsScreen::turnWheel(int i)
{
    init();
    if (param == "auto-step-increment")
    {
        autoStepIncrementEnabled = i > 0;
        displayAutoStepIncrement();
    }
    else if (param == "duration-of-recorded-notes")
    {
        durationOfRecordedNotesTcValue = i > 0;
        displayDurationOfRecordedNotes();
        displayTcValue();
    }
    else if (param == "tc-value")
    {
        setTcValueRecordedNotes(tcValue + i);
    }
}

bool StepEditOptionsScreen::isAutoStepIncrementEnabled()
{
    return autoStepIncrementEnabled;
}

bool StepEditOptionsScreen::isDurationOfRecordedNotesTcValue()
{
    return durationOfRecordedNotesTcValue;
}

int StepEditOptionsScreen::getTcValuePercentage()
{
    return tcValue;
}

void StepEditOptionsScreen::setAutoStepIncrementEnabled(bool b)
{
    autoStepIncrementEnabled = b;
}

void StepEditOptionsScreen::setDurationOfRecordedNotesTcValue(bool b)
{
    durationOfRecordedNotesTcValue = b;
}

void StepEditOptionsScreen::setTcValueRecordedNotes(int i)
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

void StepEditOptionsScreen::displayAutoStepIncrement()
{
    findField("auto-step-increment")->setText(autoStepIncrementEnabled ? "YES" : "NO");
}

void StepEditOptionsScreen::displayDurationOfRecordedNotes()
{
    findField("duration-of-recorded-notes")->setText(durationOfRecordedNotesTcValue ? "TC VALUE:" : "AS PLAYED");
}

void StepEditOptionsScreen::displayTcValue()
{
    auto f = findField("tc-value");
    f->Hide(!durationOfRecordedNotesTcValue);

    if (durationOfRecordedNotesTcValue)
    {
        f->setTextPadded(std::to_string(tcValue) + "%", " ");
    }
}
