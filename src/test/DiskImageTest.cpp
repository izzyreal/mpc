#include <catch2/catch_test_macros.hpp>
#include "TestMpc.hpp"
#include "disk/DiskController.hpp"
#include "disk/AbstractDisk.hpp"
#include "disk/ImageFileDevice.hpp"
#include "nvram/VolumesPersistence.hpp"
#include "lcdgui/LayeredScreen.hpp"
#include "lcdgui/Label.hpp"
#include "lcdgui/screens/VmpcDisksScreen.hpp"
#include <fat/AkaiFatFileSystem.hpp>
#include <util/SuperFloppyFormatter.hpp>
#include <nlohmann/json.hpp>
#include <fstream>

using namespace mpc;
using namespace mpc::disk;
namespace
{
    struct ImageFixture
    {
        Mpc mpc;
        mpc_fs::path path;
        Volume volume;
        ImageFixture()
        {
            TestMpc::initializeTestMpcWithoutIoServices(mpc);
            path = mpc_fs::path(mpc.getDisk()->getAbsolutePath()) /
                   "source image.img";
            {
                std::ofstream file(path.string(), std::ios::binary);
                file.seekp(5 * 1024 * 1024 - 1);
                file.put(0);
            }
            {
                auto device = ImageFileDevice::open(path.string(), false);
                akaifat::SuperFloppyFormatter formatter(device);
                std::unique_ptr<akaifat::fat::AkaiFatFileSystem> fs(
                    formatter.format());
                fs->close();
                device->flush();
            }
            volume = platform::selectedImage(path.string());
        }
        DiskController &controller()
        {
            return *mpc.getDiskController();
        }
        void bind()
        {
            REQUIRE(controller().bindImage(volume).empty());
        }
    };
} // namespace

TEST_CASE("Image bindings do not mount until selected", "[disk][image]")
{
    ImageFixture f;
    const auto original = MpcFile(f.path).getBytes();
    f.bind();
    REQUIRE(f.controller().getActiveDiskIndex() == 0);
    REQUIRE(f.controller().getDisks().size() == 2);
    REQUIRE_FALSE(f.controller().isFilePickerPending());
    REQUIRE(f.controller().validateImage(f.volume.volumeUUID).empty());
    REQUIRE(MpcFile(f.path).getBytes() == original);
    REQUIRE_NOTHROW(ImageFileDevice::open(f.path.string(), false));
    const auto saved = nvram::VolumesPersistence::getPersistedImages(f.mpc);
    REQUIRE(saved.size() == 1);
    CHECK(saved[0].diskImagePath == f.path.string());
    CHECK(saved[0].mode == READ_ONLY);
    CHECK_FALSE(f.controller().bindImage(f.volume).empty());
}

TEST_CASE("Image writes update the original and survive reopening",
          "[disk][image]")
{
    ImageFixture f;
    f.volume.mode = READ_WRITE;
    f.bind();
    REQUIRE(f.controller().activateDisk(1).empty());
    auto disk = f.mpc.getDisk();
    REQUIRE(disk->newFolder("SONGS"));
    REQUIRE(disk->moveForward("SONGS"));
    auto file = disk->newFile("ORIGINAL.SND");
    std::vector<char> bytes{'D', 'I', 'R', 'E', 'C', 'T'};
    file->setFileDataChecked(bytes);
    REQUIRE(f.controller().validateImage(f.volume.volumeUUID).empty());
    REQUIRE_THROWS(ImageFileDevice::open(f.path.string(), false));
    REQUIRE(f.controller().activateDisk(0).empty());
    REQUIRE_NOTHROW(ImageFileDevice::open(f.path.string(), false));
    REQUIRE(f.controller().activateDisk(1).empty());
    REQUIRE(disk->moveForward("SONGS"));
    disk->initFiles();
    REQUIRE(disk->getFile("ORIGINAL.SND"));
    CHECK(disk->getFile("ORIGINAL.SND")->getBytes() == bytes);
    REQUIRE(f.controller().activateDisk(0).empty());
    CHECK(mpc_fs::file_size(f.path).value() == 5 * 1024 * 1024);
}

