#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "TestMpc.hpp"
#include "controller/ClientEventController.hpp"
#include "controller/ClientHardwareEventController.hpp"
#include "hardware/Hardware.hpp"
#include "hardware/Component.hpp"
#include "lcdgui/Field.hpp"
#include "disk/AkaiFileRenamer.hpp"
#include "disk/SaveOperation.hpp"
#include "disk/StdDisk.hpp"
#include "disk/MpcFile.hpp"
#include "disk/Volume.hpp"
#include "sampler/Sampler.hpp"
#include "sampler/Sound.hpp"
#include "sampler/Program.hpp"
#include "engine/EngineHost.hpp"
#include "file/kaitai/ApsIo.hpp"
#include "file/kaitai/PgmIo.hpp"
#include "file/kaitai/SndIo.hpp"
#include "file/wav/WavFile.hpp"
#include "lcdgui/Label.hpp"
#include "lcdgui/screens/SaveScreen.hpp"
#include "lcdgui/screens/window/SaveApsFileScreen.hpp"
#include "lcdgui/screens/window/SaveAProgramScreen.hpp"
#include "lcdgui/screens/dialog/FileExistsScreen.hpp"
#include "lcdgui/screens/dialog2/PopupScreen.hpp"

#include <ImageBlockDevice.hpp>
#include <FileSystemFactory.hpp>
#include <fat/AkaiFatFileSystem.hpp>
#include <fat/AkaiFatLfnDirectory.hpp>
#include <util/SuperFloppyFormatter.hpp>
#include <fstream>

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>

using namespace mpc;
using namespace mpc::disk;
using namespace mpc::lcdgui;
using namespace std::chrono_literals;

namespace
{
    struct Trace
    {
        std::mutex mutex;
        std::condition_variable condition;
        std::vector<std::pair<std::string, std::thread::id>> events;
        std::string blockAt, failAt;
        bool entered = false, released = false;
        void record(const std::string &event)
        {
            std::unique_lock lock(mutex);
            events.emplace_back(event, std::this_thread::get_id());
            if (event == blockAt)
            {
                entered = true;
                condition.notify_all();
                condition.wait(lock,
                               [&]
                               {
                                   return released;
                               });
            }
            if (event == failAt)
            {
                throw std::runtime_error("injected " + event);
            }
        }
        void unblock()
        {
            std::lock_guard lock(mutex);
            released = true;
            condition.notify_all();
        }
        bool saw(const std::string &event)
        {
            std::lock_guard lock(mutex);
            for (auto &entry : events)
            {
                if (entry.first == event)
                {
                    return true;
                }
            }
            return false;
        }
    };

    struct TracedDestination : SaveDestination
    {
        std::shared_ptr<Trace> trace;
        TracedDestination(const SaveDestination &base,
                          std::shared_ptr<Trace> trace)
            : SaveDestination(base), trace(std::move(trace))
        {
        }
        void prepare() override
        {
            trace->record("prepare");
            SaveDestination::prepare();
        }
        std::shared_ptr<MpcFile> create(const std::string &name,
                                        bool replace) override
        {
            trace->record("write:" + name);
            return SaveDestination::create(name, replace);
        }
        void flush() override
        {
            trace->record("flush");
            SaveDestination::flush();
        }
        DirectoryListing scan() override
        {
            trace->record("scan");
            return SaveDestination::scan();
        }
    };

    struct TracedDisk : StdDisk
    {
        std::shared_ptr<Trace> trace = std::make_shared<Trace>();
        std::function<std::unique_ptr<SaveDestination>(int)> destinationFactory;
        explicit TracedDisk(Mpc &mpc) : StdDisk(mpc) {}
        std::unique_ptr<SaveDestination>
        captureSaveDestination(int view) override
        {
            auto destination = destinationFactory
                                   ? destinationFactory(view)
                                   : StdDisk::captureSaveDestination(view);
            return std::make_unique<TracedDestination>(*destination, trace);
        }
    };

