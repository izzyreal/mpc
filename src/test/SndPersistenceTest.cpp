#include <catch2/catch_test_macros.hpp>

#include "sampler/Sound.hpp"
#include "disk/MpcFile.hpp"
#include "file/kaitai/SndIo.hpp"

#include "SampleOps.hpp"

using namespace mpc::sampleops;

TEST_CASE("Sample ops", "[sample-ops]")
{
    for (int i = -32768; i < 32768; i++)
    {
        auto in = static_cast<int16_t>(i);
        float f_out = short_to_float(in);
        int16_t s_out = mean_normalized_float_to_short(f_out);
        REQUIRE(in == s_out);
    }
}

TEST_CASE("Write and read non-destructively", "[snd-persistence]")
{
    mpc::sampler::Sound snd1(44100);
    snd1.setName("snd_persist");
    snd1.setMono(true);

    auto inputData = snd1.getMutableSampleData();

    for (int i = -32768; i < 32768; i++)
    {
        inputData->emplace_back(short_to_float(static_cast<int16_t>(i)));
    }

    const auto writtenData = mpc::file::kaitai::SndIo::saveSound(snd1);
    auto sound = std::make_shared<mpc::sampler::Sound>(44100);
    auto result = mpc::file::kaitai::SndIo::loadBytes(
        writtenData, sound, "snd_persist");
    REQUIRE(result);

    const auto outputData = sound->getSampleData();

    REQUIRE(outputData->size() == snd1.getSampleData()->size());

    bool allTheSame = true;

    for (int i = 0; i < outputData->size(); i++)
    {
        auto v1 = (*inputData)[i];
        auto v2 = (*outputData)[i];
        auto diff = v1 - v2;

        if (diff != 0)
        {
            allTheSame = false;
            break;
        }
    }

    REQUIRE(allTheSame);
}
