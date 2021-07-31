#include "DiskController.hpp"

#include <Mpc.hpp>
#include <Paths.hpp>

#include <disk/AbstractDisk.hpp>
#include <disk/StdDisk.hpp>
#include <disk/RawDisk.hpp>
#include <disk/Volume.hpp>

using namespace mpc::disk;
using namespace mpc::lcdgui;

DiskController::DiskController(mpc::Mpc& _mpc)
	: mpc (_mpc)
{
}

void DiskController::initDisks()
{
    static auto volume1 = Volume();
    volume1.type = LOCAL_DIRECTORY;
    volume1.localDirectoryPath = mpc::Paths::defaultLocalVolumePath();
    
    static auto volume2 = Volume();
    volume2.type = USB_VOLUME;
    volume2.volumePath = "disk4";
    volume2.volumeSize = 123 * 1024 * 1024;
    
    disks.emplace_back(std::make_shared<StdDisk>(mpc, volume1));
//    disks.emplace_back(std::make_shared<RawDisk>(mpc, volume2));
}

std::weak_ptr<AbstractDisk> DiskController::getDisk()
{
    return disks[0];
}
