#include "VmpcDisksScreen.hpp"

#include <lcdgui/Parameter.hpp>
#include <lcdgui/Screens.hpp>
#include <lcdgui/screens/dialog2/PopupScreen.hpp>
#include <disk/DiskController.hpp>
#include <disk/AbstractDisk.hpp>
#include <disk/RawDisk.hpp>
#include <disk/StdDisk.hpp>
#include <nvram/VolumesPersistence.hpp>

#include <lang/StrUtil.hpp>

using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::dialog2;
using namespace mpc::lcdgui;
using namespace mpc::disk;
using namespace mpc::nvram;
using namespace moduru::lang;

VmpcDisksScreen::VmpcDisksScreen(mpc::Mpc& mpc, const int layerIndex)
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
    findChild<Label>("up").lock()->setText("\u00C7");
    findChild<Label>("down").lock()->setText("\u00C6");

    mpc.getDiskController()->detectRawUsbVolumes();

    refreshConfig();
}

void VmpcDisksScreen::function(int i)
{
    switch (i)
    {
        case 0:
            openScreen("vmpc-settings");
            break;
        case 1:
            openScreen("vmpc-keyboard");
            break;
        case 2:
            openScreen("vmpc-auto-save");
            break;
        case 4:
            openScreen("vmpc-midi");
            break;
        case 5:
            auto popupScreen = mpc.screens->get<PopupScreen>("popup");
            openScreen("popup");

            if (hasConfigChanged())
            {
                for (auto& kv : config)
                {
                    auto uuid = kv.first;
                    for (auto& d : mpc.getDisks())
                    {
                        if (d->getVolume().volumeUUID == uuid)
                            d->getVolume().mode = kv.second;
                    }
                }
                
                VolumesPersistence::save(mpc);
                popupScreen->setText("Volume configurations saved");
            }
            else
            {
                popupScreen->setText("Volume configurations unchanged");
            }

            popupScreen->returnToScreenAfterMilliSeconds("vmpc-disks", 1000);

            break;
    }
}

void VmpcDisksScreen::turnWheel(int i)
{
    auto& volume = mpc.getDisks()[row]->getVolume();
    
    if (volume.volumeUUID == "default_volume") return;
    
    auto current = config[volume.volumeUUID];
    if (current + i < 0 || current + i > 2) return;
    config[volume.volumeUUID] = static_cast<MountMode>(current + i);
    
    displayRows();
    displayFunctionKeys();
}

std::string formatFileSize(uint64_t size)
{
    std::string hrSize = "";
    uint64_t b = size;
    float k = size / 1024.0;
    float m = (size / 1024.0) / 1024.0;
    float g = ((size / 1024.0) / 1024.0) / 1024.0;
    float t = (((size / 1024.0) / 1024.0) / 1024.0) / 1024.0;

    if(t > 1)
    {
        hrSize = StrUtil::TrimDecimals(t, 1) + "T";
    } else if(g > 1)
    {
        hrSize = std::to_string((int)round(g)) + "G";
    } else if(m > 1)
    {
        hrSize = std::to_string((int)round(m)) + "M";
    } else if(k > 1)
    {
        hrSize = std::to_string((int)round(k)) + "K";
    } else
    {
        hrSize = std::to_string(b) + "B";
    }
    return hrSize;
}

void VmpcDisksScreen::displayRows()
{
    auto disks = mpc.getDisks();
    
    for (int i = 0; i < 4; i++)
    {
        auto volume = findChild<Label>("volume" + std::to_string(i)).lock();
        auto type = findChild<Label>("type" + std::to_string(i)).lock();
        auto size = findChild<Label>("size" + std::to_string(i)).lock();
        auto mode = findChild<Field>("mode" + std::to_string(i)).lock();
        
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
        size->setText(formatFileSize(disk->getTotalSize()));
        mode->setText(Volume::modeShortName(config[disk->getVolume().volumeUUID]));
    }
    
    displayUpAndDown();
}

void VmpcDisksScreen::up()
{
    if (row == 0 && rowOffset == 0) return;
    if (row == 0) rowOffset--; else row--;
    displayRows();
}

void VmpcDisksScreen::down()
{
    if (row + rowOffset + 1 >= mpc.getDisks().size()) return;
    if (row == 3) rowOffset++; else row++;
    displayRows();
}

bool VmpcDisksScreen::hasConfigChanged()
{
    auto persisted = VolumesPersistence::getPersistedConfigs();
    
    for (auto& kv : config)
    {
        if (persisted.find(kv.first) == end(persisted)) return true;
        if (persisted[kv.first] != kv.second) return true;
    }
    
    return false;
}

void VmpcDisksScreen::displayFunctionKeys()
{
    ls.lock()->setFunctionKeysArrangement(hasConfigChanged() ? 0 : 1);
}

void VmpcDisksScreen::displayUpAndDown()
{
    findChild<Label>("up").lock()->Hide(rowOffset == 0);
    findChild<Label>("down").lock()->Hide(rowOffset + 4 >= mpc.getDisks().size());
}

void VmpcDisksScreen::refreshConfig()
{
    config.clear();

    for (auto& d : mpc.getDisks())
    {
        auto& diskVol = d->getVolume();
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