    struct Fixture
    {
        Mpc mpc;
        std::shared_ptr<TracedDisk> disk;
        SaveOperation::Clock::time_point time{};
        int clockCalls = 0;
        std::unique_ptr<SaveOperation> operation;
        Fixture()
        {
            TestMpc::initializeTestMpcWithoutIoServices(mpc);
            disk = std::make_shared<TracedDisk>(mpc);
            const auto localPath =
                mpc.getDisk()->getVolume().localDirectoryPath;
            auto &volume = disk->getVolume();
            volume.type = LOCAL_DIRECTORY;
            volume.mode = READ_WRITE;
            volume.localDirectoryPath = localPath;
            volume.volumeUUID = "save-test";
            volume.label = "SAVE TEST";
            volume.volumeSize = 1024 * 1024;
            disk->initRoot();
            mpc.getDiskController()->getDisks()[0] = disk;
            mpc.getLayeredScreen()->openScreenById(ScreenId::SaveScreen);
            mpc.getLayeredScreen()->openScreenById(ScreenId::SaveApsFileScreen);
            operation = std::make_unique<SaveOperation>(mpc,
                                                        [this]
                                                        {
                                                            ++clockCalls;
                                                            return time;
                                                        });
            disk->trace->events.clear();
        }
        ~Fixture()
        {
            disk->trace->unblock();
            operation->shutdown();
        }
        std::shared_ptr<sampler::Sound> sound(const std::string &name,
                                              bool mono = true, int frames = 32)
        {
            auto s = mpc.getSampler()->addSound();
            s->setName(name);
            auto samples =
                std::make_shared<std::vector<float>>(frames * (mono ? 1 : 2));
            for (size_t i = 0; i < samples->size(); ++i)
            {
                (*samples)[i] = ((i % 64) + 1) / 128.f;
            }
            s->setSampleData(samples);
            s->setMono(mono);
            return s;
        }
        void pumpUntil(const std::function<bool()> &done,
                       bool advanceTime = true)
        {
            const auto timeout = std::chrono::steady_clock::now() + 5s;
            while (!done() && std::chrono::steady_clock::now() < timeout)
            {
                operation->tick();
                if (advanceTime)
                {
                    time += 1ms;
                }
                std::this_thread::yield();
            }
            REQUIRE(done());
        }
        void finish()
        {
            pumpUntil(
                [&]
                {
                    return !operation->isActive();
                });
        }
        std::shared_ptr<MpcFile> file(const std::string &name)
        {
            return std::make_shared<MpcFile>(
                mpc_fs::path(disk->getAbsolutePath()) / name);
        }
        std::string popup()
        {
            return mpc.screens->get<ScreenId::PopupScreen>()
                ->findChild<Label>("popup")
                ->getText();
        }
    };
} // namespace

TEST_CASE("Managed APS and PGM saves preserve bytes and capture sounds",
          "[save-operation]")
{
    const bool pgm = GENERATE(false, true);
    const int mode = GENERATE(0, 1, 2);
    const bool mono = GENERATE(true, false);
    CAPTURE(pgm, mode, mono);
    Fixture f;
    constexpr int frames = 2051;
    auto sound = f.sound("SAMPLE", mono, frames);
    const auto expectedSound = file::kaitai::SndIo::saveSound(*sound);
    std::vector<char> expectedWav;
    if (mode == 2)
    {
        f.disk->writeWav(sound, "REFERENCE.WAV");
        expectedWav = f.file("REFERENCE.WAV")->getBytes();
        REQUIRE(f.file("REFERENCE.WAV")->del());
        f.disk->initFiles();
        f.disk->trace->events.clear();
    }
    const auto program = f.mpc.getSampler()->getProgram(0);
    program->setUsed();
    program->getNoteParameters(35)->setSoundIndex(0);
    program->getNoteParameters(36)->setSoundIndex(
        0); // Keep repeated references.
    f.mpc.getEngineHost()->prepareProcessBlock(512);
    f.mpc.screens->get<ScreenId::SaveAProgramScreen>()->save = mode;
    const auto expectedHeader =
        pgm ? file::kaitai::PgmIo::saveProgram(*program, f.mpc.getSampler())
            : file::kaitai::ApsIo::save(f.mpc, "SESSION");
    REQUIRE((pgm ? f.operation->startProgram(program, "SESSION.PGM")
                 : f.operation->startAps("SESSION.APS")));
    sound->setName("CHANGED");
    sound->setSampleData(std::make_shared<std::vector<float>>(8, 0.f));
    f.finish();
    CHECK(f.file(pgm ? "SESSION.PGM" : "SESSION.APS")->getBytes() ==
          expectedHeader);
    CHECK(f.mpc.getLayeredScreen()->getCurrentScreenId() ==
          ScreenId::SaveScreen);
    if (mode == 0)
    {
        CHECK_FALSE(f.file("SAMPLE.SND")->exists());
    }
    else if (mode == 1)
    {
        CHECK(f.file("SAMPLE.SND")->getBytes() == expectedSound);
    }
    else
    {
        CHECK(f.file("SAMPLE.WAV")->getBytes() == expectedWav);
        auto wav = file::wav::WavFile::readWavStream(
            f.file("SAMPLE.WAV")->getInputStream());
        REQUIRE(wav);
        CHECK((*wav)->getNumFrames() == frames);
        CHECK((*wav)->getNumChannels() == (mono ? 1 : 2));
        std::vector<float> samples(frames * (mono ? 1 : 2));
        (*wav)->readFrames(samples, frames);
        CHECK(samples.front() > 0.f);
    }
    CHECK_FALSE(f.file("CHANGED.SND")->exists());
    for (const auto &event : f.disk->trace->events)
    {
        CHECK(event.second != std::this_thread::get_id());
    }
}

