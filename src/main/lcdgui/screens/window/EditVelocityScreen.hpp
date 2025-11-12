#pragma once
#include "lcdgui/ScreenComponent.hpp"

#include "lcdgui/screens/WithTimesAndNotes.hpp"

namespace mpc::lcdgui::screens::window
{

    class EditVelocityScreen final : public ScreenComponent,
                                     public WithTimesAndNotes
    {

    public:
        EditVelocityScreen(Mpc &mpc, int layerIndex);

        void setNote0(int i) override;

        void function(int i) override;
        void turnWheel(int i) override;
        void open() override;

    private:
        int editType = 0;
        int value = 0;
        std::vector<std::string> editTypeNames = {"ADD VALUE", "SUB VALUE",
                                                  "MULT VAL%", "SET TO VAL"};

        void setEditType(int i);
        void setValue(int i);
        void displayValue() const;
        void displayEditType() const;

    protected:
        void displayNotes() override;
        void displayTime() override;
    };
} // namespace mpc::lcdgui::screens::window
