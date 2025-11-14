#pragma once
#include "lcdgui/ScreenComponent.hpp"

namespace mpc::lcdgui::screens::dialog
{

    class CopySequenceScreen final : public ScreenComponent
    {

    public:
        void function(int i) override;
        void turnWheel(int increment) override;

        CopySequenceScreen(Mpc &, int layerIndex);

        void open() override;

    private:
        SequenceIndex sq0{MinSequenceIndex};
        SequenceIndex sq1{MinSequenceIndex};
        void setSq0(SequenceIndex);
        void setSq1(SequenceIndex);

        void displaySq0() const;
        void displaySq1() const;
    };
} // namespace mpc::lcdgui::screens::dialog