TEST_CASE(
    "Saving paces presentation after I/O and publishes only at completion",
    "[save-operation]")
{
    Fixture f;
    f.sound("ONE");
    f.sound("TWO");
    const auto initialFiles = f.disk->getFileNames();
    f.disk->trace->blockAt = "write:SESSION.APS";
    REQUIRE(f.operation->startAps("SESSION.APS"));
    f.pumpUntil(
        [&]
        {
            std::lock_guard lock(f.disk->trace->mutex);
            return f.disk->trace->entered;
        },
        false);
    CHECK(f.mpc.getLayeredScreen()->getCurrentScreenId() ==
          ScreenId::SaveApsFileScreen);
    CHECK(f.disk->getFileNames() == initialFiles);
    CHECK_FALSE(f.operation->startAps("SECOND.APS"));
    CHECK_FALSE(f.mpc.fileOperationGate.tryAcquire());
    f.disk
        ->initFiles(); // UI refresh cannot rebuild lists while worker owns I/O.
    CHECK(f.disk->getFileNames() == initialFiles);
    CHECK_FALSE(f.disk->moveBack());
    f.disk->trace->unblock();
    f.pumpUntil(
        [&]
        {
            return f.disk->trace->saw("flush");
        },
        false);
    // The first clock read means the owner received the header result.
    f.pumpUntil(
        [&]
        {
            return f.clockCalls > 0;
        },
        false);
    CHECK_FALSE(f.disk->trace->saw("write:ONE.SND"));
    f.time += f.mpc.getFileOperationTimings().saveTransition;
    f.operation->tick();
    const auto beforeSound = f.clockCalls;
    f.pumpUntil(
        [&]
        {
            return f.disk->trace->saw("write:ONE.SND");
        },
        false);
    CHECK(f.popup() == "Saving ONE             .SND");
    f.pumpUntil(
        [&]
        {
            return f.clockCalls > beforeSound;
        },
        false);
    CHECK_FALSE(f.disk->trace->saw("write:TWO.SND"));
    CHECK(f.disk->getFileNames() == initialFiles);
    f.finish();
    CHECK(f.disk->getFileNames().size() == initialFiles.size() + 3);
    CHECK(f.disk->trace->events.back().first == "scan");
}

TEST_CASE("Sound replacement and skipping retain existing save options",
          "[save-operation]")
{
    const bool replace = GENERATE(false, true);
    Fixture f;
    auto sound = f.sound("SAMPLE");
    std::vector<char> original{'O', 'L', 'D'};
    f.file("SAMPLE.SND")->setFileDataChecked(original);
    f.mpc.screens->get<ScreenId::SaveAProgramScreen>()->replaceSameSounds =
        replace;
    REQUIRE(f.operation->startAps("SESSION.APS"));
    f.finish();
    CHECK(f.file("SAMPLE.SND")->getBytes() ==
          (replace ? file::kaitai::SndIo::saveSound(*sound) : original));
}

