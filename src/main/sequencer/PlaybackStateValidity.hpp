#pragma once
#include "IntTypes.hpp"

namespace mpc::sequencer
{
    struct RenderContext;
    void
    computeValidity(RenderContext &renderCtx,
                        TimeInSamples currentTime);
} // namespace mpc::sequencer