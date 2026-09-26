#include "catch2/catch_test_macros.hpp"

#include "TestMpc.hpp"
#include "controller/ClientEventController.hpp"
#include "controller/ClientHardwareEventController.hpp"
#include "lcdgui/screens/window/CantFindFileScreen.hpp"
#include "lcdgui/screens/window/LoadApsFileScreen.hpp"
#include "lcdgui/screens/LoadScreen.hpp"
#include <thread>

#include "sampler/Sampler.hpp"
#include "disk/AbstractDisk.hpp"
#include "disk/MpcFile.hpp"
#include "disk/ApsLoader.hpp"

#include <cmrc/cmrc.hpp>
#include <string_view>

#include "engine/EngineHost.hpp"

CMRC_DECLARE(mpctest);

using namespace mpc;
using namespace mpc::disk;
using namespace mpc::lcdgui::screens::window;

void prepareApsResources(Mpc &mpc)
{
    auto disk = mpc.getDisk();

    auto fs = cmrc::mpctest::get_filesystem();

    for (auto &&entry : fs.iterate_directory("test/ApsLoading"))
    {
        auto file = fs.open("test/ApsLoading/" + entry.filename());
        char *data =
            (char *)std::string_view(file.begin(), file.end() - file.begin())
                .data();
        auto newFile = disk->newFile(entry.filename());
        std::vector dataVec(data, data + file.size());
        newFile->setFileData(dataVec);
    }

    disk->initFiles();
}

void doApsTest(Mpc &mpc)
{
    prepareApsResources(mpc);

    auto disk = mpc.getDisk();
    auto apsFile = disk->getFile("ALL_PGMS.APS");

    constexpr bool headless = true;
    ApsLoader::load(mpc, apsFile, headless);
    mpc.getEngineHost()->prepareProcessBlock(512);

    auto p1 = mpc.getSampler()->getProgram(0);
    auto p2 = mpc.getSampler()->getProgram(1);

    REQUIRE(mpc.getSampler()->getProgramCount() == 2);
    REQUIRE(p1->getName() == "PROGRAM1");
    REQUIRE(p2->getName() == "PROGRAM2");

    REQUIRE(mpc.getSampler()->getSoundCount() == 3);
    REQUIRE(mpc.getSampler()->getSoundName(0) == "sound1");
    REQUIRE(mpc.getSampler()->getSoundName(1) == "sound2");
    REQUIRE(mpc.getSampler()->getSoundName(2) == "sound3");

    REQUIRE(p1->getNoteParameters(35)->getSoundIndex() == 0);
    REQUIRE(p1->getNoteParameters(36)->getSoundIndex() == 1);
    REQUIRE(p2->getNoteParameters(35)->getSoundIndex() == 1);
    REQUIRE(p2->getNoteParameters(36)->getSoundIndex() == 2);
}

TEST_CASE("Load APS with 2 programs and 3 sounds", "[load-aps]")
{
    Mpc mpc;
    TestMpc::initializeTestMpc(mpc);
    doApsTest(mpc);
}

namespace
{
    using namespace std::chrono_literals;
    using mpc::lcdgui::ScreenId;

    struct InteractiveApsLoad
    {
        Mpc &mpc;
        explicit InteractiveApsLoad(Mpc &mpc) : mpc(mpc)
        {
            mpc.screens->get<ScreenId::LoadApsFileScreen>()->function(4);
        }

        bool done()
        {
            return bool(mpc.fileOperationGate.tryAcquire());
        }

        bool waitWithoutUi(std::chrono::milliseconds duration)
        {
            const auto until = std::chrono::steady_clock::now() + duration;
            while (!done() && std::chrono::steady_clock::now() < until)
            {
                std::this_thread::sleep_for(1ms);
            }
            return done();
        }

        bool pumpUntil(const std::function<bool()> &condition)
        {
            const auto timeout = std::chrono::steady_clock::now() + 5s;
            while (!condition() && std::chrono::steady_clock::now() < timeout)
            {
                mpc.getLayeredScreen()->timerCallback();
                std::this_thread::sleep_for(1ms);
            }
            return condition();
        }

