#pragma once
#include "lcdgui/ScreenComponent.hpp"

namespace mpc::lcdgui::screens::dialog
{

    class DeleteTrackScreen final : public ScreenComponent
    {

    public:
        void turnWheel(int i) override;
        void function(int i) override;

        DeleteTrackScreen(Mpc &mpc, int layerIndex);

        void open() override;

    private:
        int tr = 0;
        void setTr(int i);
        void displayTr() const;
    };
} // namespace mpc::lcdgui::screens::dialog
