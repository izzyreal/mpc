#pragma once
#include "lcdgui/ScreenComponent.hpp"

namespace mpc::lcdgui::screens::window
{
    class AssignmentViewScreen final : public ScreenComponent
    {

    public:
        AssignmentViewScreen(Mpc &mpc, int layerIndex);
        void open() override;
        void close() override;
        void update(Observable *o, Message message) override;

        void up() override;
        void down() override;
        void left() override;
        void right() override;
        void turnWheel(int increment) override;

    private:
        const std::vector<std::string> letters{"A", "B", "C", "D"};
        const std::vector<std::string> padFocusNames{
            "a3", "b3", "c3", "d3", "a2", "b2", "c2", "d2",
            "a1", "b1", "c1", "d1", "a0", "b0", "c0", "d0"};
        void displayAssignmentView() const;
        void displayPad(int i) const;
        void displayBankInfoAndNoteLabel() const;
        void displayNote() const;
        void displaySoundName() const;

        int getPadIndexFromFocus() const;
        std::string getFocusFromPadIndex() const;
    };
} // namespace mpc::lcdgui::screens::window
