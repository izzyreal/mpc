#include "MonoResampler.hpp"

#include <cmath>

using namespace mpc::audiomidi;

MonoResampler::MonoResampler()
{
    state = src_new(0, 1, &srcError);
}

void MonoResampler::resample(
        const std::vector<float> &inputData,
        std::vector<float> &output,
        int sourceSampleRate,
        int maxNumInputFramesToProcess)
{
    const auto ratio = 44100.f / sourceSampleRate;

    const int maxOutputSize = ceil(maxNumInputFramesToProcess * ratio);

    output.resize(maxOutputSize);

    data.data_in = &inputData[0];
    data.input_frames = maxNumInputFramesToProcess;
    data.data_out = &output[0];
    data.output_frames = maxOutputSize;
    data.end_of_input = 0;
    data.src_ratio = ratio;

    src_process(state, &data);

    output.resize(data.output_frames_gen);
}

std::vector<float> MonoResampler::wrapUpAndGetRemainder()
{
    std::vector<float> result(10000);
    std::vector<float> dummy{0};

    data.data_in = &dummy[0];
    data.input_frames = 1;
    data.data_out = &result[0];
    data.output_frames = result.size();
    data.end_of_input = 1;

    src_process(state, &data);
    result.resize(data.output_frames_gen);
    return result;
}

void MonoResampler::reset()
{
    src_reset(state);
}
