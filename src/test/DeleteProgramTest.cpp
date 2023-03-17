#include <catch2/catch_test_macros.hpp>

#include "Mpc.hpp"
#include "sampler/Sampler.hpp"
#include "engine/Drum.hpp"

TEST_CASE("Delete program", "[program]")
{
    mpc::Mpc mpc;
    mpc.init(1, 5);
    auto sampler = mpc.getSampler();
    sampler->createNewProgramAddFirstAvailableSlot();
    sampler->createNewProgramAddFirstAvailableSlot();
    sampler->createNewProgramAddFirstAvailableSlot();
    mpc.getDrum(0).setProgram(0);
    mpc.getDrum(1).setProgram(1);
    mpc.getDrum(2).setProgram(2);
    mpc.getDrum(3).setProgram(3);

    sampler->deleteProgram(sampler->getProgram(2));
    REQUIRE(mpc.getDrum(2).getProgram() == 1);

    sampler->deleteProgram(sampler->getProgram(0));
    REQUIRE(mpc.getDrum(0).getProgram() == 1);

}
