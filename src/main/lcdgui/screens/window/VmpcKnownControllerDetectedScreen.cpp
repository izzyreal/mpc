#include "VmpcKnownControllerDetectedScreen.hpp"
#include "lcdgui/screens/VmpcMidiScreen.hpp"

#include "disk/AbstractDisk.hpp"

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens::window;

VmpcKnownControllerDetectedScreen::VmpcKnownControllerDetectedScreen(mpc::Mpc &m, int layer)
    : ScreenComponent(m, "vmpc-known-controller-detected", layer)
{
    auto label0 = std::make_shared<Label>(mpc, "line0", "It looks like you have connected", 24, 10, 32 * 6);
    addChild(label0);
    auto label1 = std::make_shared<Label>(mpc, "line1", "", 24, 19, 32 * 6);
    addChild(label1);
    auto label2 = std::make_shared<Label>(mpc, "line2", "Do you want to switch active MIDI", 24, 28, 32 * 6);
    addChild(label2);
    auto label3 = std::make_shared<Label>(mpc, "line3", "mapping to this controller?", 24, 37, 32 * 6);
    addChild(label3);
}

void VmpcKnownControllerDetectedScreen::function(int i)
{
    auto vmpcMidiScreen = mpc.screens->get<VmpcMidiScreen>();
    auto &presets = mpc.midiControlPresets;
    auto preset = std::find_if(
        presets.begin(),
        presets.end(),
        [this](const std::shared_ptr<nvram::MidiControlPreset> &p)
        {
            return controllerName.find(p->name) != std::string::npos;
        });

    switch (i)
    {
    case 1:
        // NO
        mpc.getLayeredScreen()->closeCurrentScreen();
        break;
    case 2:
        // YES
        vmpcMidiScreen->shouldSwitch.store(true);
        mpc.getLayeredScreen()->closeCurrentScreen();
        break;
    case 3:
        // NEVER
        if (preset != presets.end())
        {
            (*preset)->autoloadMode = nvram::MidiControlPreset::AutoLoadMode::AutoLoadModeNo;
        }
        mpc.getLayeredScreen()->closeCurrentScreen();
        break;
    case 4:
        // ALWAYS
        if (preset != presets.end())
        {
            if ((*preset)->autoloadMode != nvram::MidiControlPreset::AutoLoadMode::AutoLoadModeYes)
            {
                (*preset)->autoloadMode = nvram::MidiControlPreset::AutoLoadMode::AutoLoadModeYes;
                mpc.getDisk()->writeMidiControlPreset(*preset);
                nvram::MidiControlPersistence::loadAllPresetsFromDiskIntoMemory(mpc);
            }
        }
        vmpcMidiScreen->shouldSwitch.store(true);
        mpc.getLayeredScreen()->closeCurrentScreen();
        break;
    default:
        break;
    }
}

void VmpcKnownControllerDetectedScreen::displayMessage()
{
    findLabel("line1")->setText(controllerName);
}

void VmpcKnownControllerDetectedScreen::open()
{
    for (auto &p : mpc.midiControlPresets)
    {
        if (controllerName.find(p->name) != std::string::npos)
        {
            if (p->autoloadMode == nvram::MidiControlPreset::AutoLoadMode::AutoLoadModeNo)
            {
                mpc.getLayeredScreen()->closeCurrentScreen();
                return;
            }
            else if (p->autoloadMode == nvram::MidiControlPreset::AutoLoadMode::AutoLoadModeYes)
            {
                auto vmpcMidiScreen = mpc.screens->get<VmpcMidiScreen>();
                vmpcMidiScreen->shouldSwitch.store(true);
                mpc.getLayeredScreen()->closeCurrentScreen();
                return;
            }
            break;
        }
    }

    displayMessage();
}
