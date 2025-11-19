#pragma once

#include "performance/Drum.hpp"

namespace mpc::sampler
{
    class Program;
    class NoteParameters;
}

namespace mpc::performance
{
    NoteParameters mapSamplerNoteParametersToPerformanceNoteParameters(const sampler::NoteParameters *);
    void mapSamplerNoteParametersToPerformanceNoteParameters(const sampler::NoteParameters *, NoteParameters &);
    void mapSamplerProgramToPerformanceProgram(const sampler::Program &,
                                               performance::Program &);
} // namespace mpc::performance
