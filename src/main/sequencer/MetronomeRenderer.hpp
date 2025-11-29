#pragma once

#include "lcdgui/ScreenId.hpp"

#include <functional>

namespace mpc::lcdgui::screens::window
{
    class CountMetronomeScreen;
}

namespace mpc::sequencer
{
    class Sequencer;
    struct RenderContext;
    struct MetronomeRenderContext
    {
        lcdgui::screens::window::CountMetronomeScreen *countMetronomeScreen;
        bool isStepEditor;
        bool isRecMainWithoutPlaying;
    };

    MetronomeRenderContext initMetronomeRenderContext(
        const std::function<bool(std::initializer_list<lcdgui::ScreenId>)>
            &isCurrentScreen,
        const std::function<bool()> &isRecMainWithoutPlaying,
        const Sequencer *);

    void renderMetronome(RenderContext &ctx,
                         const MetronomeRenderContext &mctx);
} // namespace mpc::sequencer