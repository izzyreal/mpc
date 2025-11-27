#include <catch2/catch_test_macros.hpp>

#include "TestMpc.hpp"
#include "sequencer/NonRtSequencerStateManager.hpp"
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
        const auto seq = mpc.getSequencer()->makeNewSequence(SequenceIndex(0));
        seq->init(1);
        mpc.getSequencer()->getNonRtStateManager()->drainQueue();
        REQUIRE(seq->getInitialTempo() == 120.0);
    }
}
