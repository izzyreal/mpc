#include <catch2/catch_test_macros.hpp>

#include "TestMpc.hpp"
#include "sequencer/SequencerStateManager.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Sequence.hpp"

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
        mpc.getSequencer()->getSelectedSequence()->init(1);
        mpc.getSequencer()->getStateManager()->drainQueue();
        REQUIRE(mpc.getSequencer()->getSelectedSequence()->getInitialTempo() ==
                120.0);
    }
}