TEST_CASE("Save failures stop writes and cannot be replaced by completion",
          "[save-operation]")
{
    const auto failAt =
        GENERATE("prepare", "write:SESSION.APS", "write:ONE.SND",
                 "write:TWO.SND", "flush", "scan");
    CAPTURE(failAt);
    Fixture f;
    f.sound("ONE");
    f.sound("TWO");
    f.sound("THREE");
    f.disk->trace->failAt = failAt;
    REQUIRE(f.operation->startAps("SESSION.APS"));
    f.finish();
    CHECK(f.mpc.getLayeredScreen()->getCurrentScreenId() ==
          ScreenId::PopupScreen);
    CHECK(f.popup() == "I/O error! See logs for info");
    if (std::string(failAt) != "scan")
    {
        CHECK_FALSE(f.disk->trace->saw("write:THREE.SND"));
    }
    for (int i = 0; i < 10; ++i)
    {
        f.time += 1s;
        f.operation->tick();
    }
    CHECK(f.popup() == "I/O error! See logs for info");
    CHECK_FALSE(f.mpc.isManagedSaveActive());
}

TEST_CASE(
    "Collision decisions do not delete files before replacement is authorized",
    "[save-operation]")
{
    Fixture f;
    std::vector<char> original{'O', 'L', 'D'};
    f.file("SESSION.APS")->setFileDataChecked(original);
    bool asked = false;
    REQUIRE(f.operation->startAps("SESSION.APS", false,
                                  [&]
                                  {
                                      asked = true;
                                  }));
    f.finish();
    CHECK(asked);
    CHECK(f.file("SESSION.APS")->getBytes() == original);
    REQUIRE(f.operation->startAps("SESSION.APS", true));
    f.finish();
    CHECK(f.file("SESSION.APS")->getBytes() != original);
}

TEST_CASE("Shutdown drains an authorized save without UI ticks or pacing",
          "[save-operation]")
{
    Fixture f;
    f.sound("ONE");
    f.sound("TWO");
    REQUIRE(f.operation->startAps("SESSION.APS"));
    f.operation->shutdown();
    CHECK(f.file("ONE.SND")->exists());
    CHECK(f.file("TWO.SND")->exists());
    CHECK_FALSE(f.operation->isActive());
    CHECK_FALSE(f.operation->startAps("AFTER.APS"));
    CHECK(f.mpc.getLayeredScreen()->getCurrentScreenId() ==
          ScreenId::SaveApsFileScreen);
}

TEST_CASE("Shutdown cannot authorize a pending overwrite", "[save-operation]")
{
    Fixture f;
    std::vector<char> original{'O', 'L', 'D'};
    f.file("SESSION.APS")->setFileDataChecked(original);
    REQUIRE(f.operation->startAps("SESSION.APS"));
    f.operation->shutdown();
    CHECK(f.file("SESSION.APS")->getBytes() == original);
}

