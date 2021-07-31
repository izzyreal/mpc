#include "VmpcDisksScreen.hpp"

#include <lcdgui/Parameter.hpp>
#include <disk/DiskController.hpp>
#include <disk/AbstractDisk.hpp>
#include <disk/RawDisk.hpp>
#include <disk/StdDisk.hpp>

using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui;
using namespace mpc::disk;

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
    
    displayRows();
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
    }
}

void VmpcDisksScreen::turnWheel(int i)
{
    auto& volume = mpc.getDisks()[row]->getVolume();
    if (volume.mode + i < 0 || volume.mode + i > 2) return;
    volume.mode = static_cast<MountMode>(volume.mode + i);
    displayRows();
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
        size->setText(AbstractDisk::formatFileSize(disk->getTotalSize()));
        mode->setText(disk->getModeShortName());

    }
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
