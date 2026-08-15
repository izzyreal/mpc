#include <catch2/catch_test_macros.hpp>

#include "TestMpc.hpp"
#include "disk/DiskController.hpp"
#include "disk/RawDisk.hpp"
#include "disk/StdDisk.hpp"
#include "lcdgui/Field.hpp"
#include "lcdgui/LayeredScreen.hpp"
#include "lcdgui/screens/LoadScreen.hpp"
#include "lcdgui/screens/SaveScreen.hpp"
#include "lcdgui/screens/VmpcDisksScreen.hpp"
#include "nvram/VolumesPersistence.hpp"

using namespace mpc;
using namespace mpc::disk;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;

namespace
{
    std::shared_ptr<RawDisk> makeUsbDisk(Mpc &mpc, const std::string &uuid,
                                         const std::string &label)
    {
        auto disk = std::make_shared<RawDisk>(mpc);
        auto &volume = disk->getVolume();
        volume.type = USB_VOLUME;
        volume.mode = READ_WRITE;
        volume.volumeUUID = uuid;
        volume.volumePath = "/dev/" + uuid;
        volume.label = label;
        volume.volumeSize = 1024;
        return disk;
    }

    std::shared_ptr<StdDisk> makeLocalDisk(Mpc &mpc, const std::string &uuid,
                                           const std::string &label)
    {
        auto disk = std::make_shared<StdDisk>(mpc);
        auto &volume = disk->getVolume();
        volume.type = LOCAL_DIRECTORY;
        volume.mode = READ_WRITE;
        volume.volumeUUID = uuid;
        volume.localDirectoryPath =
            mpc.getDisks()[0]->getVolume().localDirectoryPath;
        volume.label = label;
        volume.volumeSize = 1024;
        disk->initRoot();
        return disk;
    }
} // namespace

TEST_CASE("Disabling the active disk restores the previous enabled disk",
          "[disk]")
{
    Mpc mpc;
    TestMpc::initializeTestMpcWithoutIoServices(mpc);

    auto diskController = mpc.getDiskController();
    (void) diskController->getActiveDisk();

    auto &disks = diskController->getDisks();
    auto usbB = makeUsbDisk(mpc, "test-usb-b", "USB B");
    auto usbC = makeUsbDisk(mpc, "test-usb-c", "USB C");
    disks.push_back(usbB);
    disks.push_back(usbC);

    diskController->setActiveDiskIndex(1);
    diskController->setActiveDiskIndex(2);
    REQUIRE(diskController->getActiveDisk() == usbC);

    usbC->getVolume().mode = DISABLED;
    REQUIRE(diskController->ensureActiveDiskIsEnabled());

    REQUIRE(diskController->getActiveDiskIndex() == 1);
    REQUIRE(diskController->getActiveDisk() == usbB);

    usbB->getVolume().mode = DISABLED;
    REQUIRE(diskController->ensureActiveDiskIsEnabled());

    REQUIRE(diskController->getActiveDiskIndex() == 0);
    REQUIRE(diskController->getActiveDisk()->getVolume().volumeUUID ==
            "default_volume");
}

TEST_CASE("Disk fallback skips a removed previous disk", "[disk]")
{
    Mpc mpc;
    TestMpc::initializeTestMpcWithoutIoServices(mpc);

    auto diskController = mpc.getDiskController();
    (void) diskController->getActiveDisk();

    auto &disks = diskController->getDisks();
    auto usbB = makeUsbDisk(mpc, "test-usb-b", "USB B");
    auto usbC = makeUsbDisk(mpc, "test-usb-c", "USB C");
    disks.push_back(usbB);
    disks.push_back(usbC);

    diskController->setActiveDiskIndex(1);
    diskController->setActiveDiskIndex(2);
    disks.erase(disks.begin() + 1);

    REQUIRE(diskController->ensureActiveDiskIsEnabled());
    REQUIRE(diskController->getActiveDiskIndex() == 0);
    REQUIRE(diskController->getActiveDisk()->getVolume().volumeUUID ==
            "default_volume");
}

TEST_CASE("LOAD and SAVE discard a cached disabled disk after a popup",
          "[disk][ui]")
{
    Mpc mpc;
    TestMpc::initializeTestMpcWithoutIoServices(mpc);

    auto diskController = mpc.getDiskController();
    (void) diskController->getActiveDisk();

    auto &disks = diskController->getDisks();
    auto diskB = makeLocalDisk(mpc, "test-local-b", "DISK B");
    auto diskC = makeLocalDisk(mpc, "test-local-c", "DISK C");
    disks.push_back(diskB);
    disks.push_back(diskC);

    diskController->setActiveDiskIndex(1);
    diskController->setActiveDiskIndex(2);

    const auto layeredScreen = mpc.getLayeredScreen();
    const auto loadScreen = mpc.screens->get<ScreenId::LoadScreen>();
    layeredScreen->openScreenById(ScreenId::LoadScreen);
    REQUIRE(loadScreen->findField("device")->getText() == "DISK C");

    const auto disksScreen = mpc.screens->get<ScreenId::VmpcDisksScreen>();
    layeredScreen->openScreenById(ScreenId::VmpcDisksScreen);
    disksScreen->down();
    disksScreen->down();
    disksScreen->turnWheel(-2);
    disksScreen->function(5);

    REQUIRE(layeredScreen->isCurrentScreen({ScreenId::PopupScreen}));
    REQUIRE(diskController->getActiveDisk() == diskB);

    layeredScreen->openScreenById(ScreenId::LoadScreen);
    REQUIRE(loadScreen->findField("device")->getText() == "DISK B");

    diskC->getVolume().mode = READ_WRITE;
    diskController->setActiveDiskIndex(2);
    nvram::VolumesPersistence::save(mpc);

    const auto saveScreen = mpc.screens->get<ScreenId::SaveScreen>();
    layeredScreen->openScreenById(ScreenId::SaveScreen);
    REQUIRE(saveScreen->findField("device")->getText() == "DISK C");

    layeredScreen->openScreenById(ScreenId::VmpcDisksScreen);
    disksScreen->turnWheel(-2);
    disksScreen->function(5);

    REQUIRE(layeredScreen->isCurrentScreen({ScreenId::PopupScreen}));
    REQUIRE(diskController->getActiveDisk() == diskB);

    layeredScreen->openScreenById(ScreenId::SaveScreen);
    REQUIRE(saveScreen->findField("device")->getText() == "DISK B");
}
