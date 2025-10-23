#include "MidiSwScreen.hpp"
#include "VmpcSettingsScreen.hpp"
#include "lcdgui/screens/window/VmpcWarningSettingsIgnoredScreen.hpp"

using namespace mpc::lcdgui::screens;

MidiSwScreen::MidiSwScreen(mpc::Mpc& mpc, const int layerIndex)
: ScreenComponent(mpc, "midi-sw", layerIndex)
{
    initializeDefaultMapping();
}

void MidiSwScreen::initializeDefaultMapping()
{
    for (int i = 0; i < SWITCH_COUNT; i++)
        controllerToFunctionMapping.push_back({0, 0});
}

std::pair<int, int> MidiSwScreen::getSwitch(int index)
{
    return controllerToFunctionMapping[index];
}

void MidiSwScreen::open()
{
    displaySwitchLabels();
    displayCtrlsAndFunctions();

    auto vmpcSettingsScreen = mpc.screens->get<VmpcSettingsScreen>();

    if (ls->isPreviousScreenNot<VmpcWarningSettingsIgnoredScreen>() &&
        vmpcSettingsScreen->midiControlMode == VmpcSettingsScreen::MidiControlMode::VMPC)
    {
        ls->Draw();
        mpc.getLayeredScreen()->openScreen<VmpcWarningSettingsIgnoredScreen>();
    }
}

void MidiSwScreen::displaySwitchLabels()
{
    for (int i = 0; i < 4; i++)
    {
        auto label = findChild<TextComp>("switch" + std::to_string(i));
        label->setText("Switch " + std::to_string(i + 1 + xOffset));
    }
}

void MidiSwScreen::displayCtrlsAndFunctions()
{
    for (int i = 0; i < 4; i++)
    {
        auto association = controllerToFunctionMapping[i + xOffset];
        
        auto ctrlField = findChild<Field>("ctrl" + std::to_string(i));
        auto functionField = findChild<Field>("function" + std::to_string(i));
        
        auto ctrl = association.first;
        ctrlField->setText(ctrl == 0 ? "OFF" : std::to_string(ctrl - 1));
        
        auto fn = association.second;
        functionField->setText(functionNames[fn]);
    }
}

void MidiSwScreen::setSwitch(const int index, const std::pair<int, int> _switch)
{
    if (_switch.first < 0 || _switch.first > 128
        || _switch.second < 0 || _switch.second >= functionNames.size())
        return;
    
    controllerToFunctionMapping[index] = _switch;
    
    displayCtrlsAndFunctions();
}

void MidiSwScreen::setSwitches(const std::vector<std::pair<int, int>>& _switches)
{
    controllerToFunctionMapping = _switches;
}

void MidiSwScreen::turnWheel(int i)
{

    const auto focusedFieldName = getFocusedFieldNameOrThrow();
    
    const int xPos = stoi(focusedFieldName.substr(focusedFieldName.length() - 1));
    const int selectedSwitch = xOffset + xPos;
    const int yPos = (focusedFieldName.substr(0, 4) == "ctrl") ? 0 : 1;
    auto _switch = controllerToFunctionMapping[selectedSwitch];

    if (yPos == 0)
        _switch.first += i;
    else
        _switch.second += i;
    
    setSwitch(selectedSwitch, _switch);
}

void MidiSwScreen::function(int i)
{
    
    switch (i)
    {
        case 0:
        mpc.getLayeredScreen()->openScreen<SyncScreen>();
            break;
    }
}

void MidiSwScreen::left()
{

    const auto focusedFieldName = getFocusedFieldNameOrThrow();
    
    const int xPos = stoi(focusedFieldName.substr(focusedFieldName.length() - 1));
    
    if (xPos == 0 && xOffset > 0)
    {
        setXOffset(xOffset - 1);
        return;
    }
    
    ScreenComponent::left();
}

void MidiSwScreen::right()
{

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    const int xPos = stoi(focusedFieldName.substr(focusedFieldName.length() - 1));
    
    if (xPos == 3 && xOffset < SWITCH_COUNT - 4)
    {
        setXOffset(xOffset + 1);
        return;
    }
    
    ScreenComponent::right();
}

void MidiSwScreen::setXOffset(int i)
{
    if (i < 0 || i > SWITCH_COUNT - 4)
        return;
    
    xOffset = i;
    
    displaySwitchLabels();
    displayCtrlsAndFunctions();
}