TEST_CASE("Save destination supports a real FAT image without browser state",
          "[save-operation][save-raw]")
{
    const bool pgm = GENERATE(false, true);
    const int mode = GENERATE(1, 2);
    CAPTURE(pgm, mode);
    Fixture f;
    const auto imagePath = mpc_fs::path(f.disk->getAbsolutePath()) / "test.img";
    constexpr int size = 5 * 1024 * 1024;
    {
        std::ofstream initialize(imagePath.string(), std::ios::binary);
        initialize.seekp(size - 1);
        initialize.put(0);
    }
    std::fstream stream(imagePath.string(),
                        std::ios::in | std::ios::out | std::ios::binary);
    auto device = std::make_shared<akaifat::ImageBlockDevice>(stream, size);
    akaifat::SuperFloppyFormatter formatter(device);
    formatter.setVolumeLabel("SAVE TEST");
    formatter.format();
    auto fs = std::unique_ptr<akaifat::fat::AkaiFatFileSystem>(
        dynamic_cast<akaifat::fat::AkaiFatFileSystem *>(
            akaifat::FileSystemFactory::createAkai(device, false)));
    REQUIRE(fs);
    auto directory =
        std::dynamic_pointer_cast<akaifat::fat::AkaiFatLfnDirectory>(
            fs->getRoot());
    auto destination = SaveDestination::fromRaw(directory, {}, true, 0,
                                                [&]
                                                {
                                                    fs->flush();
                                                    stream.flush();
                                                });
    destination->prepare();
    auto sound = f.sound("RAW");
    auto bytes = file::kaitai::SndIo::saveSound(*sound);
    destination->create("RAW.SND", false)->setFileDataChecked(bytes);
    destination->flush();
    auto result = destination->scan();
    REQUIRE(result.files.size() == 1);
    CHECK(result.files.front()->getName() == "RAW.SND");
    CHECK(result.files.front()->getBytes() == bytes);
    REQUIRE(destination->find("raw.snd"));
    auto replacement = bytes;
    replacement.back() ^= 1;
    destination->create("RAW.SND", true)->setFileDataChecked(replacement);
    destination->flush();
    CHECK(destination->scan().files.front()->getBytes() == replacement);
    f.disk->destinationFactory = [directory, &fs, &stream](int view)
    {
        return SaveDestination::fromRaw(directory, {}, true, view,
                                        [&fs, &stream]
                                        {
                                            fs->flush();
                                            stream.flush();
                                        });
    };
    f.mpc.screens->get<ScreenId::SaveAProgramScreen>()->save = mode;
    f.mpc.screens->get<ScreenId::SaveAProgramScreen>()->replaceSameSounds =
        true;
    auto program = f.mpc.getSampler()->getProgram(0);
    program->getNoteParameters(35)->setSoundIndex(0);
    f.mpc.getEngineHost()->prepareProcessBlock(512);
    REQUIRE((pgm ? f.operation->startProgram(program, "RAW.PGM")
                 : f.operation->startAps("RAW.APS")));
    f.finish();
    auto saved = f.disk->captureSaveDestination(0);
    saved->prepare();
    REQUIRE(saved->find(pgm ? "RAW.PGM" : "RAW.APS"));
    const auto expected =
        pgm ? file::kaitai::PgmIo::saveProgram(*program, f.mpc.getSampler())
            : file::kaitai::ApsIo::save(f.mpc, "RAW");
    CHECK(saved->find(pgm ? "RAW.PGM" : "RAW.APS")->getBytes() == expected);
    REQUIRE(saved->find(mode == 1 ? "RAW.SND" : "RAW.WAV"));
    if (mode == 1)
    {
        CHECK(saved->find("RAW.SND")->getBytes() == bytes);
    }
    else
    {
        auto wav = file::wav::WavFile::readWavStream(
            saved->find("RAW.WAV")->getInputStream());
        REQUIRE(wav);
        CHECK((*wav)->getNumFrames() == 32);
    }
}

TEST_CASE("Save screen collision dialogs preserve cancel rename and replace",
          "[save-operation][save-ui]")
{
    const bool pgm = GENERATE(false, true);
    const int action = GENERATE(2, 3, 4);
    CAPTURE(pgm, action);
    Fixture f;
    auto ls = f.mpc.getLayeredScreen();
    const auto filename = pgm ? "TEST.PGM" : "TEST.APS";
    std::vector<char> original{'O', 'L', 'D'};
    f.file(filename)->setFileDataChecked(original);
    f.mpc.screens->get<ScreenId::SaveAProgramScreen>()->save = 0;
    if (pgm)
    {
        auto program = f.mpc.getSampler()->getProgram(0);
        program->setName("TEST");
        program->setUsed();
        f.mpc.getEngineHost()->prepareProcessBlock(512);
        ls->openScreenById(ScreenId::SaveScreen);
        ls->openScreenById(ScreenId::SaveAProgramScreen);
        f.mpc.screens->get<ScreenId::SaveAProgramScreen>()->function(4);
    }
    else
    {
        auto screen = f.mpc.screens->get<ScreenId::SaveApsFileScreen>();
        screen->setFileName("TEST");
        screen->function(4);
    }
    auto *operation = f.mpc.getSaveOperation();
    const auto pump = [&]
    {
        const auto until = std::chrono::steady_clock::now() + 5s;
        while (operation->isActive() &&
               std::chrono::steady_clock::now() < until)
        {
            ls->timerCallback();
            std::this_thread::yield();
        }
        REQUIRE_FALSE(operation->isActive());
    };
    pump();
    REQUIRE(ls->getCurrentScreenId() == ScreenId::FileExistsScreen);
    CHECK(f.file(filename)->getBytes() == original);
    f.mpc.screens->get<ScreenId::FileExistsScreen>()->function(action);
    if (action == 2)
    {
        pump();
        CHECK(ls->getCurrentScreenId() == ScreenId::SaveScreen);
        CHECK(f.file(filename)->getBytes() != original);
    }
    else
    {
        CHECK(ls->getCurrentScreenId() ==
              (action == 3 ? ScreenId::SaveScreen : ScreenId::NameScreen));
        CHECK(f.file(filename)->getBytes() == original);
    }
}

