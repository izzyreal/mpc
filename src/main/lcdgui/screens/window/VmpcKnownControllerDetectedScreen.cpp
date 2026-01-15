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

void VmpcKnownControllerDetectedScreen::function(const int i)
{
    const auto vmpcMidiScreen = mpc.screens->get<ScreenId::VmpcMidiScreen>();

    switch (i)
    {
        case 1:
            // NO
            mpc.getLayeredScreen()->closeCurrentScreen();
            break;
        case 2:
            // YES
            mpc.getDisk()->readMidiControlPreset(
                mpc.paths->getDocuments()->midiControlPresetsPath() /
                    (controllerName + ".json"),
                vmpcMidiScreen->switchToPreset);

            vmpcMidiScreen->shouldSwitch.store(true);
            mpc.getLayeredScreen()->closeCurrentScreen();
            break;
        case 3:
        {
            auto preset = std::make_shared<MidiControlPresetV3>();

            mpc.getDisk()->readMidiControlPreset(
                mpc.paths->getDocuments()->midiControlPresetsPath() /
                    (controllerName + ".json"),
                preset);

//            preset->setAutoLoad();

            mpc.getLayeredScreen()->closeCurrentScreen();
            break;
        }
            // NEVER
            //            if (preset != presets.end())
            //            {
            //                (*preset)->autoloadMode =
            //                AutoLoadMode::AutoLoadModeNo;
            //            }
        case 4:
            // ALWAYS
            //            if (preset != presets.end())
            //            {
            //                if ((*preset)->autoloadMode !=
            //                AutoLoadMode::AutoLoadModeYes)
            //                {
            //                    (*preset)->autoloadMode =
            //                    AutoLoadMode::AutoLoadModeYes;
            //                    mpc.getDisk()->writeMidiControlPreset(*preset);
            //                    legacy::MidiControlPersistence::
            //                        loadAllPresetsFromDiskIntoMemory(mpc);
            //                }
            //            }

            mpc.getDisk()->readMidiControlPreset(
                mpc.paths->getDocuments()->midiControlPresetsPath() /
                    (controllerName + ".json"),
                vmpcMidiScreen->switchToPreset);

            vmpcMidiScreen->shouldSwitch.store(true);
            mpc.getLayeredScreen()->closeCurrentScreen();
            break;
        default:
            break;
    }
}

void VmpcKnownControllerDetectedScreen::displayMessage() const
{
    findLabel("line1")->setText(controllerName);
}

void VmpcKnownControllerDetectedScreen::open()
{
    //    for (const auto &p : mpc.midiControlPresets)
    //    {
    //        if (controllerName.find(p->name) != std::string::npos)
    //        {
    //            if (p->autoloadMode ==
    //                legacy::MidiControlPreset::AutoLoadMode::AutoLoadModeNo)
    //            {
    //                mpc.getLayeredScreen()->closeCurrentScreen();
    //                return;
    //            }
    //            if (p->autoloadMode ==
    //                legacy::MidiControlPreset::AutoLoadMode::AutoLoadModeYes)
    //            {
    //                const auto vmpcMidiScreen =
    //                    mpc.screens->get<ScreenId::VmpcMidiScreen>();
    //                vmpcMidiScreen->shouldSwitch.store(true);
    //                mpc.getLayeredScreen()->closeCurrentScreen();
    //                return;
    //            }
    //            break;
    //        }
    //    }

    displayMessage();
}

void VmpcKnownControllerDetectedScreen::setControllerName(
    const std::string &controllerNameToUse)
{
    controllerName = controllerNameToUse;
}