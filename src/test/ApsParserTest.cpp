#include <catch2/catch_test_macros.hpp>

#include "TestMpc.hpp"
#include "sampler/Sampler.hpp"
#include "file/aps/ApsParser.hpp"

TEST_CASE("APS with maximum number of sounds", "[apsparser]")
{
    const uint16_t MAX_SOUND_COUNT = 256;

    mpc::Mpc mpc;
    mpc::TestMpc::initializeTestMpc(mpc);

    for (uint16_t i = 0; i < MAX_SOUND_COUNT; i++)
    {
        auto sound = mpc.getSampler()->addSound();
        assert(sound != nullptr);
    }

    assert(mpc.getSampler()->getSoundCount() == MAX_SOUND_COUNT);

    mpc::file::aps::ApsParser apsParserSave(mpc, "FOO");

    mpc.getSampler()->purge();

    assert(mpc.getSampler()->getSoundCount() == 0);

    mpc::file::aps::ApsParser apsParserLoad(apsParserSave.saveBytes);

    REQUIRE(apsParserLoad.getSoundNames().size() == MAX_SOUND_COUNT);
}
