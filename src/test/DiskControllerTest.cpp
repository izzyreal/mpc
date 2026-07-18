#include <catch2/catch_test_macros.hpp>

#include "TestMpc.hpp"
#include "disk/DiskController.hpp"
#include "disk/RawDisk.hpp"

using namespace mpc;
using namespace mpc::disk;

TEST_CASE("Disabling the active USB disk deactivates it", "[disk]")
{
    Mpc mpc;
    TestMpc::initializeTestMpcWithoutIoServices(mpc);

    auto diskController = mpc.getDiskController();
    (void) diskController->getActiveDisk();

    auto usbDisk = std::make_shared<RawDisk>(mpc);
    auto &volume = usbDisk->getVolume();
    volume.type = USB_VOLUME;
    volume.mode = READ_WRITE;
    volume.volumeUUID = "test-usb-volume";
    volume.volumePath = "/dev/test-usb-volume";
    volume.label = "USB";
    volume.volumeSize = 1024;

    auto &disks = diskController->getDisks();
    disks.push_back(usbDisk);
    diskController->setActiveDiskIndex(static_cast<int>(disks.size()) - 1);
    REQUIRE(diskController->getActiveDisk() == usbDisk);

    volume.mode = DISABLED;
    REQUIRE(diskController->ensureActiveDiskIsEnabled());

    REQUIRE(diskController->getActiveDiskIndex() == 0);
    REQUIRE(diskController->getActiveDisk()->getVolume().volumeUUID ==
            "default_volume");
}
