#include "DiskController.hpp"

#include <Mpc.hpp>

#include <disk/AbstractDisk.hpp>
#include <disk/StdDisk.hpp>
#include <disk/Volume.hpp>

#include <lcdgui/Screens.hpp>
#include <lcdgui/screens/VmpcDisksScreen.hpp>

using namespace mpc::disk;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;

DiskController::DiskController(mpc::Mpc& _mpc)
	: mpc (_mpc)
{
}

void DiskController::initDisks()
{
    static auto volume = Volume();
    volume.localDirectoryPath = "/Users/izmar/Documents/VMPC2000XL/Volumes/MPC2000XL";
    disks.emplace_back(std::make_shared<StdDisk>(mpc, volume));
}

std::weak_ptr<AbstractDisk> DiskController::getDisk()
{
    return disks[0];
}
