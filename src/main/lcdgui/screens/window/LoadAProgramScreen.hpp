#pragma once
#include "lcdgui/ScreenComponent.hpp"

namespace mpc::lcdgui::screens::window
{
    class LoadAProgramScreen final : public ScreenComponent
    {
    public:
        void turnWheel(int i) override;
        void function(int i) override;

        LoadAProgramScreen(Mpc &mpc, int layerIndex);
        void open() override;

        void setLoadReplaceSameSound(bool b);

        bool loadReplaceSameSound = false;

    private:
        void displayLoadReplaceSound() const;
    };
} // namespace mpc::lcdgui::screens::window
