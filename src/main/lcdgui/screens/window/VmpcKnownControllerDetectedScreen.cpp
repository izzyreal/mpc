#include "lcdgui/screens/window/VmpcKnownControllerDetectedScreen.hpp"

#include "Mpc.hpp"

#include "lcdgui/screens/VmpcMidiScreen.hpp"
#include "lcdgui/Label.hpp"

#include "disk/AbstractDisk.hpp"

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::input::midi;

VmpcKnownControllerDetectedScreen::VmpcKnownControllerDetectedScreen(
    Mpc &m, const int layer)
    : ScreenComponent(m, "vmpc-known-controller-detected", layer)
{
    const auto label0 = std::make_shared<Label>(
        mpc, "line0", "It looks like you have connected", 24, 10, 32 * 6);
    addChild(label0);
    const auto label1 =
        std::make_shared<Label>(mpc, "line1", "", 24, 19, 32 * 6);
    addChild(label1);
    const auto label2 = std::make_shared<Label>(
        mpc, "line2", "Do you want to switch active MIDI", 24, 28, 32 * 6);
    addChild(label2);
    const auto label3 = std::make_shared<Label>(
        mpc, "line3", "mapping to this controller?", 24, 37, 32 * 6);
    addChild(label3);
}

void VmpcKnownControllerDetectedScreen::open()
{
    const auto presetPath =
        mpc.paths->getDocuments()->midiControlPresetsPath() /
        (controllerName + ".json");

    if (!fs::exists(presetPath))
    {
        ls.lock()->openPreviousScreen();
        return;
    }

    const auto preset = std::make_shared<MidiControlPresetV3>();

    mpc.getDisk()->readMidiControlPreset(presetPath, preset);

    if (preset->getAutoLoadMode() == AutoLoadModeNo)
    {
        ls.lock()->openPreviousScreen();
        return;
    }

    if (preset->getAutoLoadMode() == AutoLoadModeYes)
    {
        const auto vmpcMidiScreen =
            mpc.screens->get<ScreenId::VmpcMidiScreen>();
        mpc.getDisk()->readMidiControlPreset(
            mpc.paths->getDocuments()->midiControlPresetsPath() /
                (controllerName + ".json"),
            vmpcMidiScreen->switchToPreset);

        vmpcMidiScreen->shouldSwitch.store(true);
        ls.lock()->openPreviousScreen();
        return;
    }

    displayMessage();
}

void VmpcKnownControllerDetectedScreen::function(const int i)
{
    const auto vmpcMidiScreen = mpc.screens->get<ScreenId::VmpcMidiScreen>();

    switch (i)
    {
        case 1:
            // NO
            ls.lock()->openPreviousScreen();
            break;
        case 2:
            // YES
            mpc.getDisk()->readMidiControlPreset(
                mpc.paths->getDocuments()->midiControlPresetsPath() /
                    (controllerName + ".json"),
                vmpcMidiScreen->switchToPreset);

            vmpcMidiScreen->shouldSwitch.store(true);
            ls.lock()->openPreviousScreen();
            break;
        case 3:
            // NEVER
            // intentional fall-through
        case 4:
        {
            // ALWAYS
            const auto preset = std::make_shared<MidiControlPresetV3>();

            const auto presetPath =
                mpc.paths->getDocuments()->midiControlPresetsPath() /
                (controllerName + ".json");

            mpc.getDisk()->readMidiControlPreset(presetPath, preset);

            preset->setAutoLoadMode(i == 3 ? AutoLoadModeNo : AutoLoadModeYes);

            mpc.getDisk()->writeMidiControlPreset(preset, presetPath);

            if (i == 4)
            {
                vmpcMidiScreen->switchToPreset = preset;
                vmpcMidiScreen->shouldSwitch.store(true);
            }

            ls.lock()->openPreviousScreen();
            break;
        }
        default:
            break;
    }
}

void VmpcKnownControllerDetectedScreen::displayMessage() const
{
    findLabel("line1")->setText(controllerName);
}

void VmpcKnownControllerDetectedScreen::setControllerName(
    const std::string &controllerNameToUse)
{
    controllerName = controllerNameToUse;
}