#pragma once
#include "lcdgui/ScreenComponent.hpp"

namespace mpc::lcdgui::screens::window
{
    class LoadASequenceScreen final : public ScreenComponent
    {
    public:
        LoadASequenceScreen(Mpc &mpc, int layerIndex);

        void open() override;
        void turnWheel(int i) override;
        void function(int i) override;

        int loadInto = 0;
        void setLoadInto(int i);

    private:
        void displayLoadInto();
        void displayFile();
    };
} // namespace mpc::lcdgui::screens::window
