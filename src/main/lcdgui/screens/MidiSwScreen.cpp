#include "MidiSwScreen.hpp"

using namespace mpc::lcdgui::screens;
using namespace std;

MidiSwScreen::MidiSwScreen(mpc::Mpc& mpc, const int layerIndex)
: ScreenComponent(mpc, "midi-sw", layerIndex)
{
    initializeDefaultMapping();
}

void MidiSwScreen::initializeDefaultMapping()
{
    for (int i = 0; i < SWITCH_COUNT; i++)
        controllerToFunctionMapping.push_back(pair(-1, 0));
}

void MidiSwScreen::open()
{
    displaySwitchLabels();
    displayCtrlsAndFunctions();
}

void MidiSwScreen::displaySwitchLabels()
{
    for (int i = 0; i < 4; i++)
        findChild<TextComp>("switch" + to_string(i)).lock()->setText("Switch " + to_string(i + 1 + xOffset));
}

void MidiSwScreen::displayCtrlsAndFunctions()
{
    for (int i = 0; i < 4; i++)
    {
        auto association = controllerToFunctionMapping[i + xOffset];
        
        auto ctrlField = findChild<Field>("ctrl" + to_string(i)).lock();
        auto functionField = findChild<Field>("function" + to_string(i)).lock();
        
        auto ctrl = association.first;
        ctrlField->setText(ctrl == -1 ? "OFF" : to_string(ctrl));
        
        auto fn = association.second;
        functionField->setText(functionNames[fn]);
    }
}

void MidiSwScreen::setSwitch(const int index, const std::pair<int, int> _switch)
{
    if (_switch.first < -1 || _switch.first > 128
        || _switch.second < 0 || _switch.second >= functionNames.size())
        return;
    
    controllerToFunctionMapping[index] = _switch;
    
    displayCtrlsAndFunctions();
}

void MidiSwScreen::turnWheel(int i)
{
    init();
    
    const int xPos = stoi(param.substr(param.length() - 1));
    const int selectedSwitch = xOffset + xPos;
    const int yPos = (param.substr(0, 4).compare("ctrl") == 0) ? 0 : 1;
    auto _switch = controllerToFunctionMapping[selectedSwitch];

    if (yPos == 0)
        _switch.first += i;
    else
        _switch.second += i;
    
    setSwitch(selectedSwitch, _switch);
}

void MidiSwScreen::function(int i)
{
    init();
    
    switch (i)
    {
        case 0:
            openScreen("sync");
            break;
    }
}

void MidiSwScreen::left()
{
    init();
    
    const int xPos = stoi(param.substr(param.length() - 1));
    
    if (xPos == 0 && xOffset > 0)
    {
        setXOffset(xOffset - 1);
        return;
    }
    
    ScreenComponent::left();
}

void MidiSwScreen::right()
{
    init();

    const int xPos = stoi(param.substr(param.length() - 1));

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
