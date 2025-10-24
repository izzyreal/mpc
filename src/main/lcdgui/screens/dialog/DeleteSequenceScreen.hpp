#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::dialog
{

    class DeleteSequenceScreen
        : public mpc::lcdgui::ScreenComponent
    {

    public:
        void turnWheel(int i) override;
        void function(int i) override;

        DeleteSequenceScreen(mpc::Mpc &mpc, const int layerIndex);

        void open() override;

    private:
        void displaySequenceNumberName();
    };
} // namespace mpc::lcdgui::screens::dialog
