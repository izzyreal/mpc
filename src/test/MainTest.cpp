#include <catch2/catch_test_macros.hpp>

#include "TestMpc.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Sequence.hpp"
#include "sequencer/Transport.hpp"

using namespace mpc;

TEST_CASE("Mpc is instantiated and booted", "[mpc-boot]")
{
    Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);
    REQUIRE(mpc.getSequencer());
}

SCENARIO("A Sequencer initializes correctly", "[sequencer]")
{

    GIVEN("A Sequencer")
    {
        Mpc mpc;
        mpc::TestMpc::initializeTestMpc(mpc);
        auto seq = mpc.getSequencer();
        seq->init();

        REQUIRE(seq->getTransport()->getTempo() == 120);
    }
}

SCENARIO("A Sequence initializes correctly", "[sequence]")
{

    GIVEN("An initialized Sequence")
    {
        Mpc mpc;
        mpc::TestMpc::initializeTestMpc(mpc);
        auto seq = mpc.getSequencer()->makeNewSequence();
        seq->init(1);
        REQUIRE(seq->getInitialTempo() == 120.0);
    }
}
