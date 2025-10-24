#include "VmpcDisksScreen.hpp"
#include "VmpcSettingsScreen.hpp"

#include "lcdgui/Parameter.hpp"
#include "lcdgui/Screens.hpp"
#include "disk/DiskController.hpp"
#include "disk/AbstractDisk.hpp"
#include "disk/RawDisk.hpp"
#include "disk/StdDisk.hpp"
#include "nvram/VolumesPersistence.hpp"

#include <StrUtil.hpp>

using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::dialog2;
using namespace mpc::lcdgui;
using namespace mpc::disk;
using namespace mpc::nvram;

VmpcDisksScreen::VmpcDisksScreen(mpc::Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "vmpc-disks", layerIndex)
{
    for (int i = 0; i < 4; i++)
    {
        const int y = 11 + (i * 9);
        auto volumeLabel = std::make_shared<Label>(mpc, "volume" + std::to_string(i), "", 2, y, 11 * 6);
        auto typeLabel = std::make_shared<Label>(mpc, "type" + std::to_string(i), "", 74, y, 3 * 6);
        auto sizeLabel = std::make_shared<Label>(mpc, "size" + std::to_string(i), "", 104, y, 4 * 6);
        auto modeParam = std::make_shared<Parameter>(mpc, "", "mode" + std::to_string(i), 131, y + 1, 10 * 6);

        addChild(volumeLabel);
        addChild(typeLabel);
        addChild(sizeLabel);
        addChild(modeParam);
    }
}

void VmpcDisksScreen::open()
{
    findChild<Label>("up")->setText("\u00C7");
    findChild<Label>("down")->setText("\u00C6");

    mpc.getDiskController()->detectRawUsbVolumes();

    refreshConfig();
    displayFunctionKeys();
}

void VmpcDisksScreen::function(int i)
{
    switch (i)
    {
    case 0:
        mpc.getLayeredScreen()->openScreen<VmpcSettingsScreen>();
        break;
    case 1:
        mpc.getLayeredScreen()->openScreen<VmpcKeyboardScreen>();
        break;
    case 2:
        mpc.getLayeredScreen()->openScreen<VmpcAutoSaveScreen>();
        break;
    case 4:
    {
        auto vmpcSettingsScreen = mpc.screens->get<VmpcSettingsScreen>();

        if (vmpcSettingsScreen->getMidiControlMode() == VmpcSettingsScreen::MidiControlMode::ORIGINAL)
        {
            return;
        }

        mpc.getLayeredScreen()->openScreen<VmpcMidiScreen>();
        break;
    }
    case 5:
    {
        std::string popupMsg;

        if (hasConfigChanged())
        {
            for (auto &kv : config)
            {
                auto uuid = kv.first;
                for (auto &d : mpc.getDisks())
                {
                    if (d->getVolume().volumeUUID == uuid)
                    {
                        d->getVolume().mode = kv.second;
                    }
                }
            }

            VolumesPersistence::save(mpc);
            popupMsg = "Volume configurations saved";
        }
        else
        {
            popupMsg = "Volume configurations unchanged";
        }

        ls->showPopupForMs(popupMsg, 1000);
        break;
    }
    }
}

void VmpcDisksScreen::turnWheel(int i)
{
    auto &volume = mpc.getDisks()[row]->getVolume();

    if (volume.volumeUUID == "default_volume")
    {
        return;
    }

    auto current = config[volume.volumeUUID];
    if (current + i < 0 || current + i > 2)
    {
        return;
    }
    config[volume.volumeUUID] = static_cast<MountMode>(current + i);

    displayRows();
    displayFunctionKeys();
}

void VmpcDisksScreen::displayRows()
{
    auto disks = mpc.getDisks();

    for (int i = 0; i < 4; i++)
    {
        auto volume = findChild<Label>("volume" + std::to_string(i));
        auto type = findChild<Label>("type" + std::to_string(i));
        auto size = findChild<Label>("size" + std::to_string(i));
        auto mode = findChild<Field>("mode" + std::to_string(i));

        mode->setInverted(i == row);

        if (i >= disks.size())
        {
            volume->setText("");
            type->setText("");
            size->setText("");
            mode->setText("");
            continue;
        }

        auto disk = disks[i];

        volume->setText(disk->getVolumeLabel());
        type->setText(disk->getTypeShortName());
        size->setText(byte_count_to_short_string(disk->getTotalSize(), /*one_letter_suffix = */ true));
        mode->setText(Volume::modeShortName(config[disk->getVolume().volumeUUID]));
    }

    displayUpAndDown();
}

void VmpcDisksScreen::up()
{
    if (row == 0 && rowOffset == 0)
    {
        return;
    }
    if (row == 0)
    {
        rowOffset--;
    }
    else
    {
        row--;
    }
    displayRows();
}

void VmpcDisksScreen::down()
{
    if (row + rowOffset + 1 >= mpc.getDisks().size())
    {
        return;
    }
    if (row == 3)
    {
        rowOffset++;
    }
    else
    {
        row++;
    }
    displayRows();
}

bool VmpcDisksScreen::hasConfigChanged()
{
    auto persisted = VolumesPersistence::getPersistedConfigs(mpc);

    for (auto &kv : config)
    {
        if (persisted.find(kv.first) == end(persisted))
        {
            return true;
        }
        if (persisted[kv.first] != kv.second)
        {
            return true;
        }
    }

    return false;
}

void VmpcDisksScreen::displayFunctionKeys()
{
    auto vmpcSettingsScreen = mpc.screens->get<VmpcSettingsScreen>();
    auto midiControlMode = vmpcSettingsScreen->getMidiControlMode();
    auto newArrangement = midiControlMode == VmpcSettingsScreen::MidiControlMode::ORIGINAL ? 1 : 0;
    newArrangement += hasConfigChanged() ? 0 : 1;
    ls->setFunctionKeysArrangement(newArrangement);
}

void VmpcDisksScreen::displayUpAndDown()
{
    findChild<Label>("up")->Hide(rowOffset == 0);
    findChild<Label>("down")->Hide(rowOffset + 4 >= mpc.getDisks().size());
}

void VmpcDisksScreen::refreshConfig()
{
    config.clear();

    for (auto &d : mpc.getDisks())
    {
        auto &diskVol = d->getVolume();
        config[diskVol.volumeUUID] = diskVol.mode;
    }

    if (row + rowOffset >= config.size())
    {
        row = 0;
        rowOffset = 0;
    }

    displayRows();
    displayFunctionKeys();
}
