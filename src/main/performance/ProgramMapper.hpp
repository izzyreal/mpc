#pragma once


namespace mpc::sampler
{
    class Program;
}

namespace mpc::performance
{
    class Program;

    void mapSamplerProgramToPerformanceProgram(
            const mpc::sampler::Program& src,
            mpc::performance::Program& dst);
} // namespace mpc::performance
