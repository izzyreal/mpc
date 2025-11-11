#pragma once
#include "lcdgui/ScreenComponent.hpp"

#include <vector>

namespace mpc::lcdgui
{
    class HorizontalBar2;
}

namespace mpc::sequencer
{
    class TempoChangeEvent;
}

namespace mpc::lcdgui::screens::window
{

    class TempoChangeScreen final : public ScreenComponent
    {

        std::vector<std::shared_ptr<HorizontalBar2>> bars;
        std::vector<std::shared_ptr<sequencer::TempoChangeEvent>>
            visibleTempoChanges;

        void initVisibleEvents();
        void displayTempoChange0() const;
        void displayTempoChange1() const;
        void displayTempoChange2() const;
        void displayTempoChangeOn() const;
        void displayInitialTempo() const;

    public:
        TempoChangeScreen(Mpc &mpc, int layerIndex);

        void open() override;

        void left() override;
        void right() override;
        void function(int j) override;
        void turnWheel(int j) override;
        void down() override;
        void up() override;

    private:
        std::weak_ptr<sequencer::TempoChangeEvent> previous;
        std::weak_ptr<sequencer::TempoChangeEvent> current;
        std::weak_ptr<sequencer::TempoChangeEvent> next;
        int offset = 0;
        void setOffset(int i);

        void init();
    };
} // namespace mpc::lcdgui::screens::window
