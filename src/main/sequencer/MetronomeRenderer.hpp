#pragma once

namespace mpc::lcdgui::screens::window
{
    class CountMetronomeScreen;
}

namespace mpc::sequencer
{
    struct RenderContext;
    struct MetronomeRenderContext
    {
        lcdgui::screens::window::CountMetronomeScreen *countMetronomeScreen;
        bool isStepEditor;
        bool isRecMainWithoutPlaying;
    };

    void renderMetronome(RenderContext &ctx, const MetronomeRenderContext &mctx);
} // namespace mpc::sequencer