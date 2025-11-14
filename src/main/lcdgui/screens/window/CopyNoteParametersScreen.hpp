#pragma once
#include "lcdgui/ScreenComponent.hpp"

namespace mpc::lcdgui::screens::window
{
    class CopyNoteParametersScreen final : public ScreenComponent
    {

    public:
        void turnWheel(int increment) override;
        void function(int i) override;

        CopyNoteParametersScreen(Mpc &mpc, int layerIndex);
        void open() override;

    private:
        ProgramIndex prog0{MinProgramIndex};
        ProgramIndex prog1{MinProgramIndex};
        int note1 = 0;

        void setProg0(ProgramIndex);
        void setProg1(ProgramIndex);
        void setNote0(DrumNoteNumber) const;
        void setNote1(int i);

        void displayProg0() const;
        void displayNote0() const;
        void displayProg1() const;
        void displayNote1() const;
    };
} // namespace mpc::lcdgui::screens::window
