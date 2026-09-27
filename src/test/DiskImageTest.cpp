#include "file/kaitai/Mpc60SetPreview.hpp"
#include "file/kaitai/Mpc60SetProgramLoader.hpp"
#include "sampler/Sampler.hpp"
#include <cstdlib>
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

namespace
{
    std::vector<char> emptyMpc60Image()
    {
        std::vector<char> b(819200, 0);
        const auto u16 = [&](int p, int v)
        {
            b[p] = v & 255;
            b[p + 1] = v >> 8;
        };
        b[0] = char(0xeb);
        b[1] = 0x34;
        b[2] = char(0x90);
        u16(11, 512);
        b[13] = 2;
        u16(14, 1);
        b[16] = 2;
        u16(17, 112);
        u16(19, 1600);
        b[21] = char(0xf9);
        u16(22, 3);
        u16(24, 10);
        u16(26, 2);
        for (int offset : {512, 2048})
        {
            b[offset] = char(0xf9);
            b[offset + 1] = b[offset + 2] = char(0xff);
        }
        return b;
    }
} // namespace

TEST_CASE(
    "MPC60 FAT12 images retain Akai names and packed allocation on writes",
    "[disk][image][fat12]")
{
    ImageFixture f;
    const auto original = emptyMpc60Image();
    REQUIRE(set_file_data(f.path, original));
    f.volume.mode = READ_WRITE;
    f.bind();
    REQUIRE(f.controller().activateDisk(1).empty());
    const std::string name = "PROGRAM_01.PGM";
    auto file = f.mpc.getDisk()->newFile(name);
    REQUIRE(file);
    std::vector<char> data(3500, 'x');
    file->setFileDataChecked(data);
    REQUIRE(f.controller().activateDisk(0).empty());
    auto written = MpcFile(f.path).getBytes();
    CHECK(
        std::equal(original.begin(), original.begin() + 512, written.begin()));
    CHECK(std::equal(written.begin() + 512, written.begin() + 2048,
                     written.begin() + 2048));
    bool foundAkaiName = false;
    for (size_t offset = 3584; offset < 7168; offset += 32)
    {
        if (std::string(written.data() + offset, 11) == "PROGRAM_PGM")
        {
            CHECK(std::string(written.data() + offset + 12, 8) == "01      ");
            foundAkaiName = true;
        }
    }
    CHECK(foundAkaiName);
    // Independent FAT12 oracle: four data clusters, chain 2 -> 3 -> 4 -> 5 ->
    // EOF.
    CHECK((written[515] & 255) == 3);
    CHECK((written[516] & 255) == 0x40);
    CHECK((written[517] & 255) == 0);
    CHECK((written[518] & 255) == 5);
    CHECK((written[519] & 255) == 0xf0);
    CHECK((written[520] & 255) == 0xff);
    REQUIRE(f.controller().activateDisk(1).empty());
    f.mpc.getDisk()->initFiles();
    REQUIRE(f.mpc.getDisk()->getFile(name));
    CHECK(f.mpc.getDisk()->getFile(name)->getBytes() == data);
    REQUIRE(f.mpc.getDisk()->getFile(name)->del());
    REQUIRE(f.controller().activateDisk(0).empty());
    written = MpcFile(f.path).getBytes();
    CHECK(std::equal(original.begin() + 512, original.begin() + 3584,
                     written.begin() + 512));
    REQUIRE(
        f.controller().setVolumeMode(f.volume.volumeUUID, READ_ONLY).empty());
    REQUIRE(f.controller().activateDisk(1).empty());
    REQUIRE_THROWS(f.mpc.getDisk()->newFile("NO.SND"));
    REQUIRE(f.controller().activateDisk(0).empty());
    CHECK(MpcFile(f.path).getBytes() == written);
}

TEST_CASE("FAT12 image validation rejects damage before mounting",
          "[disk][image][fat12]")
{
    ImageFixture f;
    auto bytes = emptyMpc60Image();
    SECTION("Missing sectors")
    {
        bytes.resize(737280);
    }
    SECTION("Unrecognized unsigned geometry")
    {
        bytes[24] = 9;
    }
    SECTION("Cyclic chain")
    {
        bytes[515] = 2;
        bytes[2051] = 2;
    }
    SECTION("Mismatched FAT copies")
    {
        bytes[2051] = 3;
    }
    REQUIRE(set_file_data(f.path, bytes));
    CHECK_FALSE(f.controller().bindImage(f.volume).empty());
    CHECK(f.controller().getActiveDiskIndex() == 0);
    CHECK(MpcFile(f.path).getBytes() == bytes);
}

