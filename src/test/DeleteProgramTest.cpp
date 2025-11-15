#include <catch2/catch_test_macros.hpp>

#include "TestMpc.hpp"
#include "sampler/Sampler.hpp"
#include "sequencer/Bus.hpp"
#include "sequencer/Sequencer.hpp"

using namespace mpc;
using namespace mpc::sequencer;

TEST_CASE("Delete program", "[program]")
{
    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);
    auto sampler = mpc.getSampler();
    sampler->createNewProgramAddFirstAvailableSlot();
    sampler->createNewProgramAddFirstAvailableSlot();
    sampler->createNewProgramAddFirstAvailableSlot();
    mpc.getSequencer()
        ->getBus<DrumBus>(BusType::DRUM1)
        ->setProgramIndex(ProgramIndex(0));
    mpc.getSequencer()
        ->getBus<DrumBus>(BusType::DRUM2)
        ->setProgramIndex(ProgramIndex(1));
    mpc.getSequencer()
        ->getBus<DrumBus>(BusType::DRUM3)
        ->setProgramIndex(ProgramIndex(2));
    mpc.getSequencer()
        ->getBus<DrumBus>(BusType::DRUM4)
        ->setProgramIndex(ProgramIndex(3));

    sampler->deleteProgram(sampler->getProgram(2));
    REQUIRE(mpc.getSequencer()->getBus<DrumBus>(BusType::DRUM3)->getProgram() == 1);

    sampler->deleteProgram(sampler->getProgram(0));
    REQUIRE(mpc.getSequencer()->getBus<DrumBus>(BusType::DRUM1)->getProgram() == 1);
}
