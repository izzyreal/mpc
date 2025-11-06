#pragma once
#include "lcdgui/ScreenComponent.hpp"

#include <functional>

namespace mpc::sampler
{
    class Sound;
}

namespace mpc::lcdgui::screens::dialog
{
    class FileExistsScreen final : public ScreenComponent
    {
    public:
        FileExistsScreen(Mpc &mpc, int layerIndex);
        void function(int i) override;
        void numpad(int i) override {}

        void initialize(const std::function<void()> &replaceAction,
                        const std::function<void()> &initializeNameScreen,
                        const std::function<void()> &cancelAction);

    private:
        std::function<void()> replaceAction;
        std::function<void()> initializeNameScreen;
        std::function<void()> cancelAction;
    };
} // namespace mpc::lcdgui::screens::dialog
