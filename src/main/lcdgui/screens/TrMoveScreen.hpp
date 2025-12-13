#pragma once
#include "lcdgui/ScreenComponent.hpp"

namespace mpc::lcdgui::screens
{
    class TrMoveScreen final : public ScreenComponent
    {

    public:
        void turnWheel(int i) override;
        void up() override;
        void down() override;
        void left() override;
        void right() override;
        void function(int i) override;

        TrMoveScreen(Mpc &mpc, int layerIndex);

        void open() override;

    private:
        void displayTrLabels() const;
        void displayTrFields() const;
        void displaySq() const;

        int selectedTrackIndex = NoTrackIndex;
        int currentTrackIndex = 0;

    public:
        bool isSelected() const;
        void goUp();
        void goDown();
        void select();
        void cancel();
        void insert(sequencer::Sequence *s);
    };
} // namespace mpc::lcdgui::screens