TEST_CASE("Images can be bound before the first disk lookup", "[disk][image]")
{
    ImageFixture source;
    Mpc consumer;
    TestMpc::resetTestDataRoot(consumer);
    MpcInitOptions options;
    options.startMidiDeviceDetector = options.startAudioServer =
        options.detectRawUsbVolumes = options.installDemoFiles = false;
    consumer.init(options);
    REQUIRE(consumer.getDiskController()->bindImage(source.volume).empty());
    REQUIRE(consumer.getDisks().size() == 2);
    CHECK(consumer.getDisk()->getVolume().volumeUUID == "default_volume");
    CHECK(consumer.getDiskController()->activateDisk(1).empty());
}

TEST_CASE(
    "Read-only images stay byte identical and mode changes release old access",
    "[disk][image]")
{
    ImageFixture f;
    f.bind();
    const auto original = MpcFile(f.path).getBytes();
    REQUIRE(f.controller().activateDisk(1).empty());
    REQUIRE_THROWS(f.mpc.getDisk()->newFile("NO.SND"));
    REQUIRE(f.controller().activateDisk(0).empty());
    REQUIRE(MpcFile(f.path).getBytes() == original);
    REQUIRE(
        f.controller().setVolumeMode(f.volume.volumeUUID, READ_WRITE).empty());
    REQUIRE(f.controller().activateDisk(1).empty());
    REQUIRE(
        f.controller().setVolumeMode(f.volume.volumeUUID, READ_ONLY).empty());
    REQUIRE(f.controller().getActiveDiskIndex() == 0);
    REQUIRE_NOTHROW(ImageFileDevice::open(f.path.string(), false));
    REQUIRE(f.controller().activateDisk(1).empty());
    REQUIRE_THROWS(f.mpc.getDisk()->newFile("NO.SND"));
}

TEST_CASE("Missing or locked image activation preserves the current disk",
          "[disk][image]")
{
    ImageFixture f;
    f.bind();
    const auto current = f.mpc.getDisk();
    SECTION("Missing")
    {
        REQUIRE(mpc_fs::remove(f.path).value());
        CHECK_FALSE(f.controller().activateDisk(1).empty());
    }
    SECTION("In use")
    {
        auto lock = ImageFileDevice::open(f.path.string(), false);
        CHECK_FALSE(f.controller().activateDisk(1).empty());
    }
    SECTION("I/O in progress")
    {
        auto lease = f.mpc.fileOperationGate.tryAcquire();
        CHECK_FALSE(f.controller().activateDisk(1).empty());
        CHECK_FALSE(f.controller().removeImage(f.volume.volumeUUID).empty());
    }
    CHECK(f.mpc.getDisk() == current);
}

TEST_CASE("Removing bindings preserves source images and persistence",
          "[disk][image]")
{
    ImageFixture f;
    f.bind();
    const auto original = MpcFile(f.path).getBytes();
    REQUIRE(f.controller().removeImage(f.volume.volumeUUID).empty());
    CHECK(f.controller().getDisks().size() == 1);
    CHECK(nvram::VolumesPersistence::getPersistedImages(f.mpc).empty());
    CHECK(MpcFile(f.path).getBytes() == original);
}

TEST_CASE("Image devices check boundaries and reject unsupported boot geometry",
          "[disk][image]")
{
    ImageFixture f;
    SECTION("Last sector and read-only enforcement")
    {
        auto device = ImageFileDevice::open(f.path.string(), false);
        akaifat::ByteBuffer sector(512);
        REQUIRE_NOTHROW(device->write(device->getSize() - 512, sector));
        akaifat::ByteBuffer oversized(513);
        REQUIRE_THROWS(device->write(device->getSize() - 512, oversized));
        device->flush();
        device->close();
        REQUIRE_THROWS(device->read(0, sector));
        auto ro = ImageFileDevice::open(f.path.string(), true);
        REQUIRE_THROWS(ro->write(0, sector));
    }
    SECTION("Invalid sector size")
    {
        std::fstream file(f.path.string(),
                          std::ios::binary | std::ios::in | std::ios::out);
        file.seekp(11);
        file.put(0);
        file.put(0);
        file.close();
        CHECK_FALSE(f.controller().bindImage(f.volume).empty());
        CHECK(f.controller().getDisks().size() == 1);
        CHECK(nvram::VolumesPersistence::getPersistedImages(f.mpc).empty());
    }
    SECTION("Cyclic FAT chain")
    {
        auto device = ImageFileDevice::open(f.path.string(), false);
        akaifat::ByteBuffer boot(512);
        device->read(0, boot);
        const auto &b = boot.getBuffer();
        const auto reserved = static_cast<unsigned char>(b[14]) |
                              (static_cast<unsigned char>(b[15]) << 8);
        std::vector<char> selfLink{2, 0};
        akaifat::ByteBuffer value(selfLink);
        device->write(reserved * 512 + 4, value);
        device->close();
        CHECK_FALSE(f.controller().bindImage(f.volume).empty());
    }
    SECTION("Truncated image")
    {
        std::ofstream file(f.path.string(), std::ios::binary | std::ios::trunc);
        file.put(0);
        file.close();
        CHECK_FALSE(f.controller().bindImage(f.volume).empty());
    }
}

