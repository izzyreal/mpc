#pragma once

#include "lcdgui/ScreenId.hpp"

#include <functional>

namespace mpc::lcdgui
{
    class Screens;
}
namespace mpc::lcdgui::screens::window
{
    class CountMetronomeScreen;
}

namespace mpc::sequencer
{
    struct RenderContext;
    struct MetronomeRenderContext
    {
        CountMetronomeScreen *countMetronomeScreen;
        bool isStepEditor;
        bool isRecMainWithoutPlaying;
    };

    MetronomeRenderContext initMetronomeRenderContext(
        const std::function<bool(std::initializer_list<lcdgui::ScreenId>)>
            &isCurrentScreen,
        const std::function<bool()> &isRecMainWithoutPlaying,
        const std::function<std::shared_ptr<lcdgui::Screens>()> &getScreens);

    void renderMetronome(RenderContext &ctx,
                         const MetronomeRenderContext &mctx);
} // namespace mpc::sequencer