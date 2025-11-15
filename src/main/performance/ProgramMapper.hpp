#pragma once

#include "performance/Drum.hpp"

namespace mpc::sampler
{
    class Program;
}

namespace mpc::performance
{
    class Program;

    void mapSamplerProgramToPerformanceProgram(
            const mpc::sampler::Program&,
            mpc::performance::Program&);
} // namespace mpc::performance
