#pragma once
#include "lcdgui/ScreenComponent.hpp"

namespace mpc::disk
{
    class AllLoader;
}

namespace mpc::file::all
{
    class AllSequencer;
}

namespace mpc::sequencer
{
    class Sequencer;
}

namespace mpc::lcdgui::screens
{
    class SecondSeqScreen final : public ScreenComponent
    {

    public:
        void function(int i) override;
        void turnWheel(int i) override;

    public:
        SecondSeqScreen(Mpc &mpc, int layerIndex);

        void open() override;

        int getSq()
        {
            return sq;
        }

    private:
        int sq = 0;

        void setSq(int i);
        void displaySq() const;
        void displayFunctionKeys() const;

        friend class sequencer::Sequencer;
        friend class disk::AllLoader;
        friend class file::all::AllSequencer;
    };
} // namespace mpc::lcdgui::screens
