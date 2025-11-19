#include <catch2/catch_test_macros.hpp>

#include "TestMpc.hpp"
#include "engine/EngineHost.hpp"
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
    mpc.getEngineHost()->applyPendingStateChanges();
    sampler->createNewProgramAddFirstAvailableSlot();
    mpc.getEngineHost()->applyPendingStateChanges();
    sampler->createNewProgramAddFirstAvailableSlot();
    mpc.getEngineHost()->applyPendingStateChanges();

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

    mpc.getEngineHost()->applyPendingStateChanges();

    sampler->deleteProgram(sampler->getProgram(2));
    mpc.getEngineHost()->applyPendingStateChanges();
    REQUIRE(mpc.getSequencer()
                ->getBus<DrumBus>(BusType::DRUM3)
                ->getProgramIndex() == 0);

    sampler->deleteProgram(sampler->getProgram(0));
    mpc.getEngineHost()->applyPendingStateChanges();
    REQUIRE(mpc.getSequencer()
                ->getBus<DrumBus>(BusType::DRUM1)
                ->getProgramIndex() == 1);
}
