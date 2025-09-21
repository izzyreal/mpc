#include <catch2/catch_test_macros.hpp>

#include "sampler/Sound.hpp"
#include "file/sndwriter/SndWriter.hpp"
#include "file/sndreader/SndReader.hpp"

#include "SampleOps.hpp"

using namespace mpc::file::sndwriter;
using namespace mpc::file::sndreader;
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
    snd1.setMono(true);

    auto inputData = snd1.getMutableSampleData();

    for (int i = -32768; i < 32768; i++)
    {
        inputData->emplace_back(short_to_float(static_cast<int16_t>(i)));
    }

    SndWriter sndWriter(&snd1);
    auto& writtenData = sndWriter.getSndFileArray();
    SndReader reader(writtenData);
    auto outputData = std::make_shared<std::vector<float>>();
    reader.readData(outputData);

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