TEST_CASE("A lease held by legacy disk work prevents a managed save",
          "[save-operation]")
{
    Fixture f;
    auto legacyLease = f.mpc.fileOperationGate.tryAcquire();
    REQUIRE(legacyLease);
    CHECK_FALSE(f.operation->startAps("SESSION.APS"));
    legacyLease.reset();
    REQUIRE(f.operation->startAps("SESSION.APS"));
    f.finish();
}

TEST_CASE("Checked local writes surface errors rather than claiming success",
          "[save-operation]")
{
    Fixture f;
    REQUIRE(f.disk->newFolder("BLOCKED.SND"));
    std::vector<char> bytes{'T', 'E', 'S', 'T'};
    CHECK_THROWS(f.file("BLOCKED.SND")->setFileDataChecked(bytes));
    f.disk->getVolume().mode = READ_ONLY;
    REQUIRE(f.operation->startAps("SESSION.APS"));
    f.finish();
    CHECK_FALSE(f.file("SESSION.APS")->exists());
    CHECK(f.popup() == "I/O error! See logs for info");
}

TEST_CASE("Managed save blocks input while keeping button releases balanced",
          "[save-operation][save-ui]")
{
    using client::event::ClientHardwareEvent;
    Fixture f;
    f.sound("ONE");
    f.disk->trace->blockAt = "prepare";
    REQUIRE(f.operation->startAps("SESSION.APS"));
    auto screen = f.mpc.screens->get<ScreenId::SaveApsFileScreen>();
    f.mpc.getLayeredScreen()->setFocus("save");
    const auto options = f.mpc.screens->get<ScreenId::SaveAProgramScreen>();
    const auto original = options->save;
    const auto hardware =
        f.mpc.clientEventController->clientHardwareEventController;
    auto event = ClientHardwareEvent{ClientHardwareEvent::HostInputKeyboard,
                                     ClientHardwareEvent::Type::DataWheelTurn,
                                     {},
                                     hardware::ComponentId::DATA_WHEEL,
                                     {},
                                     1.f,
                                     {}};
    hardware->handleClientHardwareEvent(event);
    CHECK(options->save == original);
    event.componentId = hardware::ComponentId::F4;
    event.type = ClientHardwareEvent::Type::MpcButtonPress;
    hardware->handleClientHardwareEvent(event);
    CHECK(
        f.mpc.getHardware()->getButton(hardware::ComponentId::F4)->isPressed());
    CHECK(f.mpc.getLayeredScreen()->getCurrentScreenId() ==
          ScreenId::SaveApsFileScreen);
    event.type = ClientHardwareEvent::Type::MpcButtonRelease;
    hardware->handleClientHardwareEvent(event);
    CHECK_FALSE(
        f.mpc.getHardware()->getButton(hardware::ComponentId::F4)->isPressed());
    CHECK(f.mpc.getLayeredScreen()->getCurrentScreenId() ==
          ScreenId::SaveApsFileScreen);
    const auto field = screen->findField("save");
    const auto rect = field->getRect();
    const auto gesture =
        input::GestureEvent{input::GestureEvent::Type::UPDATE,
                            input::GestureEvent::Movement::Relative,
                            (rect.L + rect.R) * .5f / LCD_WIDTH,
                            (rect.T + rect.B) * .5f / LCD_HEIGHT,
                            1.f,
                            0,
                            0,
                            hardware::ComponentId::LCD,
                            false,
                            false,
                            false,
                            input::GestureEvent::InputDeviceType::Mouse};
    CHECK(f.mpc.dispatchHostInput(input::HostInputEvent(gesture)) ==
          input::HostInputResult::Handled);
    CHECK(options->save == original);
    f.disk->trace->unblock();
    f.finish();
}

