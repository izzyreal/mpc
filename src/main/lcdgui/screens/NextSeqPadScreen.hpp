#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens
{
    class NextSeqPadScreen : public mpc::lcdgui::ScreenComponent
    {

    public:
        NextSeqPadScreen(mpc::Mpc &mpc, const int layerIndex);

        void right() override;
        void function(int i) override;
        void open() override;

        void refreshSeqs();

    private:
        const std::vector<std::string> letters{"A", "B", "C", "D"};
        void displayNextSq();
        int bankOffset();
        void displayBank();
        void displaySeqNumbers();
        void displaySq();
        void displaySeq(int i);
        void setSeqColor(int i);
        void displayNow0();
        void displayNow1();
        void displayNow2();
    };
} // namespace mpc::lcdgui::screens
