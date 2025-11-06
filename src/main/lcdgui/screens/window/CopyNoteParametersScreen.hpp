#pragma once
#include "lcdgui/ScreenComponent.hpp"

namespace mpc::lcdgui::screens::window
{
    class CopyNoteParametersScreen final : public ScreenComponent
    {

    public:
        void turnWheel(int i) override;
        void function(int i) override;

        CopyNoteParametersScreen(Mpc &mpc, int layerIndex);
        void open() override;

    private:
        int prog0 = 0;
        int prog1 = 0;
        int note1 = 0;

        void setProg0(int i);
        void setProg1(int i);
        void setNote0(int i) const;
        void setNote1(int i);

        void displayProg0() const;
        void displayNote0() const;
        void displayProg1() const;
        void displayNote1() const;
    };
} // namespace mpc::lcdgui::screens::window
