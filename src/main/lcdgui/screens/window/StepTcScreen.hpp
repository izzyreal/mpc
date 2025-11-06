#pragma once
#include "lcdgui/ScreenComponent.hpp"

namespace mpc::lcdgui::screens::window
{

    class StepTcScreen final : public ScreenComponent
    {

    public:
        StepTcScreen(Mpc &mpc, int layerIndex);

        void open() override;

        void turnWheel(int i) override;

    private:
        const std::vector<std::string> timingCorrectNames{
            "OFF", "1/8", "1/8(3)", "1/16", "1/16(3)", "1/32", "1/32(3)"};
        void displayTcValue() const;
    };
} // namespace mpc::lcdgui::screens::window
