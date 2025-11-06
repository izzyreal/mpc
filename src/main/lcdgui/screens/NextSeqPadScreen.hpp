#pragma once
#include "lcdgui/ScreenComponent.hpp"

namespace mpc::lcdgui::screens
{
    class NextSeqPadScreen final : public ScreenComponent
    {

    public:
        NextSeqPadScreen(Mpc &mpc, int layerIndex);

        void right() override;
        void function(int i) override;
        void open() override;

        void refreshSeqs() const;

    private:
        const std::vector<std::string> letters{"A", "B", "C", "D"};
        void displayNextSq() const;
        int bankOffset() const;
        void displayBank() const;
        void displaySeqNumbers() const;
        void displaySq() const;
        void displaySeq(int i) const;
        void setSeqColor(int i) const;
        void displayNow0() const;
        void displayNow1() const;
        void displayNow2() const;
    };
} // namespace mpc::lcdgui::screens
