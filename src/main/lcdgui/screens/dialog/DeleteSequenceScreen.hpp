#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::dialog
{

    class DeleteSequenceScreen final : public ScreenComponent
    {

    public:
        void turnWheel(int i) override;
        void function(int i) override;

        DeleteSequenceScreen(Mpc &mpc, int layerIndex);

        void open() override;

    private:
        void displaySequenceNumberName();
    };
} // namespace mpc::lcdgui::screens::dialog
