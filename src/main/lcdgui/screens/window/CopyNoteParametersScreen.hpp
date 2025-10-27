#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::window
{
    class CopyNoteParametersScreen : public mpc::lcdgui::ScreenComponent
    {

    public:
        void turnWheel(int i) override;
        void function(int i) override;

        CopyNoteParametersScreen(mpc::Mpc &mpc, const int layerIndex);
        void open() override;

    private:
        int prog0 = 0;
        int prog1 = 0;
        int note1 = 0;

        void setProg0(int i);
        void setProg1(int i);
        void setNote0(int i);
        void setNote1(int i);

        void displayProg0();
        void displayNote0();
        void displayProg1();
        void displayNote1();
    };
} // namespace mpc::lcdgui::screens::window