        ~InteractiveApsLoad()
        {
            // A failing assertion must not leave the disk's worker waiting for
            // a decision while Mpc destruction joins it.
            pumpUntil(
                [&]
                {
                    if (mpc.getLayeredScreen()->getCurrentScreenId() ==
                        ScreenId::CantFindFileScreen)
                    {
                        mpc.screens->get<ScreenId::CantFindFileScreen>()
                            ->function(1);
                    }
                    return done();
                });
            mpc.getLayeredScreen()->timerCallback();
        }
    };
} // namespace

TEST_CASE("Interactive APS loading waits for each missing-file decision",
          "[load-aps][aps-missing-file]")
{
    Mpc mpc;
    TestMpc::initializeTestMpcWithoutIoServices(mpc);
    prepareApsResources(mpc);
    auto disk = mpc.getDisk();
    for (const auto *name : {"SOUND1.SND", "SOUND2.SND", "SOUND3.SND"})
    {
        REQUIRE(disk->getFile(name)->del());
    }
    disk->initFiles();
    const auto ls = mpc.getLayeredScreen();
    ls->openScreenById(ScreenId::LoadScreen);
    mpc.screens->get<ScreenId::LoadScreen>()->setFileLoad(0);
    REQUIRE(mpc.screens->get<ScreenId::LoadScreen>()
                ->getSelectedFile()
                ->getName() == "ALL_PGMS.APS");
    ls->openScreenById(ScreenId::LoadApsFileScreen);
    const auto dialog = mpc.screens->get<ScreenId::CantFindFileScreen>();

    const auto press = [&](hardware::ComponentId id)
    {
        using client::event::ClientHardwareEvent;
        mpc.clientEventController->clientHardwareEventController
            ->handleClientHardwareEvent(
                {ClientHardwareEvent::HostInputGesture,
                 ClientHardwareEvent::Type::ButtonPressAndRelease,
                 {},
                 id,
                 1.f,
                 {},
                 {}});
    };

    const auto waitAtDialog =
        [&](InteractiveApsLoad &load, const std::string &name)
    {
        REQUIRE(load.pumpUntil(
            [&]
            {
                return ls->getCurrentScreenId() ==
                           ScreenId::CantFindFileScreen &&
                       dialog->getFileName() == name;
            }));
        // Keep UI ticks running for several missing-sound polling intervals.
        const auto until = std::chrono::steady_clock::now() + 50ms;
        while (std::chrono::steady_clock::now() < until)
        {
            ls->timerCallback();
            std::this_thread::sleep_for(1ms);
        }
        CHECK_FALSE(load.done());
        CHECK(ls->getCurrentScreenId() == ScreenId::CantFindFileScreen);
        CHECK(dialog->getFileName() == name);
        CHECK(dialog->findField("file")->getText() == name);
        CHECK_FALSE(mpc.fileOperationGate.tryAcquire());
    };

    {
        InteractiveApsLoad load(mpc);
        // Deliberately delay delivery of the dialog-opening UI task. The loader
        // must already be waiting, not race ahead to its completion callback.
        CHECK_FALSE(load.waitWithoutUi(100ms));
        waitAtDialog(load, "sound1");
        press(hardware::ComponentId::F3); // SKIP only this file.
        waitAtDialog(load, "sound2");
        press(hardware::ComponentId::F2); // SKIP ALL includes the third missing
                                          // file.
        REQUIRE(load.pumpUntil(
            [&]
            {
                return load.done();
            }));
    }
    CHECK(ls->getCurrentScreenId() == ScreenId::LoadScreen);

    // SKIP ALL belongs to one load, rather than persisting into the next APS.
    ls->openScreenById(ScreenId::LoadApsFileScreen);
    {
        InteractiveApsLoad load(mpc);
        waitAtDialog(load, "sound1");
        press(hardware::ComponentId::F2);
        REQUIRE(load.pumpUntil(
            [&]
            {
                return load.done();
            }));
    }
}
