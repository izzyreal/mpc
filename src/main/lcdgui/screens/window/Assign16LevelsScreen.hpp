#pragma once
#include "IntTypes.hpp"
#include "lcdgui/ScreenComponent.hpp"

namespace mpc::lcdgui::screens::window
{
    class Assign16LevelsScreen final : public ScreenComponent
    {

    public:
        void function(int i) override;
        void turnWheel(int i) override;
        void open() override;

        Assign16LevelsScreen(Mpc &mpc, int layerIndex);

    private:
        const std::vector<std::string> typeNames{"TUNING", "DECAY", "ATTACK",
                                                 "FILTER"};
        const std::vector<std::string> paramNames{"VELOCITY", "NOTE VAR"};

        DrumNoteNumber note{MinDrumNoteNumber};
        int parameter = 0;
        int type = 0;
        int originalKeyPad = 3;

        void displayNote() const;
        void displayParameter() const;
        void displayType() const;
        void displayOriginalKeyPad() const;

    public:
        void setNote(DrumNoteNumber);
        void setParam(int i);
        void setType(int i);
        void setOriginalKeyPad(int i);
        int getOriginalKeyPad() const;
        int getType() const;
        DrumNoteNumber getNote() const;
        int getParameter() const;
    };
} // namespace mpc::lcdgui::screens::window
