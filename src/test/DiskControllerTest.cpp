#include <catch2/catch_test_macros.hpp>

#include "TestMpc.hpp"
#include <ImageBlockDevice.hpp>
#include <fat/AkaiFatFileSystem.hpp>
#include <util/SuperFloppyFormatter.hpp>
#include <fat/AkaiFatLfnDirectoryEntry.hpp>
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
    class TestUsbDisk : public RawDisk
    {
    public:
        explicit TestUsbDisk(Mpc &mpc) : RawDisk(mpc) {}
        void initRoot() override {}
    };

    std::shared_ptr<RawDisk> makeUsbDisk(Mpc &mpc, const std::string &uuid,
                                         const std::string &label)
    {
        auto disk = std::make_shared<TestUsbDisk>(mpc);
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
    (void)diskController->getActiveDisk();

    auto &disks = diskController->getDisks();
    auto usbB = makeUsbDisk(mpc, "test-usb-b", "USB B");
    auto usbC = makeUsbDisk(mpc, "test-usb-c", "USB C");
    disks.push_back(usbB);
    disks.push_back(usbC);

    diskController->activateDisk(1);
    diskController->activateDisk(2);
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
    (void)diskController->getActiveDisk();

    auto &disks = diskController->getDisks();
    auto usbB = makeUsbDisk(mpc, "test-usb-b", "USB B");
    auto usbC = makeUsbDisk(mpc, "test-usb-c", "USB C");
    disks.push_back(usbB);
    disks.push_back(usbC);

    diskController->activateDisk(1);
    diskController->activateDisk(2);
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
    (void)diskController->getActiveDisk();

    auto &disks = diskController->getDisks();
    auto diskB = makeLocalDisk(mpc, "test-local-b", "DISK B");
    auto diskC = makeLocalDisk(mpc, "test-local-c", "DISK C");
    disks.push_back(diskB);
    disks.push_back(diskC);

    diskController->activateDisk(1);
    diskController->activateDisk(2);

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
    diskController->activateDisk(2);
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

namespace
{
    class LifecycleDisk : public StdDisk
    {
    public:
        explicit LifecycleDisk(Mpc &mpc) : StdDisk(mpc) {}
        void initFiles() override {}
        int opens = 0, closes = 0;
        bool failOpen = false, failClose = false;
        void initRoot() override
        {
            ++opens;
            if (failOpen)
            {
                throw std::runtime_error("Mount failed");
            }
        }
        void close() override
        {
            if (failClose)
            {
                throw std::runtime_error("Flush failed");
            }
            ++closes;
        }
    };
} // namespace

TEST_CASE("Device activation owns the mount lifecycle", "[disk][mount]")
{
    Mpc mpc;
    TestMpc::initializeTestMpcWithoutIoServices(mpc);
    auto controller = mpc.getDiskController();
    (void)controller->getActiveDisk();
    auto a = std::make_shared<LifecycleDisk>(mpc);
    auto b = std::make_shared<LifecycleDisk>(mpc);
    a->getVolume().mode = b->getVolume().mode = READ_WRITE;
    a->getVolume().volumeUUID = "lifecycle-a";
    b->getVolume().volumeUUID = "lifecycle-b";
    controller->getDisks().push_back(a);
    controller->getDisks().push_back(b);
    REQUIRE(controller->activateDisk(1).empty());
    REQUIRE(a->opens == 1);

    SECTION("LOAD routes selection through the lifecycle")
    {
        auto screen = mpc.screens->get<ScreenId::LoadScreen>();
        mpc.getLayeredScreen()->openScreenById(ScreenId::LoadScreen);
        mpc.getLayeredScreen()->setFocus("device");
        screen->turnWheel(1);
        screen->function(4);
        REQUIRE(controller->getActiveDisk() == b);
        REQUIRE(a->closes == 1);
        REQUIRE(b->opens == 1);
    }
    SECTION("SAVE routes selection through the lifecycle")
    {
        auto screen = mpc.screens->get<ScreenId::SaveScreen>();
        mpc.getLayeredScreen()->openScreenById(ScreenId::SaveScreen);
        mpc.getLayeredScreen()->setFocus("device");
        screen->turnWheel(1);
        screen->function(4);
        REQUIRE(controller->getActiveDisk() == b);
        REQUIRE(a->closes == 1);
        REQUIRE(b->opens == 1);
    }
    SECTION("Switch and reselect")
    {
        REQUIRE(controller->activateDisk(2).empty());
        REQUIRE(a->closes == 1);
        REQUIRE(b->opens == 1);
        REQUIRE(controller->activateDisk(2).empty());
        REQUIRE(b->opens == 1);
        REQUIRE(b->closes == 0);
        REQUIRE(controller->activateDisk(0).empty());
        REQUIRE(b->closes == 1);
    }
    SECTION("Failed mount retains current device")
    {
        b->failOpen = true;
        REQUIRE_FALSE(controller->activateDisk(2).empty());
        REQUIRE(controller->getActiveDisk() == a);
        REQUIRE(a->closes == 0);
    }
    SECTION("Failed flush releases candidate and retains current device")
    {
        a->failClose = true;
        REQUIRE_FALSE(controller->activateDisk(2).empty());
        REQUIRE(controller->getActiveDisk() == a);
        REQUIRE(a->closes == 0);
        REQUIRE(b->closes == 1);
    }
    SECTION("Disabled and invalid devices are rejected")
    {
        b->getVolume().mode = DISABLED;
        REQUIRE_FALSE(controller->activateDisk(2).empty());
        REQUIRE_FALSE(controller->activateDisk(-1).empty());
        REQUIRE_FALSE(controller->activateDisk(3).empty());
        REQUIRE(b->opens == 0);
        REQUIRE(a->closes == 0);
    }
    SECTION("Legacy I/O prevents switching")
    {
        auto lease = mpc.fileOperationGate.tryAcquire();
        REQUIRE(lease);
        REQUIRE_FALSE(controller->activateDisk(2).empty());
        REQUIRE(controller->getActiveDisk() == a);
        REQUIRE(b->opens == 0);
        REQUIRE(a->closes == 0);
    }
    SECTION("Managed saves prevent switching")
    {
        auto lease = mpc.fileOperationGate.tryAcquire(true);
        REQUIRE(lease);
        REQUIRE_FALSE(controller->activateDisk(2).empty());
        REQUIRE(controller->getActiveDisk() == a);
        REQUIRE(b->opens == 0);
    }
}

TEST_CASE("Mounted sessions release access and preserve FAT data",
          "[disk][mount-session]")
{
    Mpc mpc;
    TestMpc::initializeTestMpcWithoutIoServices(mpc);
    auto path = mpc_fs::path(mpc.getDisk()->getAbsolutePath()) / "session.img";
    constexpr int size = 5 * 1024 * 1024;
    {
        std::ofstream initialize(path.string(), std::ios::binary);
        initialize.seekp(size - 1);
        initialize.put(0);
    }
    {
        std::fstream stream(path.string(),
                            std::ios::in | std::ios::out | std::ios::binary);
        auto device = std::make_shared<akaifat::ImageBlockDevice>(stream, size);
        akaifat::SuperFloppyFormatter formatter(device);
        std::unique_ptr<akaifat::fat::AkaiFatFileSystem> fs(formatter.format());
        fs->close();
    }
    Volume volume;
    volume.type = USB_VOLUME;
    volume.mode = READ_WRITE;
    volume.volumePath = path.string();
    volume.volumeSize = size;
    int opens = 0, releases = 0;
    const auto open = [&](const std::string &source, bool readOnly)
    {
        ++opens;
        return std::fstream(
            source, std::ios::binary | std::ios::in |
                        (readOnly ? std::ios::openmode{} : std::ios::out));
    };
    const auto release = [&](const std::string &source)
    {
        CHECK(source == path.string());
        ++releases;
    };
    SECTION("Writes survive closing and reopening")
    {
        {
            MountedVolumeSession session(volume, open, release);
            std::string name = "SAVED.SND";
            auto entry = std::dynamic_pointer_cast<
                akaifat::fat::AkaiFatLfnDirectoryEntry>(
                session.getRoot()->addFile(name));
            MpcFile file(entry);
            std::vector<char> bytes{'M', 'P', 'C'};
            file.setFileDataChecked(bytes);
            session.close();
            session.close();
            CHECK(releases == 1);
        }
        CHECK(releases == 1);
        volume.mode = READ_ONLY;
        const auto before = MpcFile(path).getBytes();
        {
            MountedVolumeSession session(volume, open, release);
            std::string name = "SAVED.SND";
            auto entry = std::dynamic_pointer_cast<
                akaifat::fat::AkaiFatLfnDirectoryEntry>(
                session.getRoot()->getEntry(name));
            REQUIRE(entry);
            CHECK(MpcFile(entry).getBytes() ==
                  std::vector<char>{'M', 'P', 'C'});
            session.flush();
        }
        CHECK(MpcFile(path).getBytes() == before);
        CHECK(opens == 2);
        CHECK(releases == 2);
    }
    SECTION("Failed open releases platform access")
    {
        REQUIRE_THROWS(MountedVolumeSession(
            volume,
            [](const std::string &, bool)
            {
                return std::fstream{};
            },
            release));
        CHECK(releases == 1);
    }
    SECTION("Invalid filesystem releases platform access")
    {
        {
            std::fstream corrupt(path.string(), std::ios::in | std::ios::out |
                                                    std::ios::binary);
            corrupt.seekp(510);
            corrupt.put(0);
        }
        REQUIRE_THROWS(MountedVolumeSession(volume, open, release));
        CHECK(opens == 1);
        CHECK(releases == 1);
    }
    SECTION("Captured destination retains session ownership")
    {
        auto session =
            std::make_shared<MountedVolumeSession>(volume, open, release);
        auto destination =
            SaveDestination::fromRaw(session->getRoot(), {}, true, 0,
                                     [session]
                                     {
                                         session->flush();
                                     });
        session.reset();
        CHECK(releases == 0);
        destination->prepare();
        auto file = destination->create("HELD.SND", false);
        std::vector<char> bytes{'A'};
        file->setFileDataChecked(bytes);
        destination->flush();
        destination.reset();
        CHECK(releases == 1);
    }
}
