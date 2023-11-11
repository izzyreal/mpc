#include "MonoResampler.hpp"

#include <cmath>

using namespace mpc::audiomidi;

MonoResampler::MonoResampler()
{
    state = src_new(0, 1, &srcError);
}

MonoResampler::~MonoResampler()
{
    src_delete(state);
}

uint32_t MonoResampler::resample(
        const std::vector<float> &inputData,
        std::vector<float> &output,
        int sourceSampleRate,
        int maxNumInputFramesToProcess)
{
    const auto ratio = 44100.f / sourceSampleRate;

    data.data_in = &inputData[0];
    data.input_frames = maxNumInputFramesToProcess;
    data.data_out = &output[0];
    data.output_frames = output.size();
    data.end_of_input = 0;
    data.src_ratio = ratio;

    src_process(state, &data);

    return data.output_frames_gen;
}

uint32_t MonoResampler::wrapUpAndGetRemainder(std::vector<float>& output)
{
    std::vector<float> dummy{0};

    data.data_in = &dummy[0];
    data.input_frames = 1;
    data.data_out = &output[0];
    data.output_frames = output.size();
    data.end_of_input = 1;

    src_process(state, &data);
    return data.output_frames_gen;
}

void MonoResampler::reset()
{
    src_reset(state);
}