TEST_CASE("Startup restores active images without presentation context",
          "[disk][image]")
{
    ImageFixture f;
    f.bind();
    REQUIRE(f.controller().activateDisk(1).empty());
    REQUIRE(nvram::VolumesPersistence::save(f.mpc));
    const auto config =
        get_file_data(f.mpc.paths->configPath() / "volumes.json").value();
    REQUIRE(f.controller().activateDisk(0).empty());
    SECTION("Available") {}
    SECTION("Missing")
    {
        REQUIRE(mpc_fs::remove(f.path).value());
    }
    Mpc restored;
    TestMpc::resetTestDataRoot(restored);
    REQUIRE(mpc_fs::create_directories(restored.paths->configPath()));
    REQUIRE(
        set_file_data(restored.paths->configPath() / "volumes.json", config));
    MpcInitOptions options;
    options.startMidiDeviceDetector = options.startAudioServer =
        options.detectRawUsbVolumes = options.installDemoFiles = false;
    restored.init(options);
    (void)restored.getDisk();
    const bool available = mpc_fs::exists(f.path).value();
    CHECK(restored.getDiskController()->getActiveDiskIndex() ==
          (available ? 1 : 0));
    CHECK(restored.getDisks().size() == 2);
    CHECK_FALSE(restored.getDiskController()->isFilePickerPending());
}

TEST_CASE("DISKS scrolls and edits image bindings beyond the first page",
          "[disk][image][ui]")
{
    ImageFixture f;
    const auto bytes = MpcFile(f.path).getBytes();
    for (int i = 1; i <= 5; ++i)
    {
        auto path =
            f.path.parent_path() / ("image" + std::to_string(i) + ".img");
        REQUIRE(set_file_data(path, bytes));
        auto volume = platform::selectedImage(path.string(),
                                              "IMAGE " + std::to_string(i));
        REQUIRE(f.controller().bindImage(volume).empty());
    }
    const auto screen = f.mpc.screens->get<lcdgui::ScreenId::VmpcDisksScreen>();
    f.mpc.getLayeredScreen()->openScreenById(lcdgui::ScreenId::VmpcDisksScreen);
    for (int i = 0; i < 5; ++i)
    {
        screen->down();
    }
    CHECK(screen->findChild<lcdgui::Label>("volume3")->getText() == "IMAGE 5");
    screen->turnWheel(1);
    screen->function(5);
    CHECK(f.controller().getDisks()[5]->getVolume().mode == READ_WRITE);
    CHECK(f.controller().getDisks()[3]->getVolume().mode == READ_ONLY);
}

TEST_CASE("Failed binding persistence leaves the device list unchanged",
          "[disk][image]")
{
    ImageFixture f;
    const auto path = f.mpc.paths->configPath() / "volumes.json";
    REQUIRE(mpc_fs::create_directories(path));
    CHECK_FALSE(f.controller().bindImage(f.volume).empty());
    CHECK(f.controller().getDisks().size() == 1);
    CHECK(mpc_fs::exists(f.path).value());
}

TEST_CASE(
    "Picker completion and cancellation are safe without a host registration",
    "[disk][image]")
{
    platform::NativeFilePicker idle;
    CHECK_FALSE(idle.pending());
    idle.cancel();
    CHECK_FALSE(idle.poll());
    auto request = std::make_shared<platform::PickerRequest>();
    int dismissals = 0;
    request->setDismiss(
        [&]
        {
            ++dismissals;
        });
    request->cancel();
    request->cancel();
    CHECK(dismissals == 1);
    request->finish({platform::selectedImage("late.img"), {}});
    CHECK_FALSE(request->takeResult());
}
