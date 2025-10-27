#pragma once
#include <lcdgui/ScreenComponent.hpp>

#include <functional>

namespace mpc::sampler
{
    class Sound;
}

namespace mpc::lcdgui::screens::dialog
{
    class FileExistsScreen : public mpc::lcdgui::ScreenComponent
    {
    public:
        FileExistsScreen(mpc::Mpc &mpc, const int layerIndex);
        void function(int i) override;
        void numpad(int i) override {}

        void initialize(std::function<void()> replaceAction,
                        std::function<void()> initializeNameScreen,
                        std::function<void()> cancelAction);

    private:
        std::function<void()> replaceAction;
        std::function<void()> initializeNameScreen;
        std::function<void()> cancelAction;
    };
} // namespace mpc::lcdgui::screens::dialog
