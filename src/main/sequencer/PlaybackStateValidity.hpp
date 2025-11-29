#pragma once
#include "IntTypes.hpp"

namespace mpc::sequencer
{
    struct RenderContext;
    void
    computeSafeValidity(RenderContext &renderCtx,
                        TimeInSamples currentTime);
} // namespace mpc::sequencer