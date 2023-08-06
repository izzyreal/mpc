#pragma once

#include <vector>

#include <samplerate.h>

namespace mpc::audiomidi {

    class MonoResampler
    {
    public:
        MonoResampler();
        void resample(const std::vector<float>& input,
                      std::vector<float>& output,
                      int sourceSampleRate,
                      int maxNumInputFramesToProcess);

        std::vector<float> wrapUpAndGetRemainder();

        void reset();

    private:
        SRC_STATE* state;
        SRC_DATA data;
        int srcError = 0;
    };
}
