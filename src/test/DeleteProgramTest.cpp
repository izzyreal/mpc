#include <catch2/catch_test_macros.hpp>

#include "TestMpc.hpp"
#include "sampler/Sampler.hpp"
#include "sequencer/Bus.hpp"
#include "sequencer/Sequencer.hpp"

TEST_CASE("Delete program", "[program]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);
    auto sampler = mpc.getSampler();
    sampler->createNewProgramAddFirstAvailableSlot();
    sampler->createNewProgramAddFirstAvailableSlot();
    sampler->createNewProgramAddFirstAvailableSlot();
    mpc.getSequencer()->getDrumBus(0)->setProgram(0);
    mpc.getSequencer()->getDrumBus(1)->setProgram(1);
    mpc.getSequencer()->getDrumBus(2)->setProgram(2);
    mpc.getSequencer()->getDrumBus(3)->setProgram(3);

    sampler->deleteProgram(sampler->getProgram(2));
    REQUIRE(mpc.getSequencer()->getDrumBus(2)->getProgram() == 1);

    sampler->deleteProgram(sampler->getProgram(0));
    REQUIRE(mpc.getSequencer()->getDrumBus(0)->getProgram() == 1);
}
