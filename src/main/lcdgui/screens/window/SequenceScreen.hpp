#pragma once
#include "lcdgui/ScreenComponent.hpp"
#include "lcdgui/screens/OpensNameScreen.hpp"

namespace mpc::lcdgui::screens::window
{
    class SequenceScreen final : public ScreenComponent, public OpensNameScreen
    {
    public:
        void openNameScreen() override;
        SequenceScreen(Mpc &mpc, int layerIndex);

        void open() override;
        void function(int i) override;

    private:
        void displaySequenceName();
    };
} // namespace mpc::lcdgui::screens::window
