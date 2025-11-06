#pragma once
#include <lcdgui/ScreenComponent.hpp>

#include <functional>

namespace mpc::lcdgui::screens::window
{

    class VmpcConvertAndLoadWavScreen : public mpc::lcdgui::ScreenComponent
    {

    public:
        void function(int i) override;

        VmpcConvertAndLoadWavScreen(mpc::Mpc &mpc, int layerIndex);

        void setLoadRoutine(const std::function<void()> &newLoadRoutine);

    private:
        std::function<void()> loadRoutine;
    };
} // namespace mpc::lcdgui::screens::window
