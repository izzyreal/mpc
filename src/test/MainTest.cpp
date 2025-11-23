#include <catch2/catch_test_macros.hpp>

#include "TestMpc.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/SequenceStateManager.hpp"
#include "sequencer/Transport.hpp"

using namespace mpc;

TEST_CASE("Mpc is instantiated and booted", "[mpc-boot]")
{
    Mpc mpc;
    TestMpc::initializeTestMpc(mpc);
    REQUIRE(mpc.getSequencer());
}

SCENARIO("A Sequence initializes correctly", "[sequence]")
{

    GIVEN("An initialized Sequence")
    {
        Mpc mpc;
        TestMpc::initializeTestMpc(mpc);
        const auto seq = mpc.getSequencer()->makeNewSequence();
        seq->init(1);
        seq->getStateManager()->drainQueue();
        REQUIRE(seq->getInitialTempo() == 120.0);
    }
}