TEST_CASE("Header-only and empty saves retain the header popup delay",
          "[save-operation][save-ui]")
{
    const int mode = GENERATE(0, 1);
    Fixture f;
    f.mpc.screens->get<ScreenId::SaveAProgramScreen>()->save = mode;
    REQUIRE(f.operation->startAps("SESSION.APS"));
    f.pumpUntil(
        [&]
        {
            return f.clockCalls > 0;
        },
        false);
    CHECK(f.popup() == "Saving SESSION.APS");
    f.time += f.mpc.getFileOperationTimings().saveTransition - 1ms;
    f.operation->tick();
    CHECK(f.operation->isActive());
    CHECK_FALSE(f.disk->trace->saw("scan"));
    f.time += 1ms;
    f.pumpUntil(
        [&]
        {
            return !f.operation->isActive();
        },
        false);
    CHECK(f.mpc.getLayeredScreen()->getCurrentScreenId() ==
          ScreenId::SaveScreen);
}

TEST_CASE("Checked directory normalization propagates setup failures",
          "[save-operation]")
{
    Fixture f;
    const auto location = mpc_fs::path(f.disk->getAbsolutePath());
    CHECK_THROWS(AkaiFileRenamer::renameFilesInDirectory(location / "MISSING",
                                                         location / "TEMP"));
    auto bytes = std::vector<char>{'X'};
    f.file("BLOCKED")->setFileDataChecked(bytes);
    CHECK_THROWS(AkaiFileRenamer::renameFilesInDirectory(
        location, location / "BLOCKED" / "TEMP"));
}

TEST_CASE(
    "A removed save destination fails without walking it on the UI thread",
    "[save-operation]")
{
    Fixture f;
    const auto rootPath = mpc_fs::path(f.disk->getAbsolutePath());
    REQUIRE(f.disk->newFolder("TARGET"));
    f.disk->initFiles();
    REQUIRE(f.disk->moveForward("TARGET"));
    REQUIRE(mpc_fs::remove(rootPath / "TARGET"));
    REQUIRE(f.operation->startAps("SESSION.APS"));
    f.finish();
    CHECK(f.popup() == "I/O error! See logs for info");
    CHECK_FALSE(*mpc_fs::exists(rootPath / "SESSION.APS"));
}

TEST_CASE("A save keeps its captured nested destination until publication",
          "[save-operation]")
{
    Fixture f;
    const auto rootPath = mpc_fs::path(f.disk->getAbsolutePath());
    REQUIRE(f.disk->newFolder("TARGET"));
    f.disk->initFiles();
    REQUIRE(f.disk->moveForward("TARGET"));
    f.disk->initFiles();
    f.disk->trace->blockAt = "prepare";
    REQUIRE(f.operation->startAps("SESSION.APS"));
    CHECK_FALSE(f.disk->moveBack());
    CHECK_FALSE(f.disk->moveForward("MISSING"));
    const auto diskIndex = f.mpc.getDiskController()->getActiveDiskIndex();
    f.mpc.getDiskController()->activateDisk(diskIndex + 1);
    CHECK(f.mpc.getDiskController()->getActiveDiskIndex() == diskIndex);
    f.disk->trace->unblock();
    f.finish();
    CHECK(*mpc_fs::exists(rootPath / "TARGET" / "SESSION.APS"));
    CHECK_FALSE(*mpc_fs::exists(rootPath / "SESSION.APS"));
    CHECK(f.disk->getFileNames() == std::vector<std::string>{"SESSION.APS"});
}

TEST_CASE("Mpc destruction joins its save before releasing screens and disk",
          "[save-operation]")
{
    mpc_fs::path destination;
    {
        Mpc mpc;
        TestMpc::initializeTestMpcWithoutIoServices(mpc);
        destination = mpc.getDisk()->getAbsolutePath();
        auto sound = mpc.getSampler()->addSound();
        sound->setName("TEARDOWN");
        sound->setSampleData(std::make_shared<std::vector<float>>(32, .5f));
        mpc.screens->get<ScreenId::SaveAProgramScreen>()->save = 1;
        REQUIRE(mpc.getSaveOperation()->startAps("TEARDOWN.APS"));
    }
    CHECK(*mpc_fs::exists(destination / "TEARDOWN.APS"));
    CHECK(*mpc_fs::exists(destination / "TEARDOWN.SND"));
}
