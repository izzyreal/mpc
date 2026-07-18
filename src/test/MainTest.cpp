#include <catch2/catch_test_macros.hpp>

#include "TestMpc.hpp"
#include "sequencer/SequencerStateManager.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Sequence.hpp"

using namespace mpc;

TEST_CASE(
    "File operation timings preserve production defaults and support a uniform "
    "override",
    "[file-operation-timings]")
{
    const FileOperationTimings defaults;
    REQUIRE(defaults.progressDisplay == std::chrono::milliseconds(50));
    REQUIRE(defaults.singleSoundLoadTransition ==
            std::chrono::milliseconds(300));
    REQUIRE(defaults.saveTransition == std::chrono::milliseconds(700));
    REQUIRE(defaults.shortOperationFeedback == std::chrono::milliseconds(400));
    REQUIRE(defaults.ioErrorFeedback == std::chrono::milliseconds(1000));
    REQUIRE(defaults.busyDeviceFeedback == std::chrono::milliseconds(2000));
    REQUIRE(defaults.programMissingSoundPoll == std::chrono::milliseconds(25));
    REQUIRE(defaults.apsMissingSoundPoll == std::chrono::milliseconds(50));

    const auto uniform =
        FileOperationTimings::uniform(std::chrono::milliseconds(10));
    REQUIRE(uniform.progressDisplay == std::chrono::milliseconds(10));
    REQUIRE(uniform.singleSoundLoadTransition == std::chrono::milliseconds(10));
    REQUIRE(uniform.saveTransition == std::chrono::milliseconds(10));
    REQUIRE(uniform.shortOperationFeedback == std::chrono::milliseconds(10));
    REQUIRE(uniform.ioErrorFeedback == std::chrono::milliseconds(10));
    REQUIRE(uniform.busyDeviceFeedback == std::chrono::milliseconds(10));
    REQUIRE(uniform.programMissingSoundPoll == std::chrono::milliseconds(10));
    REQUIRE(uniform.apsMissingSoundPoll == std::chrono::milliseconds(10));
}

TEST_CASE("Mpc is instantiated and booted", "[mpc-boot]")
{
    Mpc mpc;
    TestMpc::initializeTestMpc(mpc);
    REQUIRE(mpc.getSequencer());
    REQUIRE(mpc.getFileOperationTimings().progressDisplay ==
            std::chrono::milliseconds(10));
}

SCENARIO("A Sequence initializes correctly", "[sequence]")
{
    GIVEN("An initialized Sequence")
    {
        Mpc mpc;
        TestMpc::initializeTestMpc(mpc);
        mpc.getSequencer()->getSelectedSequence()->init(1);
        mpc.getSequencer()->getStateManager()->drainQueue();
        REQUIRE(mpc.getSequencer()->getSelectedSequence()->getInitialTempo() ==
                120.0);
    }
}