// External factory sound data is not redistributed. Opt in with a local corpus.
TEST_CASE("MPC60 image corpus mounts SET files and survives direct writes",
          "[.][mpc60-corpus]")
{
    const auto *path = std::getenv("VMPC_MPC60_IMAGE_CORPUS");
    REQUIRE(path != nullptr);
    size_t checked = 0;
    for (const auto &entry : mpc_fs::directory_iterator(path))
    {
        if (entry.path().extension() != ".img")
        {
            continue;
        }
        CAPTURE(entry.path().filename().string());
        ImageFixture f;
        const auto source = MpcFile(entry.path()).getBytes();
        // First exercise the original in read-only mode.
        f.volume = platform::selectedImage(entry.path().string());
        f.bind();
        REQUIRE(f.controller().activateDisk(1).empty());
        const auto setName = [&]() -> std::string
        {
            const auto root =
                f.mpc.getDisk()->captureSaveDestination(0)->scan();
            for (const auto &item : root.allFiles)
            {
                if (item->getName().size() >= 4 &&
                    item->getName().substr(item->getName().size() - 4) ==
                        ".SET")
                {
                    return item->getName();
                }
            }
            return {};
        }();
        REQUIRE_FALSE(setName.empty());
        f.mpc.getDisk()->initFiles();
        auto file = f.mpc.getDisk()->getFile(setName);
        REQUIRE(file);
        auto setBytes = file->getBytes();
        const auto preview =
            mpc::file::kaitai::Mpc60SetPreviewLoader::loadPreview(setBytes);
        CHECK(preview.totalNumberOfSampleWords ==
              preview.soundSampleWords->size());
        // A representative existing importer integration, without requiring
        // every historical SET variant to be supported by the content loader.
        if (setName == "STUDIO.SET")
        {
            REQUIRE(mpc::file::kaitai::Mpc60SetProgramLoader::load(
                f.mpc, file, preview,
                mpc::file::kaitai::Mpc60SetProgramLoader::defaultConversionTable(f.mpc), true));
            CHECK(f.mpc.getSampler()->getSoundCount() > 0);
        }
        REQUIRE(f.controller().activateDisk(0).empty());
        CHECK(MpcFile(entry.path()).getBytes() == source);
        // All writes target a disposable copy, never the source corpus.
        REQUIRE(set_file_data(f.path, source));
        auto writable = platform::selectedImage(f.path.string());
        writable.mode = READ_WRITE;
        REQUIRE(f.controller().bindImage(writable).empty());
        REQUIRE(f.controller().activateDisk(2).empty());
        akaifat::fat::Fat12Type codec;
        std::vector<char> fat(source.begin() + 512, source.begin() + 2048);
        bool hasSpace = false;
        for (int cluster = 2; cluster < 795; ++cluster)
        {
            hasSpace |= codec.readEntry(fat, cluster) == 0;
        }
        std::vector<char> payload{'O', 'K'};
        if (hasSpace)
        {
            auto marker = f.mpc.getDisk()->newFile("CHECK_123456.BIN");
            REQUIRE(marker);
            marker->setFileDataChecked(payload);
        }
        else
        {
            // Full factory disks can still overwrite an existing allocation.
            f.mpc.getDisk()->initFiles();
            REQUIRE(f.mpc.getDisk()->getFile(setName));
            f.mpc.getDisk()->getFile(setName)->setFileDataChecked(setBytes);
        }
        REQUIRE(f.controller().activateDisk(0).empty());
        REQUIRE(f.controller().activateDisk(2).empty());
        f.mpc.getDisk()->initFiles();
        if (hasSpace)
        {
            REQUIRE(f.mpc.getDisk()->getFile("CHECK_123456.BIN"));
            CHECK(f.mpc.getDisk()->getFile("CHECK_123456.BIN")->getBytes() ==
                  payload);
        }
        REQUIRE(f.mpc.getDisk()->getFile(setName));
        CHECK(f.mpc.getDisk()->getFile(setName)->getBytes() == setBytes);
        REQUIRE(f.controller().activateDisk(0).empty());
        const auto written = MpcFile(f.path).getBytes();
        CHECK(written.size() == source.size());
        CHECK(
            std::equal(source.begin(), source.begin() + 512, written.begin()));
        CHECK(std::equal(written.begin() + 512, written.begin() + 2048,
                         written.begin() + 2048));
        ++checked;
    }
    REQUIRE(checked > 0);
    SUCCEED("Checked " + std::to_string(checked) + " images");
}
