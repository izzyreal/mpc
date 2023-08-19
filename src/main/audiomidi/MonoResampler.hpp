#pragma once

#include <vector>

#include <samplerate.h>

namespace mpc::audiomidi {

    class MonoResampler
    {
    public:
        MonoResampler();

        // Returns number of generated frames
        uint32_t resample(const std::vector<float>& input,
                      std::vector<float>& output,
                      int sourceSampleRate,
                      int maxNumInputFramesToProcess);

        // Returns number of generated frames
        uint32_t wrapUpAndGetRemainder(std::vector<float>& output);

        void reset();

    private:
        SRC_STATE* state;
        SRC_DATA data;
        int srcError = 0;
    };
}
