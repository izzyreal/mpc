#pragma once

#include "lcdgui/ScreenComponent.hpp"

namespace mpc::lcdgui::screens::window
{
    class LoadASequencePlayScreen final : public ScreenComponent
    {
    public:
        LoadASequencePlayScreen(Mpc &mpc, int layerIndex);

        void open() override;

    private:
        void displayPosition() const;
    };
} // namespace mpc::lcdgui::screens::